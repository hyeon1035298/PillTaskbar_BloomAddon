// SPDX-License-Identifier: GPL-3.0-only
//
// port::Run: finds the OLD build's symbols in a NEW build of the same module, from the
// two binaries alone. A wrong RVA hooks or calls the wrong code inside explorer, so the
// engine answers only when independent kinds of evidence agree, and everything else
// comes back unported.
//
// 1. Analysis (each build): function starts (unchained .pdata begins, code pointers in
//    relocated data, call/jmp/lea targets outside .pdata), and per function a masked
//    token string: rel32 branches that leave the function, RIP-relative and image-RVA
//    displacements and relocated slots are masked; a masked RIP slot keeps a hash of the
//    import, string or read-only constant it points at. The "shape" masks every
//    displacement and immediate as well.
// 2. Seeds: masked bodies equal 1:1 between the builds, unique in both, with a shape that
//    is unique in both, after strict consensus (an anchor whose slots disagree with any
//    other anchor about one target is dropped).
// 3. Propagation to a fixed point:
//      refs      the target at the same slot of an identical anchor
//      vtslot    the slots of a mapped vftable (same length, no disagreeing known slot)
//      referrer  x maps to x', x is used only by T and x' only by T' (similar bodies)
//    and vftables by RTTI, by slot content or by code references (all must agree);
//    slot content alone decides only when a known slot holds a function of the table's
//    own.
//    Derived pairs with an identical body become anchors.
// 4. Validation: a changed body whose strings, imports or GUIDs were swapped (some only in
//    OLD, some only in NEW) bans the sources that produced it, and propagation reruns.
// 5. Acceptance (R1-R9 below), applied to every candidate; only then a name is Ported.

#include <windows.h>

#include <Zydis.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <cwchar>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "pe.h"
#include "port.h"

namespace port {

namespace {


// --- hashing ---

struct H128 {
    uint64_t a = 0, b = 0;
    bool operator==(const H128& o) const { return a == o.a && b == o.b; }
    bool operator<(const H128& o) const { return a != o.a ? a < o.a : b < o.b; }
};
struct H128Hash {
    size_t operator()(const H128& h) const { return (size_t)(h.a ^ (h.b * 0x9E3779B97F4A7C15ull)); }
};

uint64_t Mix(uint64_t x) {
    x ^= x >> 30;
    x *= 0xBF58476D1CE4E5B9ull;
    x ^= x >> 27;
    x *= 0x94D049BB133111EBull;
    x ^= x >> 31;
    return x;
}

uint64_t Hash64(const void* data, size_t n, uint64_t seed) {
    const uint8_t* p = (const uint8_t*)data;
    uint64_t h = seed ^ (n * 0x9E3779B97F4A7C15ull);
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        uint64_t v;
        memcpy(&v, p + i, 8);
        h = Mix(h ^ v) + 0x632BE59BD9B4E019ull;
    }
    uint64_t tail = 0;
    for (size_t k = 0; i + k < n; k++) tail |= (uint64_t)p[i + k] << (8 * k);
    return Mix(h ^ tail ^ 0xA5A5A5A5ull);
}

bool Stopped(const std::atomic<bool>* cancel) { return cancel && cancel->load(std::memory_order_relaxed); }

// Checked every so many functions in the long loops.
constexpr uint32_t kStopEvery = 512;

H128 Hash128(const void* data, size_t n) { return {Hash64(data, n, 0x243F6A8885A308D3ull), Hash64(data, n, 0x13198A2E03707344ull)}; }

// --- one build ---

// Python-tuple order of the prototype's kind strings: abs < call < jcc < jmp < rip < rva.
enum RefKind : uint8_t { kAbs, kCall, kJcc, kJmp, kRip, kRva };

struct Ref {
    uint32_t off;
    uint32_t tgt;  // RVA; kNoTarget when it leaves the image
    uint8_t kind;
    uint8_t size;
};
constexpr uint32_t kNoTarget = 0xFFFFFFFFu;

struct Func {
    uint32_t start = 0, end = 0;
    uint32_t tokOff = 0;  // into Build::pool, 2 bytes per code byte
    std::vector<Ref> refs;
    std::vector<uint32_t> insnEnds;
    std::vector<uint64_t> stableSems;  // sorted: strings, imports and GUIDs it uses
    H128 tok, shape;
    bool branchIntoHead = false;  // an internal branch lands in its first 5 bytes
    uint32_t Size() const { return end - start; }
};

struct Col {
    uint32_t off, cd, td, chd;
    std::string name;
};

struct Referrers {
    uint32_t first = 0, last = kNoTarget, count = 0;
};

class Build {
public:
    PeImage pe;
    std::vector<uint8_t> pool;
    std::vector<Func> funcs;                       // sorted by start
    std::unordered_map<uint32_t, uint32_t> index;  // start -> funcs[]
    std::unordered_map<H128, std::vector<uint32_t>, H128Hash> byTok;
    std::unordered_map<H128, uint32_t, H128Hash> shapeCount;
    std::unordered_map<uint32_t, Referrers> referrers;
    std::vector<uint64_t> rdataRefd;  // bitmap: RIP targets outside code
    std::unordered_map<uint32_t, std::vector<uint32_t>> ptrAt;  // code target -> data slots holding it
    std::unordered_map<uint32_t, Col> vtCol;                    // table start -> RTTI locator
    std::map<std::tuple<std::string, uint32_t, uint32_t>, std::vector<uint32_t>> rtti;
    std::unordered_map<std::string, std::vector<uint32_t>> rttiByName;
    std::unordered_set<uint32_t> callTargets;  // targets of direct calls
    std::unordered_set<uint32_t> unchainedBegins;

    // expect: the build the caller means (port::Identity), checked before the analysis.
    bool Load(const std::wstring& path, std::wstring& error, const Identity* expect, const std::atomic<bool>* cancel);
    const Func* At(uint32_t start) const {
        auto it = index.find(start);
        return it == index.end() ? nullptr : &funcs[it->second];
    }
    const uint8_t* Tok(const Func& f) const { return pool.data() + f.tokOff; }
    bool RdataRefd(uint32_t rva) const {
        return rva < rdataRefd.size() * 64 && (rdataRefd[rva >> 6] >> (rva & 63) & 1);
    }
    bool VtStart(uint32_t rva) const { return RdataRefd(rva) || vtCol.count(rva); }
    // Slot targets of the table at v, cached: they depend on the build only.
    const std::vector<uint32_t>& VtSlots(uint32_t v) const {
        auto it = slotCache_.find(v);
        if (it != slotCache_.end()) return it->second;
        return slotCache_.emplace(v, ReadSlots(v)).first->second;
    }
    size_t TokCount(const Func& f) const {
        auto it = byTok.find(f.tok);
        return it == byTok.end() ? 0 : it->second.size();
    }
    uint32_t ShapeCount(const Func& f) const {
        auto it = shapeCount.find(f.shape);
        return it == shapeCount.end() ? 0 : it->second;
    }

private:
    ZydisDecoder dec_;
    mutable std::unordered_map<uint32_t, std::vector<uint32_t>> slotCache_;
    std::vector<uint32_t> ReadSlots(uint32_t v) const;
    std::unordered_set<uint32_t> chained_;
    std::vector<std::pair<uint32_t, uint32_t>> pdataRanges_;

    bool InsidePdata(uint32_t v) const;
    bool Semantic(uint32_t tgt, uint32_t size, std::string& out) const;
    bool SemanticStr(uint32_t tgt, std::string& out) const;
    void Analyze(Func& f);
    void ReadRtti();
};

bool GuidLike(const uint8_t* b) { return (b[7] >> 4) >= 1 && (b[7] >> 4) <= 5 && (b[8] & 0xC0) == 0x80; }

bool Printable(unsigned c) { return (c >= 32 && c < 127) || c == 9 || c == 10 || c == 13; }

bool Build::SemanticStr(uint32_t tgt, std::string& out) const {
    if (!pe.IsReadOnlyData(tgt)) return false;
    // ASCII or UTF-16LE literal of at least 4 characters.
    uint32_t e = tgt;
    while (e < tgt + 256 && e < pe.sizeOfImage && pe.U8(e)) e++;
    if (e < tgt + 256 && e < pe.sizeOfImage && e - tgt >= 4) {
        bool ok = true;
        for (uint32_t i = tgt; i < e && ok; i++) ok = Printable(pe.U8(i));
        if (ok) {
            out = "S:" + std::string((const char*)pe.Bytes(tgt, e - tgt), e - tgt);
            return true;
        }
    }
    int n = 0;
    while (n < 128 && (uint64_t)tgt + 2 * n + 1 < pe.sizeOfImage) {
        unsigned c = pe.U16(tgt + 2 * n);
        if (c == 0) break;
        if (!Printable(c)) {
            n = -1;
            break;
        }
        n++;
    }
    if (n >= 4) {
        out = "W:" + std::string((const char*)pe.Bytes(tgt, 2 * n), 2 * n);
        return true;
    }
    return false;
}

// What a RIP-relative target means across a rebuild: an import, a string literal, or
// the bytes of a read-only constant with no pointer in it (GUIDs, floats).
bool Build::Semantic(uint32_t tgt, uint32_t size, std::string& out) const {
    auto it = pe.iat.find(tgt);
    if (it != pe.iat.end()) {
        out = "I:" + it->second;
        return true;
    }
    if (!pe.IsReadOnlyData(tgt)) return false;
    if (SemanticStr(tgt, out)) return true;
    if (size == 1 || size == 2 || size == 4 || size == 8 || size == 16 || size == 32) {
        for (uint64_t r = tgt >= 7 ? tgt - 7 : 0; r < (uint64_t)tgt + size; r++)
            if (pe.IsReloc((uint32_t)r)) return false;
        uint32_t n = (uint32_t)std::min<uint64_t>(size, pe.sizeOfImage - tgt);
        out = "D:" + std::string((const char*)pe.Bytes(tgt, n), n);
        return true;
    }
    return false;
}

void Build::Analyze(Func& f) {
    const uint32_t start = f.start, n = f.end - f.start;
    const uint8_t* code = pe.Bytes(start, n);
    f.tokOff = (uint32_t)pool.size();
    pool.resize(pool.size() + 2 * (size_t)n);
    uint8_t* tok = pool.data() + f.tokOff;
    for (uint32_t i = 0; i < n; i++) {
        tok[2 * i] = 0;
        tok[2 * i + 1] = code[i];
    }
    std::vector<std::pair<uint32_t, uint32_t>> coarse;
    auto mask = [&](uint32_t off, uint32_t size, const std::string* sem) {
        uint8_t hh[4] = {};
        if (sem) {
            uint64_t h = Hash64(sem->data(), sem->size(), 0x6A09E667F3BCC908ull);
            memcpy(hh, &h, 4);
        }
        for (uint32_t k = 0; k < size && off + k < n; k++) {
            tok[2 * (off + k)] = sem ? 2 : 1;
            tok[2 * (off + k) + 1] = sem ? hh[k % 4] : 0;
        }
    };
    ZydisDecodedInstruction ins;
    ZydisDecodedOperand ops[ZYDIS_MAX_OPERAND_COUNT];
    uint32_t pos = 0;
    while (pos < n) {
        if (!ZYAN_SUCCESS(ZydisDecoderDecodeFull(&dec_, code + pos, n - pos, &ins, ops))) break;
        const uint32_t off = pos, size = ins.length;
        f.insnEnds.push_back(off + size);
        const auto cat = ins.meta.category;
        const bool branch =
            cat == ZYDIS_CATEGORY_CALL || cat == ZYDIS_CATEGORY_UNCOND_BR || cat == ZYDIS_CATEGORY_COND_BR;
        for (int k = 0; k < 2; k++)
            if (ins.raw.imm[k].size && !branch) coarse.emplace_back(off + ins.raw.imm[k].offset, ins.raw.imm[k].size / 8);
        if (ins.raw.disp.size && ins.raw.disp.offset) coarse.emplace_back(off + ins.raw.disp.offset, ins.raw.disp.size / 8);
        if (branch && ins.raw.imm[0].size && ins.raw.imm[0].is_relative && ins.operand_count_visible > 0 &&
            ops[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
            int64_t tgt = (int64_t)start + off + size + ins.raw.imm[0].value.s;
            uint8_t kind = cat == ZYDIS_CATEGORY_CALL ? kCall : (cat == ZYDIS_CATEGORY_UNCOND_BR ? kJmp : kJcc);
            if (tgt < (int64_t)start || tgt >= (int64_t)f.end) {
                uint32_t isz = ins.raw.imm[0].size / 8;
                mask(off + ins.raw.imm[0].offset, isz, nullptr);
                uint32_t t = tgt >= 0 && tgt < (int64_t)pe.sizeOfImage ? (uint32_t)tgt : kNoTarget;
                f.refs.push_back({off + ins.raw.imm[0].offset, t, kind, (uint8_t)isz});
            } else if (tgt < (int64_t)start + 5) {
                f.branchIntoHead = true;  // MinHook writes a 5-byte jump there
            }
        }
        if (ins.raw.disp.size == 32) {
            for (int k = 0; k < ins.operand_count_visible; k++) {
                const ZydisDecodedOperand& o = ops[k];
                if (o.type != ZYDIS_OPERAND_TYPE_MEMORY) continue;
                if (o.mem.base == ZYDIS_REGISTER_RIP) {
                    int64_t t64 = (int64_t)start + off + size + o.mem.disp.value;
                    uint32_t tgt = t64 >= 0 && t64 < (int64_t)pe.sizeOfImage ? (uint32_t)t64 : kNoTarget;
                    uint32_t acc = ins.mnemonic == ZYDIS_MNEMONIC_LEA ? 16 : o.size / 8;
                    std::string sem;
                    bool has = tgt != kNoTarget && Semantic(tgt, acc, sem);
                    if (has && (sem[0] == 'S' || sem[0] == 'W' || sem[0] == 'I' ||
                                (sem[0] == 'D' && sem.size() == 18 && GuidLike((const uint8_t*)sem.data() + 2))))
                        f.stableSems.push_back(Hash64(sem.data(), sem.size(), 0xB7E151628AED2A6Bull));
                    mask(off + ins.raw.disp.offset, 4, has ? &sem : nullptr);
                    f.refs.push_back({off + ins.raw.disp.offset, tgt, kRip, 4});
                } else if (o.mem.base != ZYDIS_REGISTER_RSP && o.mem.base != ZYDIS_REGISTER_RBP &&
                           o.mem.base != ZYDIS_REGISTER_NONE) {
                    uint32_t d = (uint32_t)(o.mem.disp.value & 0xFFFFFFFF);
                    if (d >= 0x1000 && d < pe.sizeOfImage) {
                        mask(off + ins.raw.disp.offset, 4, nullptr);
                        f.refs.push_back({off + ins.raw.disp.offset, d, kRva, 4});
                    }
                }
                break;
            }
        }
        pos = off + size;
    }
    auto it = std::lower_bound(pe.relocs.begin(), pe.relocs.end(), start);
    for (; it != pe.relocs.end() && *it < f.end; ++it) {
        uint32_t r = *it;
        if (r + 8 > f.end) break;
        mask(r - start, 8, nullptr);
        uint64_t v = pe.U64(r);
        uint32_t t = v >= pe.imageBase && v - pe.imageBase < pe.sizeOfImage ? (uint32_t)(v - pe.imageBase) : kNoTarget;
        f.refs.push_back({r - start, t, kAbs, 8});
    }
    std::sort(f.refs.begin(), f.refs.end(), [](const Ref& a, const Ref& b) {
        if (a.off != b.off) return a.off < b.off;
        if (a.kind != b.kind) return a.kind < b.kind;
        if (a.tgt != b.tgt) return a.tgt < b.tgt;
        return a.size < b.size;
    });
    std::sort(f.stableSems.begin(), f.stableSems.end());
    f.stableSems.erase(std::unique(f.stableSems.begin(), f.stableSems.end()), f.stableSems.end());
    f.tok = Hash128(tok, 2 * (size_t)n);
    std::vector<uint8_t> shape(tok, tok + 2 * (size_t)n);
    for (size_t i = 0; i < shape.size(); i += 2)
        if (shape[i] == 2) shape[i] = 1, shape[i + 1] = 0;
    for (auto& [o, sz] : coarse)
        for (uint32_t k = o; k < std::min(o + sz, n); k++) shape[2 * k] = 1, shape[2 * k + 1] = 0;
    f.shape = Hash128(shape.data(), shape.size());
}

bool Build::InsidePdata(uint32_t v) const {
    auto it = std::upper_bound(pdataRanges_.begin(), pdataRanges_.end(), std::make_pair(v, UINT32_MAX));
    if (it == pdataRanges_.begin()) return false;
    --it;
    return it->first < v && v < it->second;
}

void Build::ReadRtti() {
    for (uint32_t r : pe.relocs) {
        if (pe.IsExec(r)) continue;
        uint32_t colRva;
        if (!pe.PointerAt(r, colRva) || !pe.Bytes(colRva, 24)) continue;
        uint32_t sig = pe.U32(colRva), off = pe.U32(colRva + 4), cd = pe.U32(colRva + 8), td = pe.U32(colRva + 12),
                 chd = pe.U32(colRva + 16), self = pe.U32(colRva + 20);
        if (sig != 1 || self != colRva || !(td > 0 && td < pe.sizeOfImage) || !(chd > 0 && chd < pe.sizeOfImage))
            continue;
        Col c{off, cd, td, chd, pe.CString(td + 16)};
        rtti[{c.name, off, cd}].push_back(r + 8);
        rttiByName[c.name].push_back(r + 8);
        vtCol[r + 8] = std::move(c);
    }
}

bool Build::Load(const std::wstring& path, std::wstring& error, const Identity* expect,
                 const std::atomic<bool>* cancel) {
    if (!pe.Load(path, error)) return false;
    if (pe.machine != IMAGE_FILE_MACHINE_AMD64) {
        error = L"not an x64 module";
        return false;
    }
    if (expect) {
        wchar_t buf[200];
        if ((expect->timeDateStamp && expect->timeDateStamp != pe.timeDateStamp) ||
            (expect->sizeOfImage && expect->sizeOfImage != pe.sizeOfImage)) {
            swprintf(buf, 200, L"the NEW module is another build (%08X %X, expected %08X %X)", pe.timeDateStamp,
                     pe.sizeOfImage, expect->timeDateStamp, expect->sizeOfImage);
            error = buf;
            return false;
        }
        if (expect->hasPdbId &&
            (!pe.hasCodeView || memcmp(pe.guid, expect->pdbGuid, 16) != 0 || pe.age != expect->pdbAge)) {
            error = L"the NEW module is another build (its RSDS GUID or age differs)";
            return false;
        }
    }
    if (!ZYAN_SUCCESS(ZydisDecoderInit(&dec_, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64))) {
        error = L"Zydis did not start";
        return false;
    }
    std::map<uint32_t, uint32_t> primary;
    for (auto& p : pe.pdata) {
        pdataRanges_.emplace_back(p.begin, p.end);
        if (p.chained) chained_.insert(p.begin);
        else primary.emplace(p.begin, p.end);  // the first entry wins
    }
    std::sort(pdataRanges_.begin(), pdataRanges_.end());
    std::set<uint32_t> starts;
    for (auto& [b, e] : primary)
        if (pe.IsExec(b)) starts.insert(b), unchainedBegins.insert(b);
    std::vector<std::pair<uint32_t, uint32_t>> codePtrs;
    for (uint32_t r : pe.relocs) {
        if (pe.IsExec(r)) continue;
        uint32_t v;
        if (pe.PointerAt(r, v) && pe.IsExec(v)) {
            codePtrs.emplace_back(r, v);
            if (!chained_.count(v)) starts.insert(v);
        }
    }
    uint32_t done = 0;
    for (auto& [b, e] : primary) {
        if (++done % kStopEvery == 0 && Stopped(cancel)) {
            error = L"stopped";
            return false;
        }
        if (!pe.IsExec(b) || !pe.Bytes(b, e - b)) continue;
        Func f;
        f.start = b;
        f.end = e;
        Analyze(f);
        funcs.push_back(std::move(f));
    }
    std::set<uint32_t> extra;
    for (auto& f : funcs)
        for (auto& r : f.refs)
            if ((r.kind == kCall || r.kind == kJmp || r.kind == kRip) && r.tgt != kNoTarget && pe.IsExec(r.tgt) &&
                !starts.count(r.tgt) && !chained_.count(r.tgt) && !InsidePdata(r.tgt))
                extra.insert(r.tgt);
    starts.insert(extra.begin(), extra.end());
    std::vector<uint32_t> srt(starts.begin(), starts.end());
    for (auto& [b, e] : pdataRanges_) srt.push_back(b);
    std::sort(srt.begin(), srt.end());
    srt.erase(std::unique(srt.begin(), srt.end()), srt.end());
    std::unordered_set<uint32_t> have;
    for (auto& f : funcs) have.insert(f.start);
    for (uint32_t s : starts) {
        if (++done % kStopEvery == 0 && Stopped(cancel)) {
            error = L"stopped";
            return false;
        }
        if (have.count(s)) continue;
        auto it = std::upper_bound(srt.begin(), srt.end(), s);
        uint64_t end = it != srt.end() ? *it : (uint64_t)s + 64;
        end = std::min<uint64_t>({end, (uint64_t)s + 4096, (uint64_t)pe.sizeOfImage});
        while (end > (uint64_t)s + 1 && pe.U8((uint32_t)end - 1) == 0xCC) end--;
        if (end <= s || !pe.Bytes(s, (uint32_t)(end - s))) continue;
        Func f;
        f.start = s;
        f.end = (uint32_t)end;
        Analyze(f);
        funcs.push_back(std::move(f));
    }
    std::sort(funcs.begin(), funcs.end(), [](const Func& a, const Func& b) { return a.start < b.start; });
    rdataRefd.assign((pe.sizeOfImage + 63) / 64, 0);
    for (uint32_t i = 0; i < funcs.size(); i++) {
        const Func& f = funcs[i];
        index[f.start] = i;
        byTok[f.tok].push_back(f.start);
        shapeCount[f.shape]++;
        for (auto& r : f.refs) {
            if (r.tgt == kNoTarget) continue;
            Referrers& rr = referrers[r.tgt];
            if (rr.last != f.start) {
                if (!rr.count) rr.first = f.start;
                rr.count++;
                rr.last = f.start;
            }
            if (r.kind == kRip && !pe.IsExec(r.tgt)) rdataRefd[r.tgt >> 6] |= 1ull << (r.tgt & 63);
            if (r.kind == kCall) callTargets.insert(r.tgt);
        }
    }
    for (auto& [r, v] : codePtrs) ptrAt[v].push_back(r);
    ReadRtti();
    return true;
}

// Code pointers of the table at v, up to the next table start (an RTTI slot or a code ref).
std::vector<uint32_t> Build::ReadSlots(uint32_t v) const {
    std::vector<uint32_t> out;
    for (uint32_t p = v; pe.IsReloc(p) && out.size() < 2048; p += 8) {
        if (p != v && (VtStart(p) || vtCol.count(p + 8))) break;
        uint32_t t;
        if (!pe.PointerAt(p, t) || !pe.IsExec(t)) break;
        out.push_back(t);
    }
    return out;
}

// --- similarity: difflib.SequenceMatcher(None, a, b, autojunk=False).ratio() over
// per-instruction tokens ---

class Similarity {
public:
    double Ratio(const Build& ob, const Func& fo, const Build& nb, const Func& fn) {
        std::vector<uint32_t> a = Split(ob, fo), b = Split(nb, fn);
        if (a.empty() || b.empty()) return 0.0;
        b2j_.clear();
        for (uint32_t j = 0; j < b.size(); j++) b2j_[b[j]].push_back(j);
        if (rowA_.size() < b.size() + 1) rowA_.assign(b.size() + 1, 0), rowB_.assign(b.size() + 1, 0);
        size_t matched = 0;
        std::vector<std::array<uint32_t, 4>> queue{{0, (uint32_t)a.size(), 0, (uint32_t)b.size()}};
        while (!queue.empty()) {
            auto [alo, ahi, blo, bhi] = queue.back();
            queue.pop_back();
            uint32_t i, j, k;
            Longest(a, alo, ahi, blo, bhi, i, j, k);
            if (!k) continue;
            matched += k;
            if (alo < i && blo < j) queue.push_back({alo, i, blo, j});
            if (i + k < ahi && j + k < bhi) queue.push_back({i + k, ahi, j + k, bhi});
        }
        return 2.0 * (double)matched / (double)(a.size() + b.size());
    }

private:
    std::unordered_map<H128, uint32_t, H128Hash> ids_;
    std::unordered_map<uint32_t, std::vector<uint32_t>> b2j_;

    std::vector<uint32_t> Split(const Build& b, const Func& f) {
        std::vector<uint32_t> out;
        const uint8_t* tok = b.Tok(f);
        uint32_t p = 0;
        for (uint32_t e : f.insnEnds) {
            H128 h = Hash128(tok + 2 * (size_t)p, 2 * (size_t)(e - p));
            auto it = ids_.try_emplace(h, (uint32_t)ids_.size()).first;
            out.push_back(it->second);
            p = e;
        }
        return out;
    }

    // j2len as two dense rows that are reset through the entries they touched.
    std::vector<uint32_t> rowA_, rowB_, touchedA_, touchedB_;

    void Longest(const std::vector<uint32_t>& a, uint32_t alo, uint32_t ahi, uint32_t blo, uint32_t bhi,
                 uint32_t& besti, uint32_t& bestj, uint32_t& bestsize) {
        besti = alo, bestj = blo, bestsize = 0;
        if (rowA_.size() < bhi + 1) rowA_.assign(bhi + 1, 0), rowB_.assign(bhi + 1, 0);
        std::vector<uint32_t>*cur = &rowA_, *nxt = &rowB_, *curT = &touchedA_, *nxtT = &touchedB_;
        for (uint32_t i = alo; i < ahi; i++) {
            auto it = b2j_.find(a[i]);
            if (it != b2j_.end()) {
                for (uint32_t j : it->second) {
                    if (j < blo) continue;
                    if (j >= bhi) break;
                    uint32_t k = (j ? (*cur)[j - 1] : 0) + 1;
                    (*nxt)[j] = k;
                    nxtT->push_back(j);
                    if (k > bestsize) besti = i - k + 1, bestj = j - k + 1, bestsize = k;
                }
            }
            for (uint32_t j : *curT) (*cur)[j] = 0;
            curT->clear();
            std::swap(cur, nxt);
            std::swap(curT, nxtT);
        }
        for (uint32_t j : *curT) (*cur)[j] = 0;
        curT->clear();
    }
};

// --- the propagation engine ---

enum Tag : uint8_t { kTagAnchor = 1, kTagRefs = 2, kTagVtslot = 4, kTagReferrer = 8 };
enum SrcKind : uint64_t { kSrcA = 1, kSrcV = 2, kSrcR = 3 };

// The independent roots a pair's evidence grew from: a seed anchor, or a table matched by
// RTTI or by slot content. Only "one or several" matters, so a set keeps its two
// smallest ids; two such sets merge to two exactly when the full sets do.
struct Roots {
    uint64_t id[2] = {};
    uint8_t n = 0;
    void Add(uint64_t r) {
        if ((n > 0 && id[0] == r) || (n > 1 && id[1] == r)) return;
        if (n < 2) id[n++] = r;
        else if (r < id[1]) id[1] = r;
        else return;
        if (n == 2 && id[0] > id[1]) std::swap(id[0], id[1]);
    }
    void Add(const Roots& o) {
        for (uint8_t i = 0; i < o.n; i++) Add(o.id[i]);
    }
    size_t size() const { return n; }
};
uint64_t Src(SrcKind k, uint32_t id) { return (uint64_t)k << 32 | id; }
uint64_t Key(uint32_t x, uint32_t y) { return (uint64_t)x << 32 | y; }

struct Cands {
    std::vector<std::pair<uint32_t, uint8_t>> ys;  // NEW candidate, tags
    uint8_t& Add(uint32_t y) {
        for (auto& [v, t] : ys)
            if (v == y) return t;
        ys.emplace_back(y, 0);
        return ys.back().second;
    }
};

void MergeInto(std::vector<uint64_t>& dst, const std::vector<uint64_t>& src) {
    if (src.empty()) return;
    std::vector<uint64_t> out;
    out.reserve(dst.size() + src.size());
    std::set_union(dst.begin(), dst.end(), src.begin(), src.end(), std::back_inserter(out));
    dst.swap(out);
}
void InsertSorted(std::vector<uint64_t>& v, uint64_t x) {
    auto it = std::lower_bound(v.begin(), v.end(), x);
    if (it == v.end() || *it != x) v.insert(it, x);
}

struct VtMapping {
    uint32_t to = 0;
    bool rtti = false, refs = false, slots = false;  // which kinds named this table
};

class Engine {
public:
    Engine(const Build& o, const Build& n, const std::vector<uint32_t>& vtNamed, const std::atomic<bool>* cancel)
        : old_(o), new_(n), cancel_(cancel) {
        std::set<uint32_t> v(vtNamed.begin(), vtNamed.end());
        for (auto& [a, c] : old_.vtCol) v.insert(a);
        vtCands_.assign(v.begin(), v.end());
    }

    void Run();
    bool stopped = false;  // Run ended early on the cancel flag; nothing it holds counts

    std::unordered_map<uint32_t, Cands> ev;
    // Per pair: the sources a contradiction bans, and the roots its evidence grew from. A
    // derived pair inherits the roots of what produced it.
    std::unordered_map<uint64_t, std::vector<uint64_t>> prov;
    std::unordered_map<uint64_t, Roots> roots;
    std::unordered_map<uint32_t, uint32_t> anchors;
    std::unordered_map<uint32_t, VtMapping> vtMap;
    std::unordered_set<uint32_t> seeds, shared, finalBad, conflicted;
    std::unordered_set<uint32_t> d2Known;  // slots that a slot-content match relied on

    double Sim(uint32_t a, uint32_t b) {
        uint64_t k = Key(a, b);
        auto it = simCache_.find(k);
        if (it != simCache_.end()) return it->second;
        const Func* fo = old_.At(a);
        const Func* fn = new_.At(b);
        double r = fo && fn ? sim_.Ratio(old_, *fo, new_, *fn) : 0.0;
        simCache_[k] = r;
        return r;
    }
    Roots RootsOf(uint32_t x, uint32_t y) const {
        static const Roots none;
        auto it = roots.find(Key(x, y));
        return it == roots.end() ? none : it->second;
    }
    bool Single(uint32_t x, uint32_t& y, uint8_t& tags) const {
        auto it = ev.find(x);
        if (it == ev.end() || it->second.ys.size() != 1) return false;
        y = it->second.ys[0].first;
        tags = it->second.ys[0].second;
        return true;
    }
    // RTTI (method d), and slot content (d2).
    bool ByRtti(uint32_t v, uint32_t& to) const;
    // decisive: a known slot holds a function no other OLD slot holds. Only such a
    // match may stand on slots alone (no code references, no RTTI).
    bool BySlots(uint32_t v, uint32_t& to, std::vector<uint32_t>* known = nullptr, bool* decisive = nullptr) const;

private:
    const Build& old_;
    const Build& new_;
    const std::atomic<bool>* cancel_;
    std::vector<uint32_t> vtCands_;
    std::unordered_set<uint32_t> banAnchor_, banVt_, banRef_;
    Similarity sim_;
    std::unordered_map<uint64_t, double> simCache_;

    static bool RefPairs(const Func& fo, const Func& fn, std::vector<std::pair<uint32_t, uint32_t>>& out) {
        out.clear();
        if (fo.refs.size() != fn.refs.size()) return false;
        for (size_t i = 0; i < fo.refs.size(); i++) {
            if (fo.refs[i].off != fn.refs[i].off || fo.refs[i].kind != fn.refs[i].kind) {
                out.clear();
                return false;
            }
            out.emplace_back(fo.refs[i].tgt, fn.refs[i].tgt);
        }
        return true;
    }
    std::unordered_map<uint32_t, uint32_t> Prune(std::unordered_map<uint32_t, uint32_t> a);
    void Propagate(const std::unordered_map<uint32_t, uint32_t>& seeds);
    std::unordered_set<uint32_t> Contradictions();
    void Add(uint32_t x, uint32_t y, uint8_t tag, uint64_t src) {
        ev[x].Add(y) |= tag;
        InsertSorted(prov[Key(x, y)], src);
    }
    void AddRoots(uint32_t x, uint32_t y, const Roots& src) { roots[Key(x, y)].Add(src); }
};

bool Engine::ByRtti(uint32_t v, uint32_t& to) const {
    auto it = old_.vtCol.find(v);
    if (it == old_.vtCol.end()) return false;
    const Col& c = it->second;
    auto m = new_.rtti.find({c.name, c.off, c.cd});
    if (m != new_.rtti.end()) {
        if (m->second.size() != 1) return false;
        to = m->second[0];
        return true;
    }
    auto bases = [](const Build& b, const Col& col) {
        std::vector<std::string> out;
        uint32_t nb = b.pe.U32(col.chd + 8), arr = b.pe.U32(col.chd + 12);
        for (uint32_t i = 0; i < std::min<uint32_t>(nb, 256); i++) {
            uint32_t bcd = b.pe.U32(arr + 4 * i);
            int32_t mdisp = (int32_t)b.pe.U32(bcd + 8), pdisp = (int32_t)b.pe.U32(bcd + 12);
            if (mdisp == (int32_t)col.off && pdisp == -1) out.push_back(b.pe.CString(b.pe.U32(bcd) + 16));
        }
        std::sort(out.begin(), out.end());
        return out;
    };
    auto byName = new_.rttiByName.find(c.name);
    if (byName == new_.rttiByName.end()) return false;
    auto want = bases(old_, c);
    std::vector<uint32_t> cands;
    for (uint32_t x : byName->second)
        if (bases(new_, new_.vtCol.at(x)) == want) cands.push_back(x);
    if (cands.size() != 1) return false;
    to = cands[0];
    return true;
}

// A slot target held by more data slots than this says nothing about one table.
constexpr size_t kCommonSlot = 8;

bool Engine::BySlots(uint32_t v, uint32_t& to, std::vector<uint32_t>* knownOut, bool* decisive) const {
    const auto& so = old_.VtSlots(v);
    if (decisive) *decisive = false;
    if (so.size() < 2) return false;
    // Known slots, without the ones every table shares (_purecall, common thunks): those
    // match any table of the right length.
    std::vector<std::pair<uint32_t, uint32_t>> known;
    bool ownKnown = false;  // a known slot holds a function only this table holds
    for (uint32_t k = 0; k < so.size(); k++) {
        uint32_t y;
        uint8_t t;
        auto users = old_.ptrAt.find(so[k]);
        if (users != old_.ptrAt.end() && users->second.size() > kCommonSlot) continue;
        if (!Single(so[k], y, t)) continue;
        known.emplace_back(k, y);
        ownKnown = ownKnown || (users != old_.ptrAt.end() && users->second.size() == 1);
    }
    if (known.size() < 2) return false;
    // Every NEW table that agrees with each known slot inside its own length. One of the
    // OLD length is a candidate. One of another length is a rival: the real table may
    // have grown or shrunk while a base or derived class's table kept the length and the
    // shared slots, and the known slots cannot tell the two apart. Any table longer than
    // the first known slot holds its target there, so its pointers find them all.
    auto [k0, y0] = known[0];
    std::set<uint32_t> cands;
    bool rival = false;
    auto it = new_.ptrAt.find(y0);
    if (it != new_.ptrAt.end())
        for (uint32_t q : it->second) {
            if (q < 8 * k0) continue;
            uint32_t p = q - 8 * k0;
            if (!new_.VtStart(p)) continue;
            const auto& sn = new_.VtSlots(p);
            if (sn.size() <= k0) continue;  // q sits in a later table
            bool all = true;
            for (auto& [k, y] : known) {
                if (k >= sn.size()) break;
                if (sn[k] != y) {
                    all = false;
                    break;
                }
            }
            if (!all) continue;
            if (sn.size() == so.size()) cands.insert(p);
            else rival = true;
        }
    if (cands.size() != 1 || rival) return false;
    to = *cands.begin();
    // A real table of the same length that overrides an inherited slot, or that shifted
    // its slots, is neither a candidate nor a rival: its overriding slot does not match
    // the known one, and a shifted table does not hold the first known target where it
    // is looked for. When every known slot is inherited, a base table that kept them is
    // then the only candidate. Knowing every slot does not rule that out: the override
    // can sit at a known slot (the OLD function shared with the base, the table's own
    // functions only in common slots), and then that slot drops the real table. Only a
    // known slot of the table's own (its function in no other OLD slot) is not in a base
    // table.
    if (decisive) *decisive = ownKnown;
    if (knownOut)
        for (auto& [k, y] : known) knownOut->push_back(so[k]);
    return true;
}

std::unordered_map<uint32_t, uint32_t> Engine::Prune(std::unordered_map<uint32_t, uint32_t> anchors) {
    std::vector<std::pair<uint32_t, uint32_t>> pairs;
    for (int it = 0; it < 10; it++) {
        std::unordered_map<uint32_t, std::unordered_set<uint32_t>> votes;
        for (auto& [o, n] : anchors) {
            votes[o].insert(n);
            RefPairs(*old_.At(o), *new_.At(n), pairs);
            for (auto& [x, y] : pairs)
                if (x != kNoTarget && y != kNoTarget) votes[x].insert(y);
        }
        std::vector<uint32_t> bad;
        for (auto& [o, n] : anchors) {
            bool b = votes[o].size() > 1;
            RefPairs(*old_.At(o), *new_.At(n), pairs);
            for (auto& [x, y] : pairs) b = b || (x != kNoTarget && y != kNoTarget && votes[x].size() > 1);
            if (b) bad.push_back(o);
        }
        if (bad.empty()) break;
        for (uint32_t o : bad) anchors.erase(o);
    }
    return anchors;
}

void Engine::Propagate(const std::unordered_map<uint32_t, uint32_t>& seedMap) {
    anchors.clear();
    for (auto& [o, n] : seedMap)
        if (!banAnchor_.count(o)) anchors[o] = n;
    ev.clear();
    prov.clear();
    roots.clear();
    vtMap.clear();
    d2Known.clear();
    std::unordered_set<uint32_t> doneAnchor, doneVt;
    std::vector<std::pair<uint32_t, uint32_t>> pairs;
    auto total = [&] {
        size_t s = 0;
        for (auto& [x, c] : ev) s += c.ys.size();
        return s;
    };
    for (int rounds = 1;; rounds++) {
        if (Stopped(cancel_)) {
            stopped = true;
            return;
        }
        auto before = std::make_tuple(anchors.size(), total(), vtMap.size());
        std::vector<std::pair<uint32_t, uint32_t>> fresh;
        for (auto& [o, n] : anchors)
            if (doneAnchor.insert(o).second) fresh.emplace_back(o, n);
        std::sort(fresh.begin(), fresh.end());
        for (auto& [o, n] : fresh) {
            Add(o, n, kTagAnchor, Src(kSrcA, o));
            // A seed is a root of its own; a derived anchor passes on the roots it grew from.
            if (seedMap.count(o)) roots[Key(o, n)].Add(Src(kSrcA, o));
            const Roots from = RootsOf(o, n);
            RefPairs(*old_.At(o), *new_.At(n), pairs);
            for (auto& [x, y] : pairs) {
                if (x == kNoTarget || y == kNoTarget) continue;
                Add(x, y, kTagRefs, Src(kSrcA, o));
                AddRoots(x, y, from);
            }
        }
        if (Stopped(cancel_)) {
            stopped = true;
            return;
        }
        for (uint32_t v : vtCands_) {
            if (vtMap.count(v) || banVt_.count(v)) continue;
            std::set<uint32_t> c;
            VtMapping m;
            uint32_t to;
            if (ByRtti(v, to)) c.insert(to), m.rtti = true;
            auto it = ev.find(v);
            if (it != ev.end()) {
                for (auto& [y, t] : it->second.ys) c.insert(y);
                m.refs = true;
            }
            std::vector<uint32_t> known;
            bool decisive = false;
            // Slots alone decide only when they are decisive.
            if (BySlots(v, to, &known, &decisive) && (decisive || m.refs || m.rtti)) c.insert(to), m.slots = true;
            if (c.size() == 1) {
                m.to = *c.begin();
                vtMap[v] = m;
                if (m.slots && !m.refs && !m.rtti) d2Known.insert(known.begin(), known.end());
            }
        }
        for (auto& [v, m] : vtMap) {
            if (doneVt.count(v)) continue;
            const auto &so = old_.VtSlots(v), &sn = new_.VtSlots(m.to);
            if (so.empty() || so.size() != sn.size()) continue;
            bool any = false, bad = false;
            for (size_t k = 0; k < so.size(); k++) {
                auto a = anchors.find(so[k]);
                if (a == anchors.end()) continue;
                any = true;
                bad = bad || a->second != sn[k];
            }
            if (!any || bad) continue;
            doneVt.insert(v);
            std::vector<uint64_t> srcs{Src(kSrcV, v)};
            auto p = prov.find(Key(v, m.to));
            if (p != prov.end()) MergeInto(srcs, p->second);
            // RTTI and slot content are roots of their own; code references only pass on
            // the roots of the anchors that hold them.
            Roots rs = RootsOf(v, m.to);
            if (m.rtti || m.slots) rs.Add(Src(kSrcV, v));
            for (size_t k = 0; k < so.size(); k++) {
                ev[so[k]].Add(sn[k]) |= kTagVtslot;
                MergeInto(prov[Key(so[k], sn[k])], srcs);
                AddRoots(so[k], sn[k], rs);
            }
        }
        if (Stopped(cancel_)) {
            stopped = true;
            return;
        }
        {
            std::vector<std::pair<uint32_t, uint32_t>> singles;
            for (auto& [x, c] : ev)
                if (c.ys.size() == 1) singles.emplace_back(x, c.ys[0].first);
            std::sort(singles.begin(), singles.end());
            for (auto& [x, y] : singles) {
                auto live = ev.find(x);
                if (live->second.ys.size() != 1) continue;
                auto ro = old_.referrers.find(x);
                auto rn = new_.referrers.find(y);
                if (ro == old_.referrers.end() || rn == new_.referrers.end() || ro->second.count != 1 ||
                    rn->second.count != 1)
                    continue;
                uint32_t t = ro->second.first, t2 = rn->second.first;
                if (banRef_.count(t)) continue;
                if (Sim(t, t2) < 0.6) continue;
                Add(t, t2, kTagReferrer, Src(kSrcR, x));
                if (Key(t, t2) != Key(x, y)) {
                    // References into an unordered_map stay valid when it grows.
                    auto& dst = prov[Key(t, t2)];
                    auto p = prov.find(Key(x, y));
                    if (p != prov.end()) MergeInto(dst, p->second);
                    // The referrer adds no root: it only follows what x -> y stands on.
                    AddRoots(t, t2, RootsOf(x, y));
                }
            }
        }
        for (auto& [x, c] : ev) {
            if (anchors.count(x) || c.ys.size() != 1 || banAnchor_.count(x)) continue;
            uint32_t y = c.ys[0].first;
            const Func* fo = old_.At(x);
            const Func* fn = new_.At(y);
            // Only a body unique in both builds: one candidate behind a common body (a thunk,
            // a getter) may be any of its copies.
            if (fo && fn && fo->tok == fn->tok && old_.TokCount(*fo) == 1 && new_.TokCount(*fn) == 1) anchors[x] = y;
        }
        auto after = std::make_tuple(anchors.size(), total(), vtMap.size());
        if (after == before || rounds > 30) break;
    }
}

std::unordered_set<uint32_t> Engine::Contradictions() {
    std::unordered_set<uint32_t> bad;
    for (auto& [x, c] : ev) {
        if (c.ys.size() != 1) continue;
        uint32_t y = c.ys[0].first;
        const Func* fo = old_.At(x);
        const Func* fn = new_.At(y);
        if (fo && fn) {
            if (fo->tok == fn->tok) continue;
            std::vector<uint64_t> a, b;
            std::set_difference(fo->stableSems.begin(), fo->stableSems.end(), fn->stableSems.begin(),
                                fn->stableSems.end(), std::back_inserter(a));
            std::set_difference(fn->stableSems.begin(), fn->stableSems.end(), fo->stableSems.begin(),
                                fo->stableSems.end(), std::back_inserter(b));
            if (!a.empty() && !b.empty()) bad.insert(x);
        } else if (!old_.pe.IsExec(x) && !new_.pe.IsExec(y) && old_.pe.IsReadOnlyData(x) &&
                   new_.pe.IsReadOnlyData(y)) {
            const uint8_t* bo = old_.pe.Bytes(x, 16);
            const uint8_t* bn = new_.pe.Bytes(y, 16);
            if (!bo || !bn || !GuidLike(bo) || memcmp(bo, bn, 16) == 0) continue;
            bool reloc = false;
            for (uint64_t r = x >= 7 ? x - 7 : 0; r < (uint64_t)x + 16 && !reloc; r++) reloc = old_.pe.IsReloc((uint32_t)r);
            if (!reloc) bad.insert(x);
        }
    }
    return bad;
}

void Engine::Run() {
    std::unordered_map<uint32_t, uint32_t> a0;
    for (auto& f : old_.funcs) {
        auto m = new_.byTok.find(f.tok);
        if (m != new_.byTok.end() && m->second.size() == 1 && old_.TokCount(f) == 1) a0[f.start] = m->second[0];
    }
    auto pruned = Prune(a0);
    if (Stopped(cancel_)) {
        stopped = true;
        return;
    }
    std::unordered_map<uint32_t, uint32_t> seedMap;
    for (auto& [o, n] : pruned)
        if (old_.ShapeCount(*old_.At(o)) == 1 && new_.ShapeCount(*new_.At(n)) == 1) seedMap[o] = n;
    for (auto& [o, n] : seedMap) seeds.insert(o);
    for (int outer = 1;; outer++) {
        Propagate(seedMap);
        if (stopped) return;
        if (outer > 12) break;
        auto bad = Contradictions();
        finalBad = bad;
        bool grew = false;
        for (uint32_t x : bad) {
            uint32_t y = ev[x].ys[0].first;
            for (uint64_t s : prov[Key(x, y)]) {
                uint32_t kind = (uint32_t)(s >> 32), id = (uint32_t)s;
                if (kind == kSrcA) grew |= banAnchor_.insert(id).second;
                else if (kind == kSrcV) grew |= banVt_.insert(id).second;
                else if (kind == kSrcR) grew |= banRef_.insert(x).second;
            }
        }
        if (!grew) break;
    }
    std::unordered_map<uint32_t, std::vector<uint32_t>> claim;
    for (auto& [x, c] : ev) {
        if (c.ys.size() != 1 || !old_.At(x)) continue;
        claim[c.ys[0].first].push_back(x);
    }
    for (auto& [y, xs] : claim)
        if (xs.size() > 1) shared.insert(xs.begin(), xs.end());
    for (auto& [x, c] : ev)
        if (c.ys.size() > 1) conflicted.insert(x);
}

// --- names ---

bool HasTemplateMarks(const std::wstring& n) {
    return n.find(L'<') != std::wstring::npos || n.find(L'`') != std::wstring::npos ||
           n.find(L"lambda") != std::wstring::npos;
}

// A vftable name without its own decoration: "const C::`vftable'{for `I'}" -> "C" (and "I").
// The name always starts with "const "; a later one belongs to a template argument
// ("class std::vector<int> const &").
bool VftableClass(const std::wstring& n, std::wstring& cls, std::wstring& rest) {
    size_t p = n.find(L"::`vftable'");
    if (p == std::wstring::npos) return false;
    size_t s = n.compare(0, 6, L"const ") == 0 ? 6 : 0;
    cls = n.substr(s, p - s);
    rest = n.substr(p + 11);
    return true;
}

// "public: virtual long __cdecl C::M(args)" -> "C". Empty when there is no class.
std::wstring FunctionClass(const std::wstring& n) {
    size_t paren = std::wstring::npos;
    int depth = 0;
    for (size_t i = 0; i < n.size(); i++) {
        if (n[i] == L'<') depth++;
        else if (n[i] == L'>') depth--;
        else if (n[i] == L'(' && depth == 0) {
            paren = i;
            break;
        }
    }
    if (paren == std::wstring::npos) return {};
    std::wstring head = n.substr(0, paren);
    size_t cc = head.rfind(L"__cdecl ");
    if (cc != std::wstring::npos) head = head.substr(cc + 8);
    depth = 0;
    size_t last = std::wstring::npos;
    for (size_t i = 0; i + 1 < head.size(); i++) {
        if (head[i] == L'<') depth++;
        else if (head[i] == L'>') depth--;
        else if (depth == 0 && head[i] == L':' && head[i + 1] == L':') last = i;
    }
    return last == std::wstring::npos ? std::wstring() : head.substr(0, last);
}

// WIL's per-feature globals and helpers: an identifier "Feature_<id>" plus one of WIL's
// suffixes, whether the id is a number or a name (Feature_61276672__descriptor,
// Feature_SystemLightTheme__descriptor, Feature_<id>__private_featureState,
// Feature_<id>__private_IsEnabledDeviceUsageNoInline, Feature_<id>_logged_traits).
bool PerFeatureName(const std::wstring& n) {
    auto idChar = [](wchar_t c) { return iswalnum(c) || c == L'_'; };
    auto endsWith = [](const std::wstring& s, const wchar_t* t) {
        size_t k = wcslen(t);
        return s.size() >= k && s.compare(s.size() - k, k, t) == 0;
    };
    for (size_t p = n.find(L"Feature_"); p != std::wstring::npos; p = n.find(L"Feature_", p + 1)) {
        if (p > 0 && idChar(n[p - 1])) continue;  // part of a longer identifier
        size_t e = p;
        while (e < n.size() && idChar(n[e])) e++;
        std::wstring id = n.substr(p + 8, e - p - 8);  // what follows "Feature_"
        for (const wchar_t* suffix : {L"__descriptor", L"_logged_traits"})
            if (endsWith(id, suffix) && id.size() > wcslen(suffix)) return true;
        size_t priv = id.find(L"__private_");
        if (priv != std::wstring::npos && priv > 0 && priv + 10 < id.size()) return true;
    }
    return false;
}

// WIL feature state (wil::details::FeatureImpl<__WilFeatureTraits_...> and the
// Feature_<id>__ names): one instance of the same code and data per feature, so
// references cannot tell one feature from another.
bool NeverPorted(const std::wstring& n) {
    return n.find(L"wil::details::FeatureImpl<") != std::wstring::npos ||
           n.find(L"__WilFeatureTraits_") != std::wstring::npos || PerFeatureName(n);
}

// MSVC names a lambda's type by a hash of where it sits ("<lambda_3c46...>"), and a
// rebuild renames it whenever that context changes, even when its code stays the same:
// the evidence can find the code while the NEW build has no such name.
bool LambdaName(const std::wstring& n) { return n.find(L"<lambda_") != std::wstring::npos; }

// A data name is "<type> <name>": only the name part says whether the data belongs to a
// template (a static member of one, or a local static of a template function).
bool DataTemplateName(const std::wstring& n) {
    int depth = 0;
    size_t start = 0;
    for (size_t i = 0; i < n.size(); i++) {
        wchar_t c = n[i];
        if (c == L'<' || c == L'`') depth++;
        else if ((c == L'>' || c == L'\'') && depth > 0) depth--;
        else if (c == L' ' && depth == 0) start = i + 1;
    }
    return HasTemplateMarks(n.substr(start));
}

// --- acceptance ---

struct Verdict {
    Outcome outcome = Outcome::NotFound;
    uint32_t rva = 0;
    std::wstring evidence;
};

std::wstring TagText(uint8_t t) {
    std::wstring s;
    auto add = [&](uint8_t bit, const wchar_t* name) {
        if (!(t & bit)) return;
        if (!s.empty()) s += L'+';
        s += name;
    };
    add(kTagAnchor, L"anchor");
    add(kTagRefs, L"refs");
    add(kTagVtslot, L"vtslot");
    add(kTagReferrer, L"referrer");
    return s;
}

int Bits(uint8_t t) { return __builtin_popcount(t); }

class Acceptor {
public:
    Acceptor(const Build& o, const Build& n, Engine& e,
             const std::unordered_map<uint32_t, std::vector<std::wstring>>& names, const std::atomic<bool>* cancel)
        : old_(o), new_(n), eng_(e), names_(names), cancel_(cancel) {}

    void Decide();
    bool stopped = false;  // Decide ended early on the cancel flag
    const Verdict* Of(uint32_t oldRva) const {
        auto it = verdicts_.find(oldRva);
        return it == verdicts_.end() ? nullptr : &it->second;
    }

private:
    const Build& old_;
    const Build& new_;
    Engine& eng_;
    const std::unordered_map<uint32_t, std::vector<std::wstring>>& names_;
    const std::atomic<bool>* cancel_;
    std::unordered_map<uint32_t, Verdict> verdicts_;
    std::unordered_map<uint32_t, uint32_t> vtAccepted_;     // OLD table -> NEW table
    std::unordered_set<uint32_t> newSlotTargets_;            // NEW functions in accepted tables
    // OLD slot function -> (NEW slot function, OLD table) for every slot of an accepted table.
    std::unordered_multimap<uint32_t, std::pair<uint32_t, uint32_t>> slotIndex_;
    // OLD slot address -> (OLD table, slot) for every slot of an accepted table that kept
    // its length.
    std::unordered_map<uint32_t, std::pair<uint32_t, uint32_t>> slotAt_;
    // OLD tables by the class their name gives.
    std::unordered_map<std::wstring, std::vector<uint32_t>> tablesOf_;
    // Names that have an adjustor or vtordisp thunk, as the name without it.
    std::unordered_set<std::wstring> thunked_;
    // OLD target -> the OLD functions that refer to it, built on first use.
    mutable std::unordered_map<uint32_t, std::vector<uint32_t>> refBy_;
    mutable bool refByBuilt_ = false;

    bool AnyName(uint32_t a, bool (*pred)(const std::wstring&)) const {
        auto it = names_.find(a);
        if (it == names_.end()) return false;
        for (auto& n : it->second)
            if (pred(n)) return true;
        return false;
    }
    bool IsVftable(uint32_t a) const {
        return AnyName(a, [](const std::wstring& n) { return n.find(L"`vftable'") != std::wstring::npos; });
    }
    bool TemplateName(uint32_t a) const;
    bool R8Roots(uint32_t a, uint32_t y) const;
    void Vftables();
    void Functions();
    void Globals();
    void NeverPortedNames();
    void Collisions();
    void Thunks();
    bool OwnSlotProof(uint32_t a, uint32_t y) const;
    bool CalleeContradicts(const Func& fo, const Func& fn) const;
    const std::vector<uint32_t>& RefBy(uint32_t x) const;
    bool StillFolded(uint32_t a, uint32_t y) const;
    bool OwnFoldedSlot(const std::wstring& name, uint32_t a, uint32_t y) const;
    struct TableSig {
        std::unordered_map<H128, uint32_t, H128Hash> targets, shapes;
    };
    TableSig oldSigs_, newSigs_;
    static void TableSigs(const Build& b, TableSig& out);
    static std::pair<H128, H128> TableHashes(const Build& b, const std::vector<uint32_t>& slots);
    static bool TableTwin(const Build& b, const std::vector<uint32_t>& slots, const TableSig& sig);

public:
    // a at slot k of an accepted table of the name's own class, y at slot k of its NEW table.
    bool NameSlotProof(const std::wstring& name, uint32_t a, uint32_t y) const;
    // R9: one name of several on the OLD address a goes to y only with this proof.
    bool FoldedProof(const std::wstring& name, uint32_t a, uint32_t y) const {
        return old_.pe.IsExec(a) && old_.At(a) && (StillFolded(a, y) || OwnFoldedSlot(name, a, y));
    }
};

// R8: references that grew from two independent roots, or a reference in a seed itself.
bool Acceptor::R8Roots(uint32_t a, uint32_t y) const {
    if (eng_.RootsOf(a, y).size() >= 2) return true;
    auto p = eng_.prov.find(Key(a, y));
    if (p != eng_.prov.end())
        for (uint64_t src : p->second)
            if ((src >> 32) == kSrcA && eng_.seeds.count((uint32_t)src)) return true;
    return false;
}

bool Acceptor::TemplateName(uint32_t a) const {
    auto it = names_.find(a);
    if (it == names_.end()) return true;  // unnamed: never ported, treat as the strict case
    for (auto& n : it->second) {
        std::wstring cls, rest;
        if (VftableClass(n, cls, rest)) {
            size_t f = rest.find(L"{for `");
            std::wstring iface = f == std::wstring::npos ? L"" : rest.substr(f + 6);
            if (HasTemplateMarks(cls) || iface.find(L'<') != std::wstring::npos ||
                iface.find(L"lambda") != std::wstring::npos)
                return true;
        } else if (HasTemplateMarks(n)) {
            return true;
        }
    }
    return false;
}

// R7: vftables. Every kind that names the table must agree (RTTI, code references,
// slot content), the NEW table sits in a data section, 8-aligned, has the OLD length,
// every slot is a relocated pointer to a function start, and the qword before it is not
// one more slot. A template class needs two kinds.
void Acceptor::TableSigs(const Build& b, TableSig& out) {
    for (uint32_t r : b.pe.relocs) {
        if (b.pe.IsExec(r) || !b.VtStart(r)) continue;
        const auto& sl = b.VtSlots(r);
        if (sl.empty()) continue;
        auto [t, sh] = TableHashes(b, sl);
        out.targets[t]++;
        out.shapes[sh]++;
    }
}

std::pair<H128, H128> Acceptor::TableHashes(const Build& b, const std::vector<uint32_t>& slots) {
    std::vector<uint8_t> shapes;
    for (uint32_t t : slots) {
        const Func* f = b.At(t);
        H128 h = f ? f->shape : H128{t, 0};
        shapes.insert(shapes.end(), (const uint8_t*)&h, (const uint8_t*)&h + sizeof(h));
    }
    return {Hash128(slots.data(), slots.size() * 4), Hash128(shapes.data(), shapes.size())};
}

bool Acceptor::TableTwin(const Build& b, const std::vector<uint32_t>& slots, const TableSig& sig) {
    auto [t, sh] = TableHashes(b, slots);
    auto a = sig.targets.find(t), c = sig.shapes.find(sh);
    return (a != sig.targets.end() && a->second > 1) || (c != sig.shapes.end() && c->second > 1);
}

void Acceptor::Vftables() {
    TableSigs(old_, oldSigs_);
    TableSigs(new_, newSigs_);
    std::unordered_map<uint32_t, std::vector<uint32_t>> onNew;
    for (auto& [v, m] : eng_.vtMap) {
        if (!IsVftable(v)) continue;
        Verdict& vd = verdicts_[v];
        vd.outcome = Outcome::Ambiguous;
        std::set<uint32_t> c{m.to};
        uint32_t toR = 0, toS = 0;
        std::vector<uint32_t> known;
        bool decisive = false;
        bool rtti = eng_.ByRtti(v, toR), slots = eng_.BySlots(v, toS, &known, &decisive);
        auto it = eng_.ev.find(v);
        bool refs = it != eng_.ev.end();
        slots = slots && (decisive || refs || rtti);  // as in Propagate
        if (rtti) c.insert(toR);
        if (slots) c.insert(toS);
        if (refs)
            for (auto& [y, t] : it->second.ys) c.insert(y);
        std::wstring kinds;
        if (rtti) kinds += L"rtti+";
        if (refs) kinds += L"coderefs+";
        if (slots)
            kinds += L"slots(" + std::to_wstring(known.size()) + L"/" + std::to_wstring(old_.VtSlots(v).size()) + L")+";
        if (!kinds.empty()) kinds.pop_back();
        if (c.size() != 1) {
            vd.evidence = L"vftable kinds disagree (" + kinds + L")";
            continue;
        }
        if (eng_.finalBad.count(v)) {
            vd.evidence = L"contradicted";
            continue;
        }
        uint32_t y = m.to;
        const PeSection* sec = new_.pe.SectionOf(y);
        const auto &so = old_.VtSlots(v), &sn = new_.VtSlots(y);
        const wchar_t* why = nullptr;
        if (!sec || sec->exec || y % 8 != 0) why = L"not an aligned data address";
        else if (so.empty() || sn.empty()) why = L"no slots";
        // A table may grow between builds; only a match by code references or RTTI says so.
        else if (so.size() != sn.size() && !refs && !rtti) why = L"slot count changed";
        for (size_t k = 0; !why && k < sn.size(); k++)
            if (!new_.pe.IsReloc(y + 8 * (uint32_t)k) || !new_.At(sn[k])) why = L"a slot is not a function start";
        // The qword before the table: a code pointer in NEW where OLD has none means the
        // table may start earlier (or the match sits a slot late).
        uint32_t before;
        bool codeBeforeNew = y >= 8 && new_.pe.PointerAt(y - 8, before) && new_.pe.IsExec(before);
        bool codeBeforeOld = v >= 8 && old_.pe.PointerAt(v - 8, before) && old_.pe.IsExec(before);
        if (!why && codeBeforeNew && !codeBeforeOld) why = L"a code pointer right before it";
        if (!why && !new_.VtStart(y)) why = L"no code refers to its start";
        if (why) {
            vd.evidence = std::wstring(L"R7 ") + why + L" (" + kinds + L")";
            continue;
        }
        int nk = (int)rtti + (int)refs + (int)slots;
        // A table matched while propagating can end with no kind left (its slots no longer
        // decide by now): it stands on nothing.
        if (nk == 0) {
            vd.evidence = L"no kind of evidence holds at the end";
            continue;
        }
        if (TemplateName(v) && nk < 2) {
            vd.evidence = L"R3 template class, one kind (" + kinds + L")";
            continue;
        }
        // R8 for tables: code references alone, as for globals, need two independent roots
        // or a seed's own reference; otherwise the table stands on one derived match.
        if (nk == 1 && refs && !R8Roots(v, y)) {
            vd.evidence = L"R8 one root of evidence (" + kinds + L")";
            continue;
        }
        // R4 for tables: one whose slots (or the shapes of its slot functions) repeat in
        // another table of either build needs more than its slot content.
        if (!refs && !rtti && (TableTwin(old_, so, oldSigs_) || TableTwin(new_, sn, newSigs_))) {
            vd.evidence = L"R4 twin table, slot content only (" + kinds + L")";
            continue;
        }
        vd.outcome = Outcome::Ported;
        vd.rva = y;
        vd.evidence = kinds;
        onNew[y].push_back(v);
    }
    for (auto& [y, vs] : onNew) {
        if (vs.size() > 1) {
            for (uint32_t v : vs) verdicts_[v] = {Outcome::Ambiguous, 0, L"R5 two tables on one NEW table"};
            continue;
        }
        vtAccepted_[vs[0]] = y;
        for (uint32_t t : new_.VtSlots(y)) newSlotTargets_.insert(t);
        // A slot proves a name only while the table keeps its length: a table that grew or
        // shrank belongs to a changed interface, where one slot can hold a renamed method.
        const auto &so = old_.VtSlots(vs[0]), &sn = new_.VtSlots(y);
        if (so.size() == sn.size())
            for (size_t k = 0; k < so.size(); k++) {
                slotIndex_.emplace(so[k], std::make_pair(sn[k], vs[0]));
                slotAt_[vs[0] + 8 * (uint32_t)k] = {vs[0], (uint32_t)k};
            }
    }
    for (auto& [v, ns] : names_)
        for (auto& n : ns) {
            std::wstring cls, rest;
            if (VftableClass(n, cls, rest)) tablesOf_[cls].push_back(v);
            if (n.compare(0, 8, L"[thunk]:") == 0) {
                // "[thunk]:<head>`adjustor{8}' (args)" -> "<head>(args)"
                std::wstring t = n.substr(8);
                for (const wchar_t* mark : {L"`adjustor{", L"`vtordisp{", L"`vtordispex{"}) {
                    size_t p = t.find(mark);
                    size_t e = p == std::wstring::npos ? p : t.find(L"}' ", p);
                    if (e != std::wstring::npos) t.erase(p, e + 3 - p);
                }
                thunked_.insert(t);
            }
        }
}

// No contradiction for a same body: each direct call or jump out of it goes, in NEW, to
// the one candidate its OLD target has. An overload whose signature changed can keep its
// masked body while what it calls moves to another function.
bool Acceptor::CalleeContradicts(const Func& fo, const Func& fn) const {
    if (fo.refs.size() != fn.refs.size()) return true;
    for (size_t i = 0; i < fo.refs.size(); i++) {
        const Ref& r = fo.refs[i];
        if ((r.kind != kCall && r.kind != kJmp) || r.tgt == kNoTarget || r.tgt == fo.start) continue;
        uint32_t c;
        uint8_t t;
        if (eng_.Single(r.tgt, c, t) && c != fn.refs[i].tgt) return true;
    }
    return false;
}

bool Acceptor::OwnSlotProof(uint32_t a, uint32_t y) const {
    auto it = names_.find(a);
    if (it == names_.end()) return false;
    for (auto& n : it->second)
        if (NameSlotProof(n, a, y)) return true;
    return false;
}

bool Acceptor::NameSlotProof(const std::wstring& name, uint32_t a, uint32_t y) const {
    std::wstring want = FunctionClass(name);
    if (want.empty()) return false;
    auto range = slotIndex_.equal_range(a);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.first != y) continue;
        auto nv = names_.find(it->second.second);
        if (nv == names_.end()) continue;
        for (auto& vn : nv->second) {
            std::wstring cls, rest;
            if (VftableClass(vn, cls, rest) && cls == want) return true;
        }
    }
    return false;
}

const std::vector<uint32_t>& Acceptor::RefBy(uint32_t x) const {
    if (!refByBuilt_) {
        refByBuilt_ = true;
        for (auto& f : old_.funcs)
            for (auto& r : f.refs)
                if (r.tgt != kNoTarget) {
                    auto& v = refBy_[r.tgt];
                    if (v.empty() || v.back() != f.start) v.push_back(f.start);
                }
    }
    static const std::vector<uint32_t> none;
    auto it = refBy_.find(x);
    return it == refBy_.end() ? none : it->second;
}

// R9 (a): every use of a that the binary shows goes to y in NEW. Code that refers to a
// is an anchor whose NEW body refers to y there, every relocated pointer to a is a slot
// of an accepted table that kept its length and holds y there, and a is not exported.
// Then each name of the set that anything uses is still at y.
bool Acceptor::StillFolded(uint32_t a, uint32_t y) const {
    for (auto& e : old_.pe.exports)
        if (e.rva == a) return false;
    size_t uses = 0;
    for (uint32_t t : RefBy(a)) {
        auto an = eng_.anchors.find(t);
        if (an == eng_.anchors.end()) return false;
        const Func* fo = old_.At(t);
        const Func* fn = new_.At(an->second);
        if (!fo || !fn || fo->refs.size() != fn->refs.size()) return false;
        for (size_t i = 0; i < fo->refs.size(); i++) {
            if (fo->refs[i].tgt != a) continue;
            if (fn->refs[i].tgt != y) return false;
            uses++;
        }
    }
    auto p = old_.ptrAt.find(a);
    if (p != old_.ptrAt.end())
        for (uint32_t slot : p->second) {
            auto s = slotAt_.find(slot);
            if (s == slotAt_.end() || new_.VtSlots(vtAccepted_.at(s->second.first))[s->second.second] != y) return false;
            uses++;
        }
    return uses > 0;
}

// R9 (b): one name of the set by its own slot. A virtual C::f that is defined (it is in
// the set) and has no adjustor or vtordisp thunk sits directly in a slot of one of C's
// tables. When a fills exactly one slot over all of C's tables, that slot is C::f's
// whatever the other names are, and it proves y when its table has no other name, is
// accepted, kept its length and holds y there. A class in an anonymous namespace may
// share its name with another one, so it never qualifies.
bool Acceptor::OwnFoldedSlot(const std::wstring& name, uint32_t a, uint32_t y) const {
    std::wstring cls = FunctionClass(name);
    auto tabs = tablesOf_.find(cls);
    if (cls.empty() || cls.find(L'`') != std::wstring::npos || name.find(L"virtual ") == std::wstring::npos ||
        tabs == tablesOf_.end() || thunked_.count(name))
        return false;
    uint32_t hits = 0, hv = 0, hk = 0;
    for (uint32_t v : tabs->second) {
        const auto& so = old_.VtSlots(v);
        for (uint32_t k = 0; k < so.size(); k++)
            if (so[k] == a) hits++, hv = v, hk = k;
    }
    auto to = vtAccepted_.find(hv);
    if (hits != 1 || names_.at(hv).size() != 1 || to == vtAccepted_.end()) return false;
    const auto& sn = new_.VtSlots(to->second);
    return sn.size() == old_.VtSlots(hv).size() && sn[hk] == y;
}

// R1-R6 and R9 for functions.
void Acceptor::Functions() {
    uint32_t done = 0;
    for (auto& fo : old_.funcs) {
        if (++done % kStopEvery == 0 && Stopped(cancel_)) {
            stopped = true;
            return;
        }
        uint32_t a = fo.start;
        if (!names_.count(a)) continue;
        Verdict vd;
        auto it = eng_.ev.find(a);
        if (it == eng_.ev.end()) {
            vd.evidence = L"no evidence";
            verdicts_[a] = vd;
            continue;
        }
        vd.outcome = Outcome::Ambiguous;
        if (it->second.ys.size() > 1) {
            vd.evidence = L"conflicting evidence";
            verdicts_[a] = vd;
            continue;
        }
        if (eng_.shared.count(a)) {
            vd.evidence = L"R5 another function claims the same NEW function";
            verdicts_[a] = vd;
            continue;
        }
        if (eng_.finalBad.count(a)) {
            vd.evidence = L"contradicted";
            verdicts_[a] = vd;
            continue;
        }
        uint32_t y = it->second.ys[0].first;
        uint8_t tags = it->second.ys[0].second;
        std::wstring kinds = TagText(tags);
        const Func* fn = new_.At(y);
        if (!fn) {
            vd.evidence = L"R6 not a function start (" + kinds + L")";
            verdicts_[a] = vd;
            continue;
        }
        bool same = fo.tok == fn->tok;
        if (!same && eng_.Sim(a, y) < 0.6) {
            vd.evidence = L"bodies differ too much (" + kinds + L")";
            verdicts_[a] = vd;
            continue;
        }
        if (same) {
            bool tainted = false;
            for (auto& r : fo.refs) tainted = tainted || eng_.conflicted.count(r.tgt) || eng_.shared.count(r.tgt);
            if (tainted) {
                vd.evidence = L"same body, but it refers to a contested target (" + kinds + L")";
                verdicts_[a] = vd;
                continue;
            }
            if (CalleeContradicts(fo, *fn)) {
                vd.evidence = L"same body, but a call goes elsewhere than its target maps (" + kinds + L")";
                verdicts_[a] = vd;
                continue;
            }
        }
        // Proof: the masked body is identical and unique, and its shape is unique, in both builds.
        // What it cannot see: an overload whose signature changed while its body did not
        // (the new argument is never read, or goes on in a register to a callee that maps
        // to its own same-body overload). Nothing in the two binaries tells the two
        // apart, so such a name is ported to the new overload (taskbar 9444->9549:
        // CTaskBand::_UpdateItemIcon(ITaskGroup*,ITaskItem*) to the 3-argument one).
        // Requiring a second root here does not help (that one had five), and dropping
        // the proof for one root loses required names (TaskbarHost::FrameHeight).
        bool proof = same && old_.TokCount(fo) == 1 && new_.TokCount(*fn) == 1 && old_.ShapeCount(fo) == 1 &&
                     new_.ShapeCount(*fn) == 1;
        // R2: two kinds of evidence that grew from two independent roots, or the proof.
        // Referrer alone never decides.
        if ((Bits(tags) < 2 || eng_.RootsOf(a, y).size() < 2) && !proof) {
            vd.evidence = std::wstring(Bits(tags) < 2 ? L"R2 one kind of evidence (" : L"R2 one root of evidence (") +
                          kinds + L")";
            verdicts_[a] = vd;
            continue;
        }
        // R3: template and lambda names only through a structural proof. A unique same
        // body is not one: another instance of the template can take over the old code.
        if (TemplateName(a) && !OwnSlotProof(a, y)) {
            vd.evidence = L"R3 template name without a structural proof (" + kinds + L")";
            verdicts_[a] = vd;
            continue;
        }
        // R6: where the NEW function starts, its size, and its first five bytes.
        bool pdataStart = new_.unchainedBegins.count(y) && new_.pe.IsExec(y);
        if (!pdataStart) {
            bool reached = newSlotTargets_.count(y) || new_.callTargets.count(y);
            bool padded = y % 16 == 0 || (y > 0 && (new_.pe.U8(y - 1) == 0xCC || new_.pe.U8(y - 1) == 0x90));
            if (!reached || !padded || !new_.pe.IsExec(y)) {
                vd.evidence = L"R6 leaf start not proven (" + kinds + L")";
                verdicts_[a] = vd;
                continue;
            }
        }
        if (fn->branchIntoHead) {
            vd.evidence = L"R6 a branch lands in the first five bytes (" + kinds + L")";
            verdicts_[a] = vd;
            continue;
        }
        double ratio = (double)fn->Size() / (double)std::max<uint32_t>(fo.Size(), 1);
        if (ratio < 0.5 || ratio > 2.0) {
            vd.evidence = L"R6 size changed " + std::to_wstring(fo.Size()) + L"->" + std::to_wstring(fn->Size());
            verdicts_[a] = vd;
            continue;
        }
        vd.outcome = Outcome::Ported;
        vd.rva = y;
        vd.evidence = kinds + (same ? L", same body" : L", changed body") + (proof ? L", proof" : L"");
        verdicts_[a] = vd;
    }
    // Accepted tables that kept their length must agree with every answer about their
    // slots, both ways: OLD slot a -> NEW slot y, and NEW slot y <- OLD slot a.
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> fwd, back;
    for (auto& [v, to] : vtAccepted_) {
        const auto &so = old_.VtSlots(v), &sn = new_.VtSlots(to);
        if (so.size() != sn.size()) continue;
        for (size_t k = 0; k < so.size(); k++) fwd[so[k]].insert(sn[k]), back[sn[k]].insert(so[k]);
    }
    for (auto& [a, vd] : verdicts_) {
        if (vd.outcome != Outcome::Ported || !old_.At(a)) continue;
        auto f = fwd.find(a);
        auto b = back.find(vd.rva);
        bool bad = (f != fwd.end() && (f->second.size() != 1 || !f->second.count(vd.rva))) ||
                   (b != back.end() && (b->second.size() != 1 || !b->second.count(a)));
        if (bad) vd = {Outcome::Ambiguous, 0, L"an accepted vftable slot disagrees (" + vd.evidence + L")"};
    }
    // R4: twins (equal masked body or equal shape, in OLD or in NEW) need evidence that
    // is not about the body, and distinct NEW functions; otherwise the whole group is out.
    std::unordered_map<H128, std::vector<uint32_t>, H128Hash> groups;
    auto group = [&](const Build& b, const Func& f, char side, std::vector<H128>& keys) {
        if (b.TokCount(f) > 1) keys.push_back({f.tok.a ^ (uint64_t)side, f.tok.b});
        if (b.ShapeCount(f) > 1) keys.push_back({f.shape.a ^ (uint64_t)(side + 2), f.shape.b});
    };
    std::unordered_map<uint32_t, std::vector<H128>> keysOf;
    for (auto& [a, vd] : verdicts_) {
        if (vd.outcome != Outcome::Ported) continue;
        const Func* fo = old_.At(a);
        if (!fo) continue;
        const Func* fn = new_.At(vd.rva);
        std::vector<H128> keys;
        group(old_, *fo, 'o', keys);
        group(new_, *fn, 'n', keys);
        for (auto& k : keys) groups[k].push_back(a);
        keysOf[a] = std::move(keys);
    }
    std::unordered_set<uint32_t> bad;
    for (auto& [a, keys] : keysOf) {
        if (keys.empty()) continue;
        const Verdict& vd = verdicts_[a];
        uint8_t tags = eng_.ev[a].ys[0].second;
        bool distinguishing = (tags & (kTagRefs | kTagReferrer)) ||
                              ((tags & kTagVtslot) && !eng_.d2Known.count(a));
        if (!distinguishing) bad.insert(a);
        for (auto& k : keys)
            for (uint32_t b : groups[k])
                if (b != a && verdicts_[b].rva == vd.rva) bad.insert(a), bad.insert(b);
    }
    // Spread a failure over every group it belongs to.
    std::vector<uint32_t> work(bad.begin(), bad.end());
    while (!work.empty()) {
        uint32_t a = work.back();
        work.pop_back();
        for (auto& k : keysOf[a])
            for (uint32_t b : groups[k])
                if (bad.insert(b).second) work.push_back(b);
    }
    for (uint32_t a : bad) {
        Verdict& vd = verdicts_[a];
        vd.outcome = Outcome::Ambiguous;
        vd.evidence = L"R4 twin group without distinguishing evidence";
        vd.rva = 0;
    }
}

// R8: globals by exact-address references only, from data section to the same data section.
void Acceptor::Globals() {
    for (auto& [a, ns] : names_) {
        if (old_.pe.IsExec(a) || IsVftable(a) || verdicts_.count(a)) continue;
        const PeSection* so = old_.pe.SectionOf(a);
        if (!so || so->exec) continue;
        Verdict vd;
        auto it = eng_.ev.find(a);
        if (it == eng_.ev.end()) continue;  // no evidence: NotFound at lookup
        vd.outcome = Outcome::Ambiguous;
        if (eng_.finalBad.count(a)) vd.evidence = L"contradicted";
        else if (it->second.ys.size() != 1) vd.evidence = L"conflicting references";
        else {
            uint32_t y = it->second.ys[0].first;
            const PeSection* sn = new_.pe.SectionOf(y);
            if (!sn || sn->exec || sn->name != so->name) {
                vd.evidence = L"R8 not in the same data section";
            } else if (AnyName(a, DataTemplateName)) {
                // R3: references are the only evidence for data, and no structure proves
                // one instance of a template's data from another.
                vd.evidence = L"R3 template name, data has no structural proof (refs)";
            } else if (!R8Roots(a, y)) {
                vd.evidence = L"R8 one root of evidence (refs)";
            } else {
                vd.outcome = Outcome::Ported;
                vd.rva = y;
                vd.evidence = L"refs";
            }
        }
        verdicts_[a] = vd;
    }
}

// R5 across kinds: one NEW RVA, one OLD RVA.
void Acceptor::Collisions() {
    std::unordered_map<uint32_t, std::vector<uint32_t>> on;
    for (auto& [a, vd] : verdicts_)
        if (vd.outcome == Outcome::Ported) on[vd.rva].push_back(a);
    for (auto& [y, as] : on)
        if (as.size() > 1)
            for (uint32_t a : as) verdicts_[a] = {Outcome::Ambiguous, 0, L"R5 two names on one NEW RVA"};
}

void Acceptor::NeverPortedNames() {
    for (auto& [a, vd] : verdicts_) {
        if (vd.outcome != Outcome::Ported) continue;
        if (AnyName(a, NeverPorted)) vd = {Outcome::Ambiguous, 0, L"WIL feature state is never ported"};
        else if (AnyName(a, LambdaName)) vd = {Outcome::Ambiguous, 0, L"a lambda's name is never ported"};
    }
}

// An adjustor or vtordisp thunk ("[thunk]:C::f`adjustor{8}'") only moves this and jumps
// to C::f: its name is C::f's, so it is ported only with C::f, and the NEW thunk must
// jump to where C::f went.
void Acceptor::Thunks() {
    for (auto& [a, vd] : verdicts_) {
        if (vd.outcome != Outcome::Ported || !AnyName(a, [](const std::wstring& n) {
                return n.compare(0, 8, L"[thunk]:") == 0;
            }))
            continue;
        const Func* fo = old_.At(a);
        const Func* fn = new_.At(vd.rva);
        if (!fo || !fn) continue;
        auto onlyJump = [](const Func& f) -> const Ref* {
            const Ref* j = nullptr;
            for (auto& r : f.refs)
                if (r.kind == kJmp) {
                    if (j) return nullptr;
                    j = &r;
                }
            return j;
        };
        const Ref* jo = onlyJump(*fo);
        const Ref* jn = onlyJump(*fn);
        if (!jo || jo->tgt == kNoTarget) continue;  // not one tail jump: no target to follow
        auto t = verdicts_.find(jo->tgt);
        if (!jn || t == verdicts_.end() || t->second.outcome != Outcome::Ported || t->second.rva != jn->tgt)
            vd = {Outcome::Ambiguous, 0,
                  L"a thunk is ported only with the function it jumps to (" + vd.evidence + L")"};
    }
}

void Acceptor::Decide() {
    Vftables();
    Functions();
    if (stopped) return;
    Globals();
    NeverPortedNames();
    Collisions();
    Thunks();
}

}  // namespace

bool Run(const Input& in, std::vector<Result>& out, Stats& stats, std::wstring& error) {
    auto t0 = std::chrono::steady_clock::now();
    out.clear();
    auto ob = std::make_unique<Build>(), nb = std::make_unique<Build>();
    // NEW first: a build other than the one asked for fails before any analysis.
    if (!nb->Load(in.newDll, error, &in.expectNew, in.cancel)) return false;
    if (!ob->Load(in.oldDll, error, nullptr, in.cancel)) return false;
    auto t1 = std::chrono::steady_clock::now();
    stats.functionsOld = (uint32_t)ob->funcs.size();
    stats.functionsNew = (uint32_t)nb->funcs.size();

    std::unordered_map<std::wstring, uint32_t> byName, firstAt;
    std::unordered_map<uint32_t, std::vector<std::wstring>> names;
    std::vector<uint32_t> vtNamed;
    // A name the OLD PDB has at more than one address (two instances of one name, such as
    // classes in different anonymous namespaces): no single OLD function to port.
    std::unordered_set<std::wstring> several;
    for (auto& s : in.oldSymbols) {
        auto [f, fresh] = firstAt.emplace(s.name, s.rva);
        if (!fresh && f->second != s.rva) several.insert(s.name);
    }
    for (auto& s : in.oldSymbols) {
        if (s.rva >= ob->pe.sizeOfImage) continue;
        if (!byName.emplace(s.name, s.rva).second) continue;
        names[s.rva].push_back(s.name);
        if (s.name.find(L"`vftable'") != std::wstring::npos) vtNamed.push_back(s.rva);
    }
    Engine eng(*ob, *nb, vtNamed, in.cancel);
    eng.Run();
    Acceptor acc(*ob, *nb, eng, names, in.cancel);
    if (!eng.stopped) acc.Decide();
    if (eng.stopped || acc.stopped) {
        error = L"stopped";
        return false;
    }
    auto t2 = std::chrono::steady_clock::now();

    for (auto& w : in.wanted) {
        Result r;
        r.name = w.name;
        auto it = byName.find(w.name);
        if (it == byName.end()) {
            r.outcome = Outcome::Absent;
            r.evidence = L"not in the OLD PDB";
        } else if (several.count(w.name)) {
            r.outcome = Outcome::Ambiguous;
            r.evidence = L"several OLD addresses";
        } else if (const Verdict* vd = acc.Of(it->second)) {
            r.outcome = vd->outcome;
            r.rva = vd->outcome == Outcome::Ported ? vd->rva : 0;
            r.evidence = vd->evidence;
            // R9: names folded onto one OLD address (identical COMDATs) can come apart in
            // NEW, and a use of the address does not say which name it meant. Evidence
            // that a sibling of the set gave proves nothing for this name; only the
            // whole set staying at one address, or a slot no sibling can own, does.
            if (r.outcome == Outcome::Ported && names[it->second].size() > 1 &&
                !acc.FoldedProof(w.name, it->second, vd->rva)) {
                r.outcome = Outcome::Ambiguous;
                r.rva = 0;
                r.evidence = L"R9 several names share the OLD address (" + vd->evidence + L")";
            }
        } else {
            r.outcome = Outcome::NotFound;
            r.evidence = ob->pe.IsExec(it->second) && !ob->At(it->second) ? L"not a function start in OLD"
                                                                          : L"no evidence";
        }
        out.push_back(std::move(r));
    }
    auto ms = [](auto a, auto b) { return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count(); };
    stats.analyzeMs = ms(t0, t1);
    stats.engineMs = ms(t1, t2);
    stats.ms = ms(t0, std::chrono::steady_clock::now());
    return true;
}

}  // namespace port
