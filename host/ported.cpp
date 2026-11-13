// SPDX-License-Identifier: GPL-3.0-only

#include "ported.h"

// clang-format off
#include <wintrust.h>
#include <softpub.h>
#include <mscat.h>
#include <bcrypt.h>
// clang-format on

#include <algorithm>
#include <atomic>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "addon_protocol.h"
#include "json.h"

namespace host {

namespace {

std::vector<std::wstring> Lines(const std::string& utf8) {
    std::wstring text = Widen(utf8);
    if (!text.empty() && text[0] == 0xFEFF) text.erase(0, 1);
    std::vector<std::wstring> out;
    size_t start = 0;
    while (start <= text.size()) {
        size_t p = text.find(L'\n', start);
        std::wstring line = text.substr(start, p == std::wstring::npos ? std::wstring::npos : p - start);
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        out.push_back(std::move(line));
        if (p == std::wstring::npos) break;
        start = p + 1;
    }
    return out;
}

std::vector<std::wstring> Split(const std::wstring& s, wchar_t sep) {
    std::vector<std::wstring> out;
    for (size_t start = 0;;) {
        size_t p = s.find(sep, start);
        out.push_back(s.substr(start, p == std::wstring::npos ? std::wstring::npos : p - start));
        if (p == std::wstring::npos) break;
        start = p + 1;
    }
    return out;
}

std::vector<std::wstring> Words(const std::wstring& s) {
    std::vector<std::wstring> out;
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && (s[i] == L' ' || s[i] == L'\t')) i++;
        size_t j = i;
        while (j < s.size() && s[j] != L' ' && s[j] != L'\t') j++;
        if (j > i) out.push_back(s.substr(i, j - i));
        i = j;
    }
    return out;
}

std::wstring Upper(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towupper(c);
    return s;
}

bool IsHex(const std::wstring& s, size_t minLen, size_t maxLen) {
    if (s.size() < minLen || s.size() > maxLen) return false;
    return std::all_of(s.begin(), s.end(), [](wchar_t c) { return iswxdigit(c) != 0; });
}

// key=value lines; the value is everything after the first '='.
std::vector<std::pair<std::wstring, std::wstring>> KeyValues(const std::string& utf8) {
    std::vector<std::pair<std::wstring, std::wstring>> out;
    for (auto& line : Lines(utf8)) {
        size_t eq = line.find(L'=');
        if (line.empty() || eq == std::wstring::npos) continue;
        out.emplace_back(line.substr(0, eq), line.substr(eq + 1));
    }
    return out;
}

std::wstring NoTabs(std::wstring s) {
    for (auto& c : s)
        if (c == L'\t' || c == L'\r' || c == L'\n') c = L' ';
    return s;
}

void DeleteTree(const std::wstring& dir) {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(JoinPath(dir, L"*").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            std::wstring n = fd.cFileName;
            if (n == L"." || n == L"..") continue;
            std::wstring p = JoinPath(dir, n);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
                // Never follow a link out of our own folder.
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    RemoveDirectoryW(p.c_str());
                else
                    DeleteFileW(p.c_str());
            } else if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                DeleteTree(p);
            } else {
                SetFileAttributesW(p.c_str(), FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(p.c_str());
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    RemoveDirectoryW(dir.c_str());
}

bool Readable(const std::wstring& path) {
    Handle f(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                         OPEN_EXISTING, 0, nullptr));
    return (bool)f;
}

bool Running(DWORD pid) {
    Handle p(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, pid));
    return p && WaitForSingleObject(p.get(), 0) == WAIT_TIMEOUT;
}

}  // namespace

std::wstring ModuleKey(const std::wstring& module, const CodeView& cv) {
    return Lower(module) + L"_" + PdbId(cv);
}

// --- symbols-meta.txt ---

std::vector<bool> ParseSymbolsMeta(const std::string& utf8, const std::vector<std::wstring>& names) {
    std::unordered_map<std::wstring, bool> optional;
    for (auto& line : Lines(utf8)) {
        size_t tab = line.rfind(L'\t');
        if (tab == std::wstring::npos) continue;
        std::wstring flag = line.substr(tab + 1), name = line.substr(0, tab);
        if (flag == L"opt")
            optional.emplace(name, true);
        else if (flag == L"req")
            optional[name] = false;  // required anywhere wins
    }
    std::vector<bool> required;
    for (auto& n : names) {
        auto it = optional.find(n);
        required.push_back(it == optional.end() || !it->second);
    }
    return required;
}

// --- port-sources.txt ---

bool ParsePortSources(const std::string& utf8, PortSources& out) {
    PortSources s;
    for (auto& raw : Lines(utf8)) {
        std::wstring line = raw.substr(0, raw.find(L'#'));
        auto w = Words(line);
        if (w.empty()) continue;
        if (w[0] == L"deny") {
            if (w.size() != 3 || !IsHex(w[2], 9, 16)) return false;
            s.deny.emplace_back(Lower(w[1]), Upper(w[2]));
            continue;
        }
        SourceLine l;
        if ((w.size() != 3 && w.size() != 4) || !ParseFileVersion(w[1], l.version) || !IsHex(w[2], 9, 16) ||
            (w.size() == 4 && !IsHex(w[3], 64, 64)))
            return false;
        l.module = Lower(w[0]);
        l.key = Upper(w[2]);
        if (w.size() == 4) l.sha256 = Lower(w[3]);
        s.lines.push_back(std::move(l));
    }
    out = std::move(s);
    return true;
}

bool ReadPortSources(const std::wstring& exeDir, PortSources& out) {
    out = PortSources{};
    std::string text;
    if (!ReadFileBytes(JoinPath(exeDir, kPortSources), text)) return true;
    return ParsePortSources(text, out);
}

bool SourceDenied(const PortSources& src, const std::wstring& module, const std::wstring& key) {
    std::wstring mod = Lower(module), k = Upper(key);
    return std::any_of(src.deny.begin(), src.deny.end(), [&](auto& d) { return d.first == mod && d.second == k; });
}

std::vector<Candidate> LocalCandidates(const std::wstring& offsetsDir, const std::wstring& module,
                                       const CodeView& current) {
    std::vector<Candidate> out;
    std::wstring prefix = Lower(module) + L"_";
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(JoinPath(offsetsDir, prefix + L"*.txt").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return out;
    do {
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) continue;
        std::wstring name = fd.cFileName;
        std::wstring stem = name.substr(0, name.size() - 4);
        // Ported tables are no source: their build has no PDB to check them against.
        if (FileExists(JoinPath(offsetsDir, stem + kPortedExt))) continue;
        std::wstring path = JoinPath(offsetsDir, name);
        std::string text;
        OffsetsFile f;
        if (!ReadFileBytes(path, text) || !ParseOffsets(text, f) || f.module != Lower(module)) continue;
        if (f.timeDateStamp == current.timeDateStamp && f.sizeOfImage == current.sizeOfImage) continue;
        Candidate c;
        c.key = BinaryKey(f.timeDateStamp, f.sizeOfImage);
        c.local = true;
        c.localFile = path;
        c.localTime = ToTicks(fd.ftLastWriteTime);
        out.push_back(std::move(c));
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    std::sort(out.begin(), out.end(), [](auto& a, auto& b) { return a.localTime > b.localTime; });
    return out;
}

std::vector<Candidate> MergeCandidates(const std::vector<Candidate>& local, const PortSources& src,
                                       const std::wstring& module) {
    std::vector<Candidate> out = local;
    std::wstring mod = Lower(module);
    for (auto& l : src.lines) {
        if (l.module != mod) continue;
        auto it = std::find_if(out.begin(), out.end(), [&](auto& c) { return c.key == l.key; });
        if (it == out.end()) {
            Candidate c;
            c.key = l.key;
            c.version = l.version;
            c.sha256 = l.sha256;
            out.push_back(std::move(c));
        } else if (!it->version.valid) {
            it->version = l.version;
            it->sha256 = l.sha256;
        }
    }
    return out;
}

std::vector<Candidate> FilterCandidates(const std::vector<Candidate>& all, const PortSources& src,
                                        const std::wstring& module, const FileVersion& newVersion, size_t max) {
    std::wstring mod = Lower(module);
    std::vector<Candidate> out;
    for (auto& c : all) {
        if (!c.version.valid || !c.version.SameBranch(newVersion) || c.version.Value() >= newVersion.Value())
            continue;
        if (SourceDenied(src, mod, c.key)) continue;
        out.push_back(c);
    }
    std::stable_sort(out.begin(), out.end(), [](auto& a, auto& b) { return a.version.Value() > b.version.Value(); });
    if (out.size() > max) out.resize(max);
    return out;
}

std::wstring CandidateSet(const std::vector<Candidate>& c) {
    std::vector<std::wstring> keys;
    for (auto& x : c) keys.push_back(x.key);
    std::sort(keys.begin(), keys.end());
    std::wstring out;
    for (auto& k : keys) out += (out.empty() ? L"" : L" ") + k;
    return out;
}

// --- sidecar ---

const wchar_t* OutcomeName(port::Outcome o) {
    switch (o) {
        case port::Outcome::Ported: return L"ported";
        case port::Outcome::Absent: return L"absent";
        case port::Outcome::Ambiguous: return L"ambiguous";
        case port::Outcome::NotFound: return L"notfound";
    }
    return L"notfound";
}

std::string FormatSidecar(const Sidecar& s) {
    std::wstring t;
    t += Format(L"engine=%d\n", s.engine);
    t += L"host=" + s.hostVersion + L"\n";
    t += L"source=" + s.sourceVersion + L" " + s.sourceKey + L"\n";
    t += L"sourceKind=" + s.sourceKind + L"\n";
    t += L"sources=" + s.sources + L"\n";
    t += L"date=" + IsoTime(s.date) + L"\n";
    t += L"lastProbe=" + IsoTime(s.lastProbe) + L"\n";
    // name=<outcome>\t<rva hex or empty>\t<evidence>\t<symbol>: symbols carry '=' and
    // spaces but never a tab.
    for (auto& n : s.names)
        t += std::wstring(L"name=") + OutcomeName(n.outcome) + L"\t" +
             (n.outcome == port::Outcome::Ported ? Format(L"%x", n.rva) : L"") + L"\t" + NoTabs(n.evidence) + L"\t" +
             n.name + L"\n";
    return Narrow(t);
}

bool ParseSidecar(const std::string& utf8, Sidecar& out) {
    Sidecar s;
    bool haveEngine = false;
    for (auto& [k, v] : KeyValues(utf8)) {
        if (k == L"engine") {
            s.engine = _wtoi(v.c_str());
            haveEngine = true;
        } else if (k == L"host") {
            s.hostVersion = v;
        } else if (k == L"source") {
            auto w = Words(v);
            if (w.size() == 2) {
                s.sourceVersion = w[0];
                s.sourceKey = w[1];
            }
        } else if (k == L"sourceKind") {
            s.sourceKind = v;
        } else if (k == L"sources") {
            s.sources = v;
        } else if (k == L"date") {
            ParseIsoTime(v, s.date);
        } else if (k == L"lastProbe") {
            ParseIsoTime(v, s.lastProbe);
        } else if (k == L"name") {
            auto f = Split(v, L'\t');
            if (f.size() != 4) return false;
            PortedName n;
            n.name = f[3];
            n.evidence = f[2];
            if (f[0] == L"ported") {
                n.outcome = port::Outcome::Ported;
                wchar_t* end = nullptr;
                unsigned long long rva = wcstoull(f[1].c_str(), &end, 16);
                if (f[1].empty() || !end || *end || rva > 0xFFFFFFFFull) return false;
                n.rva = (uint32_t)rva;
            } else if (f[0] == L"absent") {
                n.outcome = port::Outcome::Absent;
            } else if (f[0] == L"ambiguous") {
                n.outcome = port::Outcome::Ambiguous;
            } else {
                n.outcome = port::Outcome::NotFound;
            }
            s.names.push_back(std::move(n));
        }
    }
    if (!haveEngine) return false;
    out = std::move(s);
    return true;
}

bool ProbeDue(const Sidecar& s, uint64_t now) {
    return s.lastProbe == 0 || now < s.lastProbe || now - s.lastProbe >= kPortProbeInterval;
}

std::wstring PortedStale(const std::wstring& offsetsDir, const std::wstring& key, const std::wstring& module,
                         const PortSources& src) {
    // Left behind when a quarantine could not delete them (real offsets never are:
    // writing them removes the record).
    if (IsQuarantined(offsetsDir, key)) return L"they crashed explorer before";
    std::wstring sidecarPath = JoinPath(offsetsDir, key + kPortedExt);
    if (!FileExists(sidecarPath)) return {};  // real offsets
    std::string text;
    Sidecar sc;
    if (!ReadFileBytes(sidecarPath, text) || !ParseSidecar(text, sc)) return L"its sidecar does not parse";
    if (sc.engine != port::kEngineVersion)
        return Format(L"engine %d ported them, this host has engine %d", sc.engine, port::kEngineVersion);
    if (SourceDenied(src, module, sc.sourceKey))
        return L"their source build " + sc.sourceKey + L" is denied in " + kPortSources;
    return {};
}

bool DropPorted(const std::wstring& offsetsDir, const std::wstring& key) {
    // The offsets first: alone, a sidecar only makes the next start look again.
    std::wstring off = JoinPath(offsetsDir, key + L".txt");
    if (!DeleteFileW(off.c_str()) && GetLastError() != ERROR_FILE_NOT_FOUND) return false;
    if (FileExists(off)) return false;
    DeleteFileW(JoinPath(offsetsDir, key + kPortedExt).c_str());
    return true;
}

// --- the module-write rule ---

bool BuildPortedOffsets(const std::wstring& module, const CodeView& cv, const std::vector<port::Want>& wanted,
                        const std::vector<port::OldSymbol>& oldSymbols, const std::vector<port::Result>& results,
                        OffsetsFile& out, std::vector<PortedName>& names, std::vector<std::wstring>& missing) {
    missing.clear();
    names.clear();
    if (results.size() != wanted.size()) {
        missing.push_back(L"(the engine gave " + std::to_wstring(results.size()) + L" results for " +
                          std::to_wstring(wanted.size()) + L" names)");
        return false;
    }
    std::unordered_map<std::wstring, uint32_t> old;
    for (auto& s : oldSymbols) old.emplace(s.name, s.rva);
    for (size_t i = 0; i < wanted.size(); i++) {
        if (results[i].name != wanted[i].name) {
            missing.push_back(L"(the engine's results are out of order)");
            return false;
        }
        PortedName n{wanted[i].name, results[i].outcome, results[i].rva, results[i].evidence};
        if (n.outcome == port::Outcome::Ported) {
            if (!old.count(n.name)) {
                n.outcome = port::Outcome::NotFound;
                n.evidence = L"host: not in the old PDB";
            } else if (n.rva < cv.sizeOfHeaders || n.rva >= cv.sizeOfImage) {
                n.outcome = port::Outcome::NotFound;
                n.evidence = L"host: outside the image";
            }
        }
        names.push_back(std::move(n));
    }
    // Two names may share a new address only when they shared the old one (folded code).
    std::map<uint32_t, std::vector<size_t>> byRva;
    for (size_t i = 0; i < names.size(); i++)
        if (names[i].outcome == port::Outcome::Ported) byRva[names[i].rva].push_back(i);
    for (auto& [rva, idx] : byRva) {
        if (idx.size() < 2) continue;
        uint32_t first = old[names[idx[0]].name];
        bool folded = std::all_of(idx.begin(), idx.end(), [&](size_t i) { return old[names[i].name] == first; });
        if (folded) continue;
        for (size_t i : idx) {
            names[i].outcome = port::Outcome::Ambiguous;
            names[i].evidence = L"host: shares its new address";
        }
    }
    for (size_t i = 0; i < names.size(); i++)
        if (wanted[i].required && old.count(names[i].name) && names[i].outcome != port::Outcome::Ported)
            missing.push_back(names[i].name);
    if (!missing.empty()) return false;
    out = OffsetsFile{};
    out.module = Lower(module);
    out.timeDateStamp = cv.timeDateStamp;
    out.sizeOfImage = cv.sizeOfImage;
    for (auto& n : names)
        out.entries.emplace_back(n.name, n.outcome == port::Outcome::Ported ? std::optional<DWORD>(n.rva)
                                                                            : std::nullopt);
    return true;
}

bool WritePortedFiles(const std::wstring& offsetsPath, const std::string& offsets, const std::wstring& sidecarPath,
                      const std::string& sidecar, std::vector<std::wstring>* order) {
    // The sidecar first: offsets without it would pass for real ones.
    if (!WriteFileAtomic(sidecarPath, sidecar)) return false;
    if (order) order->push_back(sidecarPath);
    if (!WriteFileAtomic(offsetsPath, offsets)) {
        DeleteFileW(sidecarPath.c_str());
        return false;
    }
    if (order) order->push_back(offsetsPath);
    return true;
}

std::wstring PortDiff(const OffsetsFile& ported, const OffsetsFile& real, size_t& differ) {
    std::unordered_map<std::wstring, std::optional<DWORD>> r(real.entries.begin(), real.entries.end());
    size_t same = 0;
    std::wstring names;
    differ = 0;
    for (auto& [name, rva] : ported.entries) {
        auto it = r.find(name);
        std::optional<DWORD> want = it == r.end() ? std::nullopt : it->second;
        if (rva == want) {
            same++;
            continue;
        }
        // An empty ported entry for a name the real PDB has is a miss, not a wrong
        // address; both are listed, only a wrong address would have hurt.
        differ++;
        names += (names.empty() ? L"" : L", ") + name +
                 (rva ? Format(L" (%lx, real %ls)", *rva, want ? Format(L"%lx", *want).c_str() : L"none")
                      : std::wstring(L" (not ported)"));
    }
    return Format(L"%zu same, %zu differ", same, differ) + (names.empty() ? L"" : L": " + names);
}

// --- .portfail ---

std::string FormatPortFail(const PortFail& f) {
    return Narrow(Format(L"engine=%d\n", f.engine) + L"host=" + f.hostVersion + L"\nsources=" + f.sources +
                  L"\nat=" + IsoTime(f.at) + L"\nwhy=" + NoTabs(f.why) + L"\n");
}

bool ParsePortFail(const std::string& utf8, PortFail& out) {
    PortFail f;
    bool haveEngine = false, haveAt = false;
    for (auto& [k, v] : KeyValues(utf8)) {
        if (k == L"engine") {
            f.engine = _wtoi(v.c_str());
            haveEngine = true;
        } else if (k == L"host") {
            f.hostVersion = v;
        } else if (k == L"sources") {
            f.sources = v;
        } else if (k == L"at") {
            haveAt = ParseIsoTime(v, f.at);
        } else if (k == L"why") {
            f.why = v;
        }
    }
    if (!haveEngine || !haveAt) return false;
    out = std::move(f);
    return true;
}

bool PortFailBlocks(const PortFail& f, int engine, const std::wstring& hostVersion, const std::wstring& sources,
                    uint64_t now) {
    return f.engine == engine && f.hostVersion == hostVersion && f.sources == sources && now >= f.at &&
           now - f.at < kPortFailRetry;
}

// --- the crash guard ---

std::string FormatGuard(const PortGuard& g) {
    json::Value v = json::Value::Obj();
    v.Set(L"v", json::Value::Int(1));
    json::Value keys = json::Value::Arr();
    for (auto& k : g.keys) keys.Push(json::Value::Str(k));
    v.Set(L"keys", keys);
    v.Set(L"armedAt", json::Value::Str(IsoTime(g.armedAt)));
    return json::Write(v);
}

bool ParseGuard(const std::string& utf8, PortGuard& out) {
    json::Value v;
    if (!json::Parse(utf8, v) || v.type != json::Value::Type::Object) return false;
    PortGuard g;
    for (auto& k : v.Get(L"keys").items)
        if (!k.AsString().empty()) g.keys.push_back(k.AsString());
    ParseIsoTime(v.Get(L"armedAt").AsString(), g.armedAt);
    out = std::move(g);
    return true;
}

bool ReadGuard(const std::wstring& dataDir, PortGuard& out) {
    std::string text;
    return ReadFileBytes(JoinPath(dataDir, kPortedGuardFile), text) && ParseGuard(text, out) && !out.keys.empty();
}

bool SetGuard(const std::wstring& dataDir, const std::vector<std::wstring>& keys, uint64_t now) {
    std::wstring path = JoinPath(dataDir, kPortedGuardFile);
    if (keys.empty()) return DeleteFileW(path.c_str()) || GetLastError() == ERROR_FILE_NOT_FOUND;
    PortGuard g;
    g.keys = keys;
    std::sort(g.keys.begin(), g.keys.end());
    PortGuard old;
    g.armedAt = ReadGuard(dataDir, old) && old.keys == g.keys && old.armedAt ? old.armedAt : now;
    return WriteFileAtomic(path, FormatGuard(g));
}

std::vector<std::wstring> QuarantineGuarded(const std::wstring& dataDir, const std::wstring& reason, uint64_t now) {
    std::vector<std::wstring> done;
    PortGuard g;
    if (!ReadGuard(dataDir, g)) return done;
    std::wstring offsetsDir = JoinPath(dataDir, addon::kOffsetsDir);
    std::vector<std::wstring> keep;  // offsets that could not be deleted stay guarded
    for (auto& key : g.keys) {
        if (key.find_first_of(L"\\/:") != std::wstring::npos) continue;
        std::wstring sidecarPath = JoinPath(offsetsDir, key + kPortedExt);
        // Only offsets that are still ported go; real ones are never touched.
        if (!FileExists(sidecarPath)) continue;
        std::string text;
        Sidecar sc;
        std::wstring source = ReadFileBytes(sidecarPath, text) && ParseSidecar(text, sc)
                                  ? sc.sourceVersion + L" " + sc.sourceKey
                                  : std::wstring();
        std::wstring q = L"reason=" + NoTabs(reason) + L"\nat=" + IsoTime(now) + L"\nsource=" + source +
                         Format(L"\nengine=%d\n", port::kEngineVersion);
        WriteFileAtomic(JoinPath(offsetsDir, key + kQuarantinedExt), Narrow(q));
        if (!DropPorted(offsetsDir, key)) keep.push_back(key);
        done.push_back(key);
    }
    SetGuard(dataDir, keep, now);
    return done;
}

bool IsQuarantined(const std::wstring& offsetsDir, const std::wstring& key) {
    return FileExists(JoinPath(offsetsDir, key + kQuarantinedExt));
}

std::vector<std::wstring> RetractPorted(const std::wstring& dataDir, const std::vector<std::wstring>& keys,
                                        const std::wstring& why, uint64_t now) {
    std::vector<std::wstring> gone;
    std::wstring offsetsDir = JoinPath(dataDir, addon::kOffsetsDir);
    for (auto& key : keys) {
        if (key.find_first_of(L"\\/:") != std::wstring::npos) continue;
        std::string text;
        Sidecar sc;
        // Real offsets (no sidecar) are never touched.
        if (!ReadFileBytes(JoinPath(offsetsDir, key + kPortedExt), text) || !ParseSidecar(text, sc)) continue;
        PortFail f{port::kEngineVersion, addon::kVersion, sc.sources, now, why};
        WriteFileAtomic(JoinPath(offsetsDir, key + kPortFailExt), FormatPortFail(f));
        if (DropPorted(offsetsDir, key)) gone.push_back(key);
    }
    PortGuard g;
    if (ReadGuard(dataDir, g)) {
        std::vector<std::wstring> left;
        for (auto& k : g.keys)
            if (std::find(gone.begin(), gone.end(), k) == gone.end()) left.push_back(k);
        if (left.size() != g.keys.size()) SetGuard(dataDir, left, now);
    }
    return gone;
}

std::wstring LostRequiredName(const std::wstring& dataDir, const std::wstring& exeDir, const std::wstring& key) {
    std::wstring offsetsDir = JoinPath(dataDir, addon::kOffsetsDir);
    if (key.find_first_of(L"\\/:") != std::wstring::npos || FileExists(JoinPath(offsetsDir, key + kPortedExt)))
        return {};
    std::string text;
    OffsetsFile cur;
    if (!ReadFileBytes(JoinPath(offsetsDir, key + L".txt"), text) || !ParseOffsets(text, cur)) return {};
    std::vector<std::wstring> empty;
    for (auto& [name, rva] : cur.entries)
        if (!rva) empty.push_back(name);
    if (empty.empty()) return {};
    std::string meta;
    std::vector<bool> required =
        ParseSymbolsMeta(ReadFileBytes(JoinPath(exeDir, kSymbolsMeta), meta) ? meta : std::string(), empty);
    std::unordered_set<std::wstring> wanted;
    for (size_t i = 0; i < empty.size(); i++)
        if (required[i]) wanted.insert(empty[i]);
    if (wanted.empty()) return {};
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(JoinPath(offsetsDir, cur.module + L"_*.txt").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return {};
    std::wstring lost;
    do {
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) continue;
        std::wstring name = fd.cFileName, stem = name.substr(0, name.size() - 4);
        // Ported offsets say nothing about what the module had.
        if (stem == key || FileExists(JoinPath(offsetsDir, stem + kPortedExt))) continue;
        OffsetsFile older;
        text.clear();
        if (!ReadFileBytes(JoinPath(offsetsDir, name), text) || !ParseOffsets(text, older) ||
            older.module != cur.module ||
            (older.timeDateStamp == cur.timeDateStamp && older.sizeOfImage == cur.sizeOfImage))
            continue;
        for (auto& [n, rva] : older.entries)
            if (rva && wanted.count(n)) {
                lost = n;
                break;
            }
    } while (lost.empty() && FindNextFileW(h, &fd));
    FindClose(h);
    return lost;
}

bool QuarantineBlocks(const std::wstring& offsetsDir, const std::wstring& key, int engine) {
    std::wstring path = JoinPath(offsetsDir, key + kQuarantinedExt);
    if (!FileExists(path)) return false;
    std::string text;
    if (!ReadFileBytes(path, text)) return true;
    for (auto& [k, v] : KeyValues(text))
        if (k == L"engine") return _wtoi(v.c_str()) == engine;
    return true;
}

// --- port-tmp ---

std::wstring PortTmpDir(const std::wstring& dataDir) {
    return JoinPath(JoinPath(dataDir, kPortTmpDir), std::to_wstring(GetCurrentProcessId()));
}

void CleanStalePortTmp(const std::wstring& dataDir) {
    std::wstring root = JoinPath(dataDir, kPortTmpDir);
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(JoinPath(root, L"*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        std::wstring n = fd.cFileName;
        if (n == L"." || n == L"..") continue;
        std::wstring p = JoinPath(root, n);
        bool pidDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                      !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && n.size() <= 10 &&
                      std::all_of(n.begin(), n.end(), [](wchar_t c) { return c >= L'0' && c <= L'9'; });
        if (pidDir && Running((DWORD)wcstoul(n.c_str(), nullptr, 10))) continue;
        // A dead process's folder, or what a host before per-process folders left.
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            DeleteTree(p);
        else
            DeleteFileW(p.c_str());
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

// --- verifying a downloaded old DLL ---

namespace {

// After a WTD_STATEACTION_VERIFY: the leaf signer's display name.
std::wstring SignerName(HANDLE state) {
    CRYPT_PROVIDER_DATA* pd = WTHelperProvDataFromStateData(state);
    CRYPT_PROVIDER_SGNR* sg = pd ? WTHelperGetProvSignerFromChain(pd, 0, FALSE, 0) : nullptr;
    if (!sg || !sg->pasCertChain || !sg->csCertChain || !sg->pasCertChain[0].pCert) return {};
    wchar_t name[256] = {};
    CertGetNameStringW(sg->pasCertChain[0].pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr, name, ARRAYSIZE(name));
    return name;
}

// WinVerifyTrust with no UI and no network; true when trusted and signed by Microsoft.
bool VerifyTrust(WINTRUST_DATA& wd, std::wstring& signer) {
    GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    wd.cbStruct = sizeof(wd);
    wd.dwUIChoice = WTD_UI_NONE;
    wd.fdwRevocationChecks = WTD_REVOKE_NONE;
    wd.dwStateAction = WTD_STATEACTION_VERIFY;
    wd.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL | WTD_REVOCATION_CHECK_NONE;
    LONG r = WinVerifyTrust((HWND)INVALID_HANDLE_VALUE, &action, &wd);
    if (r == ERROR_SUCCESS) signer = SignerName(wd.hWVTStateData);
    wd.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust((HWND)INVALID_HANDLE_VALUE, &action, &wd);
    return r == ERROR_SUCCESS && StartsWithNoCase(signer, L"Microsoft ");
}

}  // namespace

bool SignatureValid(const std::wstring& path, std::wstring& how) {
    std::wstring signer;
    WINTRUST_FILE_INFO fi{};
    fi.cbStruct = sizeof(fi);
    fi.pcwszFilePath = path.c_str();
    WINTRUST_DATA wd{};
    wd.dwUnionChoice = WTD_CHOICE_FILE;
    wd.pFile = &fi;
    if (VerifyTrust(wd, signer)) {
        how = L"embedded signature, " + signer;
        return true;
    }
    // Most Windows files are catalog-signed; a catalog this PC still has counts.
    Handle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr));
    HCATADMIN admin = nullptr;
    if (!file || !CryptCATAdminAcquireContext2(&admin, nullptr, BCRYPT_SHA256_ALGORITHM, nullptr, 0)) {
        how = L"no signature";
        return false;
    }
    bool ok = false;
    BYTE hash[64];
    DWORD hashLen = sizeof(hash);
    if (CryptCATAdminCalcHashFromFileHandle2(admin, file.get(), &hashLen, hash, 0)) {
        std::wstring tag;
        for (DWORD i = 0; i < hashLen; i++) tag += Format(L"%02X", hash[i]);
        HCATINFO cat = nullptr;
        while (!ok && (cat = CryptCATAdminEnumCatalogFromHash(admin, hash, hashLen, 0, &cat)) != nullptr) {
            CATALOG_INFO ci{};
            ci.cbStruct = sizeof(ci);
            if (!CryptCATCatalogInfoFromContext(cat, &ci, 0)) continue;
            WINTRUST_CATALOG_INFO wc{};
            wc.cbStruct = sizeof(wc);
            wc.pcwszCatalogFilePath = ci.wszCatalogFile;
            wc.pcwszMemberFilePath = path.c_str();
            wc.pcwszMemberTag = tag.c_str();
            wc.pbCalculatedFileHash = hash;
            wc.cbCalculatedFileHash = hashLen;
            wc.hCatAdmin = admin;
            WINTRUST_DATA cd{};
            cd.dwUnionChoice = WTD_CHOICE_CATALOG;
            cd.pCatalog = &wc;
            ok = VerifyTrust(cd, signer);
        }
        if (cat) CryptCATAdminReleaseCatalogContext(admin, cat, 0);
    }
    CryptCATAdminReleaseContext(admin, 0);
    how = ok ? L"catalog signature, " + signer : L"no signature this PC can check";
    return ok;
}

bool FileSha256(const std::wstring& path, std::wstring& hex) {
    std::string bytes;
    if (!ReadFileBytes(path, bytes)) return false;
    BYTE digest[32];
    if (BCryptHash(BCRYPT_SHA256_ALG_HANDLE, nullptr, 0, (PUCHAR)bytes.data(), (ULONG)bytes.size(), digest,
                   sizeof(digest)) != 0)
        return false;
    hex.clear();
    for (BYTE b : digest) hex += Format(L"%02x", b);
    return true;
}

// --- one module ---

namespace {

// How long a stopped port waits for its worker before leaving it to end on its own.
constexpr DWORD kWorkerStopWaitMs = 5000;
std::atomic<bool> g_workerLeftRunning{false};

// One piece of work for the worker thread. It holds everything the worker touches:
// the caller's own state is never shared, so a worker left running after a stop
// cannot reach freed memory.
struct WorkerJob {
    std::function<void(const std::atomic<bool>& cancel)> work;
    std::atomic<bool> cancel{false};
};

DWORD WINAPI WorkerMain(void* arg) {
    std::shared_ptr<WorkerJob> job = std::move(*(std::shared_ptr<WorkerJob>*)arg);
    delete (std::shared_ptr<WorkerJob>*)arg;
    job->work(job->cancel);
    return 0;
}

}  // namespace

bool RunOnWorker(std::function<void(const std::atomic<bool>& cancel)> work, const IdleFn& idle) {
    auto job = std::make_shared<WorkerJob>();
    job->work = std::move(work);
    auto* arg = new std::shared_ptr<WorkerJob>(job);
    Handle thread(CreateThread(nullptr, 0, WorkerMain, arg, 0, nullptr));
    if (!thread) {  // no thread to be had: do it here, as before
        delete arg;
        job->work(job->cancel);
        return true;
    }
    HANDLE h = thread.get();
    while (true) {
        DWORD r = idle ? MsgWaitForMultipleObjects(1, &h, FALSE, 100, QS_ALLINPUT) : WaitForSingleObject(h, INFINITE);
        if (r == WAIT_OBJECT_0) return true;
        if (r == WAIT_FAILED) Sleep(50);  // should not happen, but never spin
        if (idle && !idle()) break;
    }
    // Stop asked for. This is the one place the engine's cancel flag is set: the work
    // hands &cancel to port::ResolveAll and port::Input, which end within about a second.
    job->cancel = true;
    ULONGLONG start = GetTickCount64();
    while (GetTickCount64() - start < kWorkerStopWaitMs) {
        if (MsgWaitForMultipleObjects(1, &h, FALSE, 100, QS_ALLINPUT) == WAIT_OBJECT_0) return false;
        idle();  // keeps the window answering; the stop stands whatever it says
    }
    Log(L"the porting worker did not end within %lu ms after the stop; leaving it", kWorkerStopWaitMs);
    g_workerLeftRunning = true;
    return false;
}

namespace {

class Porter {
public:
    explicit Porter(const PortRequest& r)
        : req_(r), offsetsDir_(JoinPath(r.dataDir, addon::kOffsetsDir)), tmp_(PortTmpDir(r.dataDir)),
          key_(ModuleKey(r.module.name, r.cv)) {}
    ~Porter() {
        DeleteTree(tmp_);
        RemoveDirectoryW(DirName(tmp_).c_str());  // port-tmp itself, once empty
    }

    PortStatus Run(PortReport& rep);

private:
    // Old DLL by key into port-tmp, checked against the key and the candidate's hash
    // or a Microsoft signature.
    PortStatus GetOldDll(Candidate& c, std::wstring& path, std::wstring& why);
    PortStatus GetOldPdb(const CodeView& oldCv, std::wstring& path, std::wstring& why);
    PortStatus Fetch(const std::wstring& file, const std::wstring& key, const std::wstring& dest, uint64_t maxBytes,
                     std::wstring& why);
    PortStatus TryOne(Candidate& c, PortReport& rep, std::wstring& why);
    void RememberFailure(const std::wstring& why);

    const PortRequest& req_;
    std::wstring offsetsDir_, tmp_, key_;
    std::wstring identity_;  // engine run identity for .portfail
    std::vector<bool> required_;
    std::map<std::wstring, std::wstring> fetched_;  // old DLL key -> verified path
};

// why is set only when downloads are off.
PortStatus Porter::Fetch(const std::wstring& file, const std::wstring& key, const std::wstring& dest,
                         uint64_t maxBytes, std::wstring& why) {
    if (req_.noDownload) {
        why = file + L"/" + key + L": not cached and downloads are off";
        return PortStatus::Failed;
    }
    CreateDirs(DirName(dest));
    uint64_t bytes = 0;
    uint64_t before = req_.downloaded ? *req_.downloaded : 0;
    Download d = DownloadSymbolFile(file, key, dest, maxBytes, before, req_.progress, bytes);
    if (req_.downloaded) *req_.downloaded += bytes;
    switch (d) {
        case Download::Ok: return PortStatus::Written;
        case Download::NotFound: return PortStatus::Unported;
        case Download::Stopped: return PortStatus::Stopped;
        case Download::Failed: return PortStatus::Failed;
    }
    return PortStatus::Failed;
}

PortStatus Porter::GetOldDll(Candidate& c, std::wstring& path, std::wstring& why) {
    if (auto it = fetched_.find(c.key); it != fetched_.end()) {
        path = it->second;
        return PortStatus::Written;
    }
    std::wstring file = Lower(req_.module.name);
    path = JoinPath(JoinPath(tmp_, file + L"_" + c.key), file);
    DWORD stamp = 0, size = 0;
    if (!ParseBinaryKey(c.key, stamp, size)) {
        why = c.key + L": not a binary key";
        return PortStatus::Unported;
    }
    PortStatus st = Fetch(file, c.key, path, DllDownloadCap(size), why);
    if (st != PortStatus::Written) {
        if (why.empty())
            why = c.key + (st == PortStatus::Unported ? L": the old DLL is not on the symbol server" : L": download failed");
        return st;
    }
    CodeView cv;
    if (!ReadCodeView(path, cv) || BinaryKey(cv.timeDateStamp, cv.sizeOfImage) != c.key) {
        // A file just downloaded can be unreadable for local reasons (a scanner
        // holding it): no verdict on the build.
        if (!Readable(path)) {
            why = c.key + L": could not read the downloaded DLL";
            return PortStatus::Failed;
        }
        why = c.key + L": the downloaded DLL is not that build";
        return PortStatus::Unported;
    }
    // Hashing or a catalog lookup can take a while: on the worker.
    struct Check {
        std::wstring path, sha256;  // in
        bool read = false, ok = false;
        std::wstring how;
    };
    auto chk = std::make_shared<Check>();
    chk->path = path;
    chk->sha256 = c.sha256;
    bool done = RunOnWorker(
        [chk](const std::atomic<bool>&) {
            if (chk->sha256.empty()) {
                chk->read = true;
                chk->ok = SignatureValid(chk->path, chk->how);
                return;
            }
            std::wstring sha;
            chk->read = FileSha256(chk->path, sha);
            chk->ok = chk->read && sha == chk->sha256;
            chk->how = L"SHA-256 from port-sources.txt";
        },
        req_.idle);
    if (!done) {
        why = c.key + L": stopped";
        return PortStatus::Stopped;
    }
    if (!chk->read) {
        why = c.key + L": could not read the downloaded DLL";
        return PortStatus::Failed;
    }
    if (!chk->ok) {
        why = c.key + (c.sha256.empty() ? L": the downloaded DLL has " + chk->how
                                        : std::wstring(L": the downloaded DLL does not have the SHA-256 ") +
                                              kPortSources + L" gives");
        return PortStatus::Unported;
    }
    std::wstring how = chk->how;
    FileVersion v;
    if (!ReadFileVersion(path, v) || (c.version.valid && v.Value() != c.version.Value())) {
        why = c.key + L": the downloaded DLL's version is not the listed one";
        return PortStatus::Unported;
    }
    c.version = v;
    Log(L"%ls: old build %ls (%ls) checked: %ls", req_.module.name.c_str(), v.Text().c_str(), c.key.c_str(),
        how.c_str());
    fetched_[c.key] = path;
    return PortStatus::Written;
}

PortStatus Porter::GetOldPdb(const CodeView& oldCv, std::wstring& path, std::wstring& why) {
    std::wstring id = PdbId(oldCv);
    std::wstring own = JoinPath(JoinPath(JoinPath(JoinPath(req_.dataDir, addon::kSymbolsDir), oldCv.pdbName), id),
                                oldCv.pdbName);
    if (FileExists(own)) {
        path = own;
        return PortStatus::Written;
    }
    std::wstring wh = JoinPath(JoinPath(JoinPath(WindhawkSymbolsDir(), oldCv.pdbName), id), oldCv.pdbName);
    if (!req_.noWindhawkCache && FileExists(wh)) {
        Log(L"%ls: using Windhawk's cached %ls for the old build", req_.module.name.c_str(), oldCv.pdbName.c_str());
        path = wh;
        return PortStatus::Written;
    }
    path = JoinPath(JoinPath(tmp_, oldCv.pdbName + L"_" + id), oldCv.pdbName);
    PortStatus st = Fetch(oldCv.pdbName, id, path, kMaxPdbBytes, why);
    if (st != PortStatus::Written && why.empty())
        why = id + (st == PortStatus::Unported ? L": the old PDB is not on the symbol server" : L": download failed");
    return st;
}

void Porter::RememberFailure(const std::wstring& why) {
    PortFail f{port::kEngineVersion, addon::kVersion, identity_, NowTicks(), why};
    if (!WriteFileAtomic(JoinPath(offsetsDir_, key_ + kPortFailExt), FormatPortFail(f)))
        Log(L"%ls: could not write its %ls", req_.module.name.c_str(), kPortFailExt);
}

PortStatus Porter::TryOne(Candidate& c, PortReport& rep, std::wstring& why) {
    std::wstring oldDll, oldPdb;
    PortStatus st = GetOldDll(c, oldDll, why);
    if (st != PortStatus::Written) return st;
    CodeView oldCv;
    if (!ReadCodeView(oldDll, oldCv)) {  // read and checked moments ago
        why = c.key + L": could not read the downloaded DLL again";
        return PortStatus::Failed;
    }
    st = GetOldPdb(oldCv, oldPdb, why);
    if (st != PortStatus::Written) return st;

    // Reading the old PDB and the engine take seconds: on the worker, which gets its
    // own copy of everything it reads. The main thread does not touch e until the
    // worker is done.
    struct Engine {
        std::wstring oldPdb;
        port::Input in;
        std::vector<port::Result> results;
        port::Stats stats;
        std::wstring error;
        bool ok = false;
    };
    auto e = std::make_shared<Engine>();
    e->oldPdb = oldPdb;
    e->in.oldDll = oldDll;
    e->in.newDll = req_.module.path;
    // A NEW file replaced since the host read its CodeView fails the try rather than
    // label one build's addresses with another.
    e->in.expectNew = ExpectedNew(req_.cv);
    if (!RunOnWorker([e](const std::atomic<bool>& cancel) {
            e->ok = port::ResolveAll(e->oldPdb, e->in.oldDll, e->in.oldSymbols, e->error, &cancel);
        }, req_.idle)) {
        why = c.key + L": stopped";
        return PortStatus::Stopped;
    }
    port::Input& in = e->in;
    if (!e->ok) {
        // dbghelp or the files on this PC: no verdict on the build.
        why = c.key + L": " + e->error;
        return PortStatus::Failed;
    }
    // A local offsets file is only a hint which build the PC had: the old PDB must
    // give every address it holds.
    if (c.local) {
        std::string text;
        OffsetsFile f;
        std::unordered_map<std::wstring, uint32_t> old;
        for (auto& s : in.oldSymbols) old.emplace(s.name, s.rva);
        if (!ReadFileBytes(c.localFile, text)) {
            why = c.key + L": could not read the local offsets file";
            return PortStatus::Failed;
        }
        bool same = ParseOffsets(text, f);
        for (auto& [name, rva] : f.entries) {
            auto it = old.find(name);
            if (rva ? (it == old.end() || it->second != *rva) : it != old.end()) same = false;
        }
        if (!same) {
            why = c.key + L": the old PDB does not give the addresses of the local offsets file";
            return PortStatus::Unported;
        }
    }
    for (size_t i = 0; i < req_.names.size(); i++) in.wanted.push_back({req_.names[i], required_[i]});

    if (!RunOnWorker([e](const std::atomic<bool>& cancel) {
            e->in.cancel = &cancel;  // the job owns the flag for as long as the work runs
            e->ok = port::Run(e->in, e->results, e->stats, e->error);
            e->in.cancel = nullptr;
        }, req_.idle)) {
        why = c.key + L": stopped";
        return PortStatus::Stopped;
    }
    if (!e->ok) {
        why = c.key + L": the engine failed: " + e->error;
        return PortStatus::Failed;
    }
    const std::vector<port::Result>& results = e->results;
    const port::Stats& stats = e->stats;
    OffsetsFile out;
    std::vector<PortedName> names;
    std::vector<std::wstring> missing;
    size_t ported = 0;
    bool ok = BuildPortedOffsets(req_.module.name, req_.cv, in.wanted, in.oldSymbols, results, out, names, missing);
    for (auto& n : names) ported += n.outcome == port::Outcome::Ported;
    Log(L"%ls: from %ls, %zu of %zu names ported (%u and %u functions, %llu ms)", req_.module.name.c_str(),
        c.version.Text().c_str(), ported, names.size(), stats.functionsOld, stats.functionsNew,
        (unsigned long long)stats.ms);
    if (!ok) {
        std::wstring list;
        for (size_t i = 0; i < missing.size() && i < 8; i++) list += (i ? L"; " : L"") + missing[i];
        if (missing.size() > 8) list += Format(L"; and %zu more", missing.size() - 8);
        why = c.version.Text() + L": required names not ported: " + list;
        return PortStatus::Unported;
    }
    Sidecar sc;
    sc.engine = port::kEngineVersion;
    sc.hostVersion = addon::kVersion;
    sc.sourceVersion = c.version.Text();
    sc.sourceKey = c.key;
    sc.sourceKind = c.local ? L"local" : L"shipped";
    sc.sources = identity_;
    sc.date = NowTicks();
    sc.lastProbe = req_.simulated ? 0 : sc.date;
    sc.names = names;
    std::wstring offPath = JoinPath(offsetsDir_, OffsetsFileName(req_.module.name, req_.cv));
    if (!WritePortedFiles(offPath, FormatOffsets(out), JoinPath(offsetsDir_, key_ + kPortedExt), FormatSidecar(sc))) {
        // WritePortedFiles took its sidecar back; a full or locked disk is no verdict.
        why = L"could not write the ported offsets";
        return PortStatus::Failed;
    }
    DeleteFileW(JoinPath(offsetsDir_, key_ + kPortFailExt).c_str());
    rep.sourceVersion = sc.sourceVersion;
    rep.detail = Format(L"ported %zu of %zu names from %ls (%ls); provisional until the PDB is published", ported,
                        names.size(), sc.sourceVersion.c_str(), sc.sourceKind.c_str());
    return PortStatus::Written;
}

PortStatus Porter::Run(PortReport& rep) {
    const std::wstring& mod = req_.module.name;
    if (QuarantineBlocks(offsetsDir_, key_, port::kEngineVersion)) {
        rep.detail = L"ported offsets of this build crashed explorer before; waiting for the real PDB";
        return PortStatus::Quarantined;
    }
    if (IsQuarantined(offsetsDir_, key_)) {
        // Another engine's port crashed; this one starts on probation under the guard.
        Log(L"%ls: the quarantine is another engine's; porting with engine %d", mod.c_str(), port::kEngineVersion);
        DeleteFileW(JoinPath(offsetsDir_, key_ + kQuarantinedExt).c_str());
    }
    FileVersion newVersion;
    if (!ReadFileVersion(req_.module.path, newVersion)) {
        rep.detail = L"no file version to pick a source build by; not porting";
        return PortStatus::Unported;
    }
    std::string text;
    required_ = ParseSymbolsMeta(ReadFileBytes(JoinPath(req_.exeDir, kSymbolsMeta), text) ? text : std::string(),
                                 req_.names);
    PortSources src;
    if (!ReadPortSources(req_.exeDir, src)) Log(L"%ls is broken; using local sources only", kPortSources);
    std::vector<Candidate> all = MergeCandidates(LocalCandidates(offsetsDir_, mod, req_.cv), src, mod);
    // What this attempt would start from, known before any download: a failed one is
    // not repeated for a day unless this changes.
    identity_ = CandidateSet(all);
    for (auto& [m, k] : src.deny)
        if (m == Lower(mod)) identity_ += L" -" + k;
    std::string failText;
    PortFail fail;
    uint64_t now = NowTicks();
    if (ReadFileBytes(JoinPath(offsetsDir_, key_ + kPortFailExt), failText) && ParsePortFail(failText, fail) &&
        PortFailBlocks(fail, port::kEngineVersion, addon::kVersion, identity_, now)) {
        rep.detail = L"porting this build failed at " + IsoTime(fail.at) + L" (" + fail.why + L"); next try after " +
                     IsoTime(fail.at + kPortFailRetry);
        return PortStatus::Skipped;
    }
    CleanStalePortTmp(req_.dataDir);
    DeleteTree(tmp_);
    if (!CreateDirs(tmp_)) {
        rep.detail = L"could not create " + std::wstring(kPortTmpDir);
        return PortStatus::Failed;
    }
    Log(L"%ls: no PDB published for %ls (%ls); porting, %zu candidate builds", mod.c_str(), PdbId(req_.cv).c_str(),
        newVersion.Text().c_str(), all.size());

    // A local file of a build the list does not know: its version is in its DLL.
    size_t probes = 0;
    std::wstring why, probeFailed;
    for (auto& c : all) {
        if (c.version.valid || probes >= kMaxVersionProbes || SourceDenied(src, mod, c.key)) continue;
        probes++;
        std::wstring path, w;
        PortStatus st = GetOldDll(c, path, w);
        if (st == PortStatus::Stopped) {
            rep.detail = w;
            return st;
        }
        // One build out of reach is no reason not to try the others; it stays of
        // unknown version, so the filter drops it.
        if (st == PortStatus::Failed) {
            Log(L"%ls: %ls; skipping that build", mod.c_str(), w.c_str());
            probeFailed = w;
        } else if (st != PortStatus::Written) {
            Log(L"%ls: %ls", mod.c_str(), w.c_str());
        }
    }
    std::vector<Candidate> picked = FilterCandidates(all, src, mod, newVersion, kMaxPortSources);
    if (picked.empty()) {
        why = L"no older published build of " + newVersion.Text() + L"'s branch to port from";
    }
    for (auto& c : picked) {
        std::wstring w;
        PortStatus st = TryOne(c, rep, w);
        if (st == PortStatus::Written || st == PortStatus::Stopped || st == PortStatus::Failed) {
            if (st != PortStatus::Written) rep.detail = w;
            return st;
        }
        Log(L"%ls: %ls", mod.c_str(), w.c_str());
        why = w;
    }
    // A skipped build was never judged: nothing is remembered, the next start tries again.
    if (!probeFailed.empty()) {
        rep.detail = L"not ported; " + probeFailed;
        return PortStatus::Failed;
    }
    RememberFailure(why);
    rep.detail = L"not ported: " + why;
    return PortStatus::Unported;
}

}  // namespace

port::Identity ExpectedNew(const CodeView& cv) {
    port::Identity id;
    id.timeDateStamp = cv.timeDateStamp;
    id.sizeOfImage = cv.sizeOfImage;
    id.hasPdbId = !cv.pdbName.empty();  // ReadCodeView found an RSDS record
    static_assert(sizeof(id.pdbGuid) == sizeof(GUID), "RSDS GUID layout");
    memcpy(id.pdbGuid, &cv.guid, sizeof(GUID));
    id.pdbAge = cv.age;
    return id;
}

PortStatus PortModule(const PortRequest& req, PortReport& report) {
    Porter p(req);
    return p.Run(report);
}

bool PortWorkerLeftRunning() {
    return g_workerLeftRunning;
}

}  // namespace host
