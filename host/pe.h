// SPDX-License-Identifier: GPL-3.0-only
//
// A bounds-checked reader for 64-bit PE files, for the symbol porter: an image-shaped
// copy of the file (an RVA indexes it directly), sections, .pdata, base relocations,
// imports and delay imports, exports, the RSDS record and the file version. Nothing is
// executed or mapped as an image; every read past the image comes back empty.

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace port {

struct PeSection {
    std::string name;
    uint32_t va = 0;
    uint32_t span = 0;  // max(VirtualSize, SizeOfRawData) when there is raw data
    uint32_t characteristics = 0;
    bool exec = false, write = false;
};

struct PeRuntimeFunction {
    uint32_t begin = 0, end = 0, unwind = 0;
    bool chained = false;  // UNW_FLAG_CHAININFO, or an unwind field that points at another entry
};

struct PeExport {
    std::string name;
    uint32_t rva = 0;
};

class PeImage {
public:
    bool Load(const std::wstring& path, std::wstring& error);

    uint16_t machine = 0;
    uint32_t timeDateStamp = 0;
    uint32_t sizeOfImage = 0;
    uint32_t sizeOfHeaders = 0;
    uint64_t imageBase = 0;

    // RSDS
    bool hasCodeView = false;
    uint8_t guid[16] = {};
    uint32_t age = 0;
    std::string pdbName;

    // VS_FIXEDFILEINFO file version, 0 when there is none
    uint64_t fileVersion = 0;

    std::vector<PeSection> sections;
    std::vector<PeRuntimeFunction> pdata;   // in table order
    std::vector<uint32_t> relocs;           // IMAGE_REL_BASED_DIR64 slots, sorted
    std::unordered_map<uint32_t, std::string> iat;  // IAT slot -> "dll!name" (dll lower case)
    std::vector<PeExport> exports;          // named, not forwarded

    const uint8_t* Bytes(uint32_t rva, uint32_t n) const {
        if (rva > img_.size() || n > img_.size() - rva) return nullptr;
        return img_.data() + rva;
    }
    uint32_t U32(uint32_t rva) const;
    uint64_t U64(uint32_t rva) const;
    uint16_t U16(uint32_t rva) const;
    uint8_t U8(uint32_t rva) const { return rva < img_.size() ? img_[rva] : 0; }
    // A NUL-terminated byte string at rva, at most limit bytes.
    std::string CString(uint32_t rva, size_t limit = 512) const;

    const PeSection* SectionOf(uint32_t rva) const;
    bool IsExec(uint32_t rva) const;
    bool IsReadOnlyData(uint32_t rva) const;  // in a section that is neither executable nor writable
    bool IsReloc(uint32_t rva) const { return rva < relocMap_.size() * 64 && (relocMap_[rva >> 6] >> (rva & 63) & 1); }
    // The 64-bit pointer at a relocated slot, as an RVA; false when the slot is not relocated
    // or the pointer leaves the image.
    bool PointerAt(uint32_t slot, uint32_t& rva) const;

private:
    std::vector<uint8_t> img_;
    std::vector<uint64_t> relocMap_;
    std::vector<std::pair<uint32_t, uint32_t>> execRanges_, roRanges_;  // sorted [va, va+span)

    void ReadPdata(uint32_t rva, uint32_t size);
    void ReadRelocs(uint32_t rva, uint32_t size);
    void ReadImports(uint32_t rva, uint32_t size, bool delay);
    void ReadExports(uint32_t rva, uint32_t size);
    void ReadVersion(uint32_t rva, uint32_t size);
};

}  // namespace port
