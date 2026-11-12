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
// The two halves of a BinaryKey; false when it is not one.
bool ParseBinaryKey(const std::wstring& key, DWORD& timeDateStamp, DWORD& sizeOfImage);

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

// Every public of the PDB at pdbPath for the module file at path, through System32's
// dbghelp, in enumeration order: publics only, undecorated as msdia does (FixPtr64).
// The host's offsets and the porter's OLD names (port::ResolveAll) both come from this
// one walk, so the two name a symbol alike. onPublic gets each name and RVA (an address
// below the image base or more than 4 GB past it is skipped) and returns false to end
// the walk, which sets *stopped. False, with error set, when dbghelp, the module or the
// matching PDB did not load. dbghelp is not thread safe: the host calls this from its
// main thread, and the porter's worker only while the main thread waits for it.
using PublicFn = std::function<bool(std::wstring& name, DWORD rva)>;
bool EnumPublics(const std::wstring& path, const CodeView& cv, const std::wstring& pdbPath,
                 const PublicFn& onPublic, std::wstring& error, size_t* count = nullptr, bool* stopped = nullptr);

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
// Called on the main thread about every 100 ms while a long step (porting) runs on a
// worker: pumps messages and takes forwarded commands. Returns false to stop.
using IdleFn = std::function<bool()>;

// --- symbol server ---
// NotFound is the server's 404: the file is not published (yet). Everything else that
// is not a whole file is Failed.
enum class Download { Ok, NotFound, Failed, Stopped };
Download DownloadForStatus(DWORD httpStatus);
// What one download may be. The PDBs of the add-ons' modules measured 10 to 50 MB
// (taskbar, Taskbar.View, SystemTray, twinui.pcshell, builds 8655 to 9818), so 512 MB
// leaves ten times the room. An old DLL's file measured 0.98 to 1.02 of its
// SizeOfImage (the signature lies past the image): a quarter more plus 1 MB.
constexpr uint64_t kMaxPdbBytes = 512ull << 20;
inline uint64_t DllDownloadCap(DWORD sizeOfImage) {
    return (uint64_t)sizeOfImage + sizeOfImage / 4 + (1ull << 20);
}
// However the bytes trickle in, one file ends here (Failed).
constexpr uint64_t kDownloadDeadlineMs = 30 * 60 * 1000;
// Gets <file>/<key>/<file> from msdl.microsoft.com into dest (through dest.part).
// bytes: what this call received; progress sees doneBefore + bytes. A Content-Length
// above maxBytes is refused before anything is written, a body that grows past it
// or outlasts kDownloadDeadlineMs is cut off; both are Failed.
Download DownloadSymbolFile(const std::wstring& file, const std::wstring& key, const std::wstring& dest,
                            uint64_t maxBytes, uint64_t doneBefore, const ProgressFn& progress, uint64_t& bytes);
// Windhawk's symbol store, which the host only ever reads.
std::wstring WindhawkSymbolsDir();

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
    IdleFn idle;  // none: the main thread just waits for the worker
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
