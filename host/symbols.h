// SPDX-License-Identifier: GPL-3.0-only
//
// Taskbar symbol preparation: read
// each module's RSDS record from disk, find or download its PDB, resolve the names in
// symbols.txt with the System32 dbghelp, and write the offsets file the runtime reads.

#pragma once

#include <windows.h>

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace host {

struct CodeView {
    GUID guid{};
    DWORD age = 0;
    std::wstring pdbName;  // file name only
    DWORD timeDateStamp = 0;
    DWORD sizeOfImage = 0;
};

bool ReadCodeView(const std::wstring& path, CodeView& out);
// GUID as 32 upper-case hex digits followed by the age in hex: the symbol store key.
std::wstring PdbId(const CodeView& cv);

// dbghelp's undecorator ignores UNDNAME_NO_PTR64; this turns its output into what
// msdia (and so Windhawk and the mod's strings) gives.
std::wstring FixPtr64(std::wstring name);

// One offsets file: "1#<module>#<stamp>-<size>#<name>#<rva>#...". A name with no RVA
// was looked for and is not in the module.
struct OffsetsFile {
    std::wstring module;  // lower case
    DWORD timeDateStamp = 0;
    DWORD sizeOfImage = 0;
    std::vector<std::pair<std::wstring, std::optional<DWORD>>> entries;
};
std::string FormatOffsets(const OffsetsFile& f);
bool ParseOffsets(const std::string& utf8, OffsetsFile& out);
// The file name under offsets\: <module lower>_<PdbId>.txt
std::wstring OffsetsFileName(const std::wstring& module, const CodeView& cv);
// True when the file matches this module build and covers every wanted name.
bool OffsetsMatch(const OffsetsFile& f, const std::wstring& module, const CodeView& cv,
                  const std::vector<std::wstring>& names);

struct SymbolModule {
    std::wstring name;  // as the mod names it, e.g. Taskbar.View.dll
    std::wstring path;  // file on disk
};
// The add-on's symbol modules (addon::kSymbolModules). Package paths come from
// explorer's loaded modules when given, else the usual System32 / SystemApps locations.
std::vector<SymbolModule> AddonModules(const std::vector<std::wstring>& explorerModules);

bool ReadSymbolList(const std::wstring& path, std::vector<std::wstring>& names);

// Returns false to stop (the host was asked to stop).
using ProgressFn = std::function<bool(uint64_t done, uint64_t total)>;

struct SymbolOptions {
    std::wstring dataDir;
    std::wstring symbolList;  // symbols.txt
    std::vector<SymbolModule> modules;
    bool noDownload = false;
    bool noWindhawkCache = false;
    std::wstring dumpDir;  // names-<module>.txt for each module, forces resolving
    ProgressFn progress;
};

// kExitOk, kExitNoSymbols (a module or its RSDS is missing), kExitDownload,
// kExitError, or kSymbolsStopped when progress asked to stop.
constexpr int kSymbolsStopped = -1;
int PrepareSymbols(const SymbolOptions& opt);

}  // namespace host
