// SPDX-License-Identifier: GPL-3.0-only
//
// Taskbar symbol preparation: read
// each module's RSDS record from disk, find or download its PDB, resolve the names in
// symbols.txt with the System32 dbghelp, and write the offsets file the runtime reads.
// When the symbol server has no PDB for a build yet (404), ported.h may carry the
// names over from an older published build instead.

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
    DWORD sizeOfHeaders = 0;
};

bool ReadCodeView(const std::wstring& path, CodeView& out);
// GUID as 32 upper-case hex digits followed by the age in hex: the symbol store key.
std::wstring PdbId(const CodeView& cv);
// The symbol server's key for the image itself: TimeDateStamp as 8 hex digits, then
// SizeOfImage in hex, both upper case.
std::wstring BinaryKey(DWORD timeDateStamp, DWORD sizeOfImage);

// VS_FIXEDFILEINFO's file version, e.g. 10.0.26100.9818.
struct FileVersion {
    uint16_t part[4] = {};
    bool valid = false;
    // Same major.minor.build: one servicing branch (26100 and 26300 are two).
    bool SameBranch(const FileVersion& o) const {
        return valid && o.valid && part[0] == o.part[0] && part[1] == o.part[1] && part[2] == o.part[2];
    }
    uint64_t Value() const {
        return (uint64_t)part[0] << 48 | (uint64_t)part[1] << 32 | (uint64_t)part[2] << 16 | part[3];
    }
    std::wstring Text() const;
};
bool ParseFileVersion(const std::wstring& text, FileVersion& out);
bool ReadFileVersion(const std::wstring& path, FileVersion& out);

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

// --- symbol server ---
// NotFound is the server's 404: the file is not published (yet). Everything else that
// is not a whole file is Failed.
enum class Download { Ok, NotFound, Failed, Stopped };
Download DownloadForStatus(DWORD httpStatus);
// Gets <file>/<key>/<file> from msdl.microsoft.com into dest (through dest.part).
// bytes: what this call received; progress sees doneBefore + bytes.
Download DownloadSymbolFile(const std::wstring& file, const std::wstring& key, const std::wstring& dest,
                            uint64_t doneBefore, const ProgressFn& progress, uint64_t& bytes);
// Windhawk's symbol store, which the host only ever reads.
std::wstring WindhawkSymbolsDir();

// Every public symbol of a PDB, loaded for dllPath, with the same options and name
// rules as the wanted-names resolve; the first address of a name wins. False with
// error set when the PDB does not load or does not match the DLL.
bool ResolveAllPublics(const std::wstring& pdbPath, const std::wstring& dllPath,
                       std::vector<std::pair<std::wstring, DWORD>>& out, std::wstring& error);

struct SymbolOptions {
    std::wstring dataDir;
    std::wstring symbolList;  // symbols.txt
    std::vector<SymbolModule> modules;
    bool noDownload = false;
    bool noWindhawkCache = false;
    std::wstring dumpDir;  // names-<module>.txt for each module, forces resolving
    // --resolve-symbols --simulate-404 <module>: that module's own PDB counts as not
    // published (no cache, no download), so porting can be tried on a published build.
    std::wstring simulate404;
    ProgressFn progress;
};

// What PrepareSymbols found, per module and overall.
struct ModuleReport {
    std::wstring module;  // as the mod names it
    std::wstring key;     // <module lower>_<PdbId>, the offsets file name without .txt
    int exit = 0;         // kExitOk or why this module has no offsets
    bool ported = false;  // its offsets came from porting (a .ported sidecar)
    std::wstring portedFrom;  // file version of the build they came from
};
struct SymbolReport {
    std::vector<ModuleReport> modules;
    // host-state reason for a failed result: "pdbMissing" (a 404 porting did not
    // cover), "portCrashed" (a ported build is quarantined), else empty (the code's).
    std::wstring reason;
    std::vector<std::wstring> PortedKeys() const;
    std::wstring PortedFrom() const;  // the versions ported from, comma separated
};

// Every module is tried; the result is the worst of theirs (WorstExit): kExitOk,
// kExitDownload, kExitNoSymbols (a module or its RSDS is missing) or kExitError, or
// kSymbolsStopped at once when progress asked to stop.
constexpr int kSymbolsStopped = -1;
int PrepareSymbols(const SymbolOptions& opt, SymbolReport* report = nullptr);
// Which of two module results decides the exit code.
int WorstExit(int a, int b);

}  // namespace host
