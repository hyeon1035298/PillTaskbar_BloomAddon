// SPDX-License-Identifier: GPL-3.0-only

#include "pe.h"

#include <windows.h>

#include <algorithm>
#include <cstring>

namespace port {

namespace {

constexpr uint64_t kMaxFile = 512ull << 20;
constexpr uint32_t kMaxImage = 512u << 20;

bool ReadWholeFile(const std::wstring& path, std::vector<uint8_t>& out) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size;
    bool ok = GetFileSizeEx(h, &size) && size.QuadPart > 0 && (uint64_t)size.QuadPart <= kMaxFile;
    if (ok) {
        out.resize((size_t)size.QuadPart);
        size_t done = 0;
        while (ok && done < out.size()) {
            DWORD got = 0, want = (DWORD)std::min<size_t>(out.size() - done, 1u << 24);
            ok = ReadFile(h, out.data() + done, want, &got, nullptr) && got > 0;
            done += got;
        }
    }
    CloseHandle(h);
    return ok;
}

template <typename T>
bool Get(const std::vector<uint8_t>& d, uint64_t off, T& v) {
    if (off > d.size() || sizeof(T) > d.size() - off) return false;
    memcpy(&v, d.data() + off, sizeof(T));
    return true;
}

}  // namespace

uint16_t PeImage::U16(uint32_t rva) const {
    uint16_t v = 0;
    if (auto* p = Bytes(rva, 2)) memcpy(&v, p, 2);
    return v;
}

uint32_t PeImage::U32(uint32_t rva) const {
    uint32_t v = 0;
    if (auto* p = Bytes(rva, 4)) memcpy(&v, p, 4);
    return v;
}

uint64_t PeImage::U64(uint32_t rva) const {
    uint64_t v = 0;
    if (auto* p = Bytes(rva, 8)) memcpy(&v, p, 8);
    return v;
}

std::string PeImage::CString(uint32_t rva, size_t limit) const {
    std::string s;
    for (size_t i = 0; i < limit && rva + i < img_.size(); i++) {
        char c = (char)img_[rva + i];
        if (!c) break;
        s += c;
    }
    return s;
}

const PeSection* PeImage::SectionOf(uint32_t rva) const {
    for (auto& s : sections)
        if (rva >= s.va && rva - s.va < s.span) return &s;
    return nullptr;
}

static bool InRanges(const std::vector<std::pair<uint32_t, uint32_t>>& r, uint32_t rva) {
    auto it = std::upper_bound(r.begin(), r.end(), std::make_pair(rva, UINT32_MAX));
    if (it == r.begin()) return false;
    --it;
    return rva >= it->first && rva < it->second;
}

bool PeImage::IsExec(uint32_t rva) const { return InRanges(execRanges_, rva); }
bool PeImage::IsReadOnlyData(uint32_t rva) const { return InRanges(roRanges_, rva); }

bool PeImage::PointerAt(uint32_t slot, uint32_t& rva) const {
    if (!IsReloc(slot)) return false;
    uint64_t v = U64(slot);
    if (v < imageBase || v - imageBase >= sizeOfImage) return false;
    rva = (uint32_t)(v - imageBase);
    return true;
}

bool PeImage::Load(const std::wstring& path, std::wstring& error) {
    std::vector<uint8_t> d;
    if (!ReadWholeFile(path, d)) {
        error = L"cannot read " + path;
        return false;
    }
    uint16_t mz = 0;
    uint32_t lfanew = 0, sig = 0;
    if (!Get(d, 0, mz) || mz != 0x5A4D || !Get(d, 0x3C, lfanew) || !Get(d, lfanew, sig) || sig != 0x4550) {
        error = L"not a PE file";
        return false;
    }
    IMAGE_FILE_HEADER fh;
    if (!Get(d, (uint64_t)lfanew + 4, fh)) {
        error = L"truncated file header";
        return false;
    }
    uint64_t optOff = (uint64_t)lfanew + 4 + sizeof(IMAGE_FILE_HEADER);
    IMAGE_OPTIONAL_HEADER64 oh;
    if (fh.SizeOfOptionalHeader < offsetof(IMAGE_OPTIONAL_HEADER64, DataDirectory) || !Get(d, optOff, oh) ||
        oh.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        error = L"not a 64-bit PE image";
        return false;
    }
    machine = fh.Machine;
    timeDateStamp = fh.TimeDateStamp;
    sizeOfImage = oh.SizeOfImage;
    sizeOfHeaders = oh.SizeOfHeaders;
    imageBase = oh.ImageBase;
    if (sizeOfImage == 0 || sizeOfImage > kMaxImage || fh.NumberOfSections == 0 || fh.NumberOfSections > 96) {
        error = L"implausible image size or section count";
        return false;
    }
    uint32_t ndirs = std::min<uint32_t>(oh.NumberOfRvaAndSizes, IMAGE_NUMBEROF_DIRECTORY_ENTRIES);
    uint64_t dirsEnd = offsetof(IMAGE_OPTIONAL_HEADER64, DataDirectory) + ndirs * sizeof(IMAGE_DATA_DIRECTORY);
    if (dirsEnd > fh.SizeOfOptionalHeader) ndirs = (uint32_t)((fh.SizeOfOptionalHeader - offsetof(IMAGE_OPTIONAL_HEADER64, DataDirectory)) / sizeof(IMAGE_DATA_DIRECTORY));
    auto dir = [&](int i) -> IMAGE_DATA_DIRECTORY {
        return (uint32_t)i < ndirs ? oh.DataDirectory[i] : IMAGE_DATA_DIRECTORY{0, 0};
    };

    img_.assign(sizeOfImage, 0);
    memcpy(img_.data(), d.data(), std::min<size_t>({d.size(), (size_t)sizeOfImage, (size_t)std::max<uint32_t>(sizeOfHeaders, 0x400)}));
    uint64_t secOff = optOff + fh.SizeOfOptionalHeader;
    for (int i = 0; i < fh.NumberOfSections; i++) {
        IMAGE_SECTION_HEADER sh;
        if (!Get(d, secOff + (uint64_t)i * sizeof(sh), sh)) {
            error = L"truncated section table";
            return false;
        }
        PeSection s;
        char name[9] = {};
        memcpy(name, sh.Name, 8);
        s.name = name;
        s.va = sh.VirtualAddress;
        uint32_t vs = sh.Misc.VirtualSize, rs = sh.SizeOfRawData;
        s.span = rs ? std::max(vs, rs) : vs;
        if (s.va >= sizeOfImage) continue;
        s.span = std::min<uint32_t>(s.span, sizeOfImage - s.va);
        s.characteristics = sh.Characteristics;
        s.exec = (sh.Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
        s.write = (sh.Characteristics & IMAGE_SCN_MEM_WRITE) != 0;
        uint64_t n = std::min<uint64_t>(rs, s.span);
        if (sh.PointerToRawData < d.size()) n = std::min<uint64_t>(n, d.size() - sh.PointerToRawData);
        else n = 0;
        if (n) memcpy(img_.data() + s.va, d.data() + sh.PointerToRawData, (size_t)n);
        if (s.exec) execRanges_.emplace_back(s.va, s.va + s.span);
        else if (!s.write) roRanges_.emplace_back(s.va, s.va + s.span);
        sections.push_back(std::move(s));
    }
    std::sort(execRanges_.begin(), execRanges_.end());
    std::sort(roRanges_.begin(), roRanges_.end());

    ReadRelocs(dir(IMAGE_DIRECTORY_ENTRY_BASERELOC).VirtualAddress, dir(IMAGE_DIRECTORY_ENTRY_BASERELOC).Size);
    ReadPdata(dir(IMAGE_DIRECTORY_ENTRY_EXCEPTION).VirtualAddress, dir(IMAGE_DIRECTORY_ENTRY_EXCEPTION).Size);
    ReadImports(dir(IMAGE_DIRECTORY_ENTRY_IMPORT).VirtualAddress, dir(IMAGE_DIRECTORY_ENTRY_IMPORT).Size, false);
    ReadImports(dir(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT).VirtualAddress, dir(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT).Size, true);
    ReadExports(dir(IMAGE_DIRECTORY_ENTRY_EXPORT).VirtualAddress, dir(IMAGE_DIRECTORY_ENTRY_EXPORT).Size);
    ReadVersion(dir(IMAGE_DIRECTORY_ENTRY_RESOURCE).VirtualAddress, dir(IMAGE_DIRECTORY_ENTRY_RESOURCE).Size);

    // RSDS, read from the file like host/symbols.cpp ReadCodeView.
    IMAGE_DATA_DIRECTORY dbg = dir(IMAGE_DIRECTORY_ENTRY_DEBUG);
    for (uint32_t i = 0; dbg.VirtualAddress && i < dbg.Size / sizeof(IMAGE_DEBUG_DIRECTORY) && i < 64; i++) {
        IMAGE_DEBUG_DIRECTORY e;
        auto* p = Bytes(dbg.VirtualAddress + i * (uint32_t)sizeof(e), sizeof(e));
        if (!p) break;
        memcpy(&e, p, sizeof(e));
        if (e.Type != IMAGE_DEBUG_TYPE_CODEVIEW || e.SizeOfData < 25) continue;
        uint32_t magic = 0;
        if (!Get(d, e.PointerToRawData, magic) || magic != 0x53445352) continue;  // "RSDS"
        if ((uint64_t)e.PointerToRawData + e.SizeOfData > d.size()) continue;
        memcpy(guid, d.data() + e.PointerToRawData + 4, 16);
        memcpy(&age, d.data() + e.PointerToRawData + 20, 4);
        const char* nm = (const char*)d.data() + e.PointerToRawData + 24;
        pdbName.assign(nm, strnlen(nm, e.SizeOfData - 24));
        hasCodeView = true;
        break;
    }
    return true;
}

void PeImage::ReadRelocs(uint32_t rva, uint32_t size) {
    relocMap_.assign((sizeOfImage + 63) / 64, 0);
    if (!rva || !Bytes(rva, size)) return;
    uint32_t p = rva, end = rva + size;
    while (p + 8 <= end) {
        uint32_t page = U32(p), bsize = U32(p + 4);
        if (bsize < 8 || bsize > end - p) break;
        for (uint32_t i = 0; i < (bsize - 8) / 2; i++) {
            uint16_t e = U16(p + 8 + 2 * i);
            if (e >> 12 != IMAGE_REL_BASED_DIR64) continue;
            uint64_t slot = (uint64_t)page + (e & 0xFFF);
            if (slot + 8 > sizeOfImage) continue;
            relocs.push_back((uint32_t)slot);
            relocMap_[slot >> 6] |= 1ull << (slot & 63);
        }
        p += bsize;
    }
    std::sort(relocs.begin(), relocs.end());
    relocs.erase(std::unique(relocs.begin(), relocs.end()), relocs.end());
}

void PeImage::ReadPdata(uint32_t rva, uint32_t size) {
    if (!rva || !Bytes(rva, size)) return;
    for (uint32_t i = 0; i < size / 12; i++) {
        PeRuntimeFunction f;
        f.begin = U32(rva + 12 * i);
        f.end = U32(rva + 12 * i + 4);
        f.unwind = U32(rva + 12 * i + 8);
        if (!f.begin || f.end <= f.begin || f.end > sizeOfImage) continue;
        if (f.unwind & 1) {
            f.chained = true;  // the unwind field names another RUNTIME_FUNCTION
        } else {
            const uint8_t* u = Bytes(f.unwind, 1);
            f.chained = !u || ((u[0] >> 3) & UNW_FLAG_CHAININFO);
        }
        pdata.push_back(f);
    }
}

void PeImage::ReadImports(uint32_t rva, uint32_t size, bool delay) {
    if (!rva || !size) return;
    const uint32_t step = delay ? 32 : 20;
    for (uint32_t p = rva, n = 0; n < 4096 && Bytes(p, step); p += step, n++) {
        uint32_t nameRva, iatRva, intRva;
        if (delay) {
            nameRva = U32(p + 4);
            iatRva = U32(p + 12);
            intRva = U32(p + 16);
        } else {
            intRva = U32(p);
            nameRva = U32(p + 12);
            iatRva = U32(p + 16);
        }
        if (!nameRva) break;
        std::string dll = CString(nameRva, 256);
        for (auto& c : dll) c = (char)tolower((unsigned char)c);
        uint32_t thunks = intRva ? intRva : iatRva;
        for (uint32_t i = 0; i < 65536 && Bytes(thunks + 8 * i, 8) && Bytes(iatRva + 8 * i, 8); i++) {
            uint64_t t = U64(thunks + 8 * i);
            if (!t) break;
            std::string nm;
            if (t >> 63) {
                nm = "#" + std::to_string(t & 0xFFFF);
            } else {
                nm = CString((uint32_t)(t & 0x7FFFFFFF) + 2, 512);
            }
            iat[iatRva + 8 * i] = dll + "!" + nm;
        }
    }
}

void PeImage::ReadExports(uint32_t rva, uint32_t size) {
    if (!rva || !Bytes(rva, 40)) return;
    uint32_t nFuncs = U32(rva + 20), nNames = U32(rva + 24), funcs = U32(rva + 28), names = U32(rva + 32),
             ords = U32(rva + 36);
    if (nFuncs > 1u << 20 || nNames > 1u << 20) return;
    for (uint32_t i = 0; i < nNames; i++) {
        if (!Bytes(names + 4 * i, 4) || !Bytes(ords + 2 * i, 2)) break;
        uint16_t ord = U16(ords + 2 * i);
        if (ord >= nFuncs || !Bytes(funcs + 4 * ord, 4)) continue;
        uint32_t target = U32(funcs + 4 * ord);
        if (!target || (target >= rva && target - rva < size)) continue;  // forwarded
        exports.push_back({CString(U32(names + 4 * i), 512), target});
    }
}

void PeImage::ReadVersion(uint32_t rva, uint32_t size) {
    if (!rva || !Bytes(rva, 16) || size < 16) return;
    // Root -> RT_VERSION -> first name -> first language -> data entry.
    auto firstEntry = [&](uint32_t dirRva, int wantId, uint32_t& off) -> bool {
        if (!Bytes(dirRva, 16)) return false;
        uint32_t count = (uint32_t)U16(dirRva + 12) + U16(dirRva + 14);
        for (uint32_t i = 0; i < count && i < 4096; i++) {
            uint32_t e = dirRva + 16 + 8 * i;
            if (!Bytes(e, 8)) return false;
            uint32_t id = U32(e), to = U32(e + 4);
            if (wantId >= 0 && ((id & 0x80000000u) || id != (uint32_t)wantId)) continue;
            off = to;
            return true;
        }
        return false;
    };
    uint32_t off = 0;
    if (!firstEntry(rva, 16, off) || !(off & 0x80000000u)) return;
    if (!firstEntry(rva + (off & 0x7FFFFFFF), -1, off) || !(off & 0x80000000u)) return;
    if (!firstEntry(rva + (off & 0x7FFFFFFF), -1, off) || (off & 0x80000000u)) return;
    uint32_t dataEntry = rva + off;
    if (!Bytes(dataEntry, 16)) return;
    uint32_t data = U32(dataEntry), len = U32(dataEntry + 4);
    if (len < 92 || !Bytes(data, 92)) return;
    // VS_VERSIONINFO: 3 WORDs, L"VS_VERSION_INFO\0" (32 bytes), padding to 4 -> offset 40.
    if (U32(data + 40) != 0xFEEF04BD) return;
    fileVersion = (uint64_t)U32(data + 48) << 32 | U32(data + 52);
}

}  // namespace port
