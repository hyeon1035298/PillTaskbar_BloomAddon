// SPDX-License-Identifier: GPL-3.0-only
//
// Ported offsets: when the symbol server has no PDB for a module build (404), the host
// downloads an older published build of the same DLL with its PDB and lets the porting
// engine (port.h) carry the wanted names over. What it writes is marked provisional
// (a .ported sidecar), replaced once Microsoft publishes the PDB, and guarded: an
// explorer crash while ported offsets are in use quarantines them
// (docs/addon-protocol.md, "옮겨 찾은 오프셋").

#pragma once

#include <windows.h>

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "port.h"
#include "symbols.h"
#include "util.h"

namespace host {

// Next to the host exe.
constexpr wchar_t kSymbolsMeta[] = L"symbols-meta.txt";
constexpr wchar_t kPortSources[] = L"port-sources.txt";
// In the data folder.
constexpr wchar_t kPortTmpDir[] = L"port-tmp";
constexpr wchar_t kPortedGuardFile[] = L"ported-guard.json";
// Beside offsets\<key>.txt.
constexpr wchar_t kPortedExt[] = L".ported";
constexpr wchar_t kPortFailExt[] = L".portfail";
constexpr wchar_t kQuarantinedExt[] = L".quarantined";

// While ported: how often the real PDB is asked for again.
constexpr uint64_t kPortProbeInterval = 6 * 60 * kTicksPerMinute;
// A failed port is tried again after this long, or sooner when the engine or the
// candidate builds change.
constexpr uint64_t kPortFailRetry = 24 * 60 * kTicksPerMinute;
// An attempt that judged every candidate it reached but could not reach a local one
// (its version probe failed) is tried again after this long: sooner than a judged
// failure, but not on every start, each of which would download the others again.
constexpr uint64_t kPortProbeFailRetry = 2 * 60 * kTicksPerMinute;
// Source builds tried per module, newest first.
constexpr size_t kMaxPortSources = 2;
// Local candidates whose version is unknown until their DLL is downloaded.
constexpr size_t kMaxVersionProbes = 2;

// <module lower>_<PdbId>, the offsets file name without ".txt".
std::wstring ModuleKey(const std::wstring& module, const CodeView& cv);

// --- symbols-meta.txt: name<TAB>req|opt, from the mod's SYMBOL_HOOK tables ---
// One flag per name; a name the file does not list (or no file) is required.
std::vector<bool> ParseSymbolsMeta(const std::string& utf8, const std::vector<std::wstring>& names);

// --- port-sources.txt ---
// "<module> <file version> <msdl key> [sha256]" lines, newest first, and
// "deny <module> <msdl key>" lines; # starts a comment.
struct SourceLine {
    std::wstring module;  // lower case
    FileVersion version;
    std::wstring key;     // upper case
    std::wstring sha256;  // lower case, may be empty
};
struct PortSources {
    std::vector<SourceLine> lines;
    std::vector<std::pair<std::wstring, std::wstring>> deny;  // module lower, key upper
};
bool ParsePortSources(const std::string& utf8, PortSources& out);
// exeDir's port-sources.txt; no file is an empty list. False (and out empty) when the
// file does not parse.
bool ReadPortSources(const std::wstring& exeDir, PortSources& out);
bool SourceDenied(const PortSources& src, const std::wstring& module, const std::wstring& key);

// One older build of the module the port may start from.
struct Candidate {
    std::wstring key;       // msdl key of the old DLL
    FileVersion version;    // invalid until known
    std::wstring sha256;    // pinned by port-sources.txt, may be empty
    bool local = false;     // from the user's own offsets\<module>_*.txt
    std::wstring localFile; // that file, to cross-check the old PDB against
    uint64_t localTime = 0; // its last write, orders local files of unknown version
};
// The user's offsets files for this module other than the current build's.
std::vector<Candidate> LocalCandidates(const std::wstring& offsetsDir, const std::wstring& module,
                                       const CodeView& current);
// Local candidates first, then the shipped lines for the module; one entry per key,
// a local file whose key is shipped takes the line's version and hash.
std::vector<Candidate> MergeCandidates(const std::vector<Candidate>& local, const PortSources& src,
                                       const std::wstring& module);
// Keeps those of the new build's branch, older than it, not denied, with a known
// version; newest first, at most max.
std::vector<Candidate> FilterCandidates(const std::vector<Candidate>& all, const PortSources& src,
                                        const std::wstring& module, const FileVersion& newVersion, size_t max);
// The candidate keys joined, the identity of one port attempt.
std::wstring CandidateSet(const std::vector<Candidate>& c);

// --- the sidecar offsets\<key>.ported ---
struct PortedName {
    std::wstring name;
    port::Outcome outcome = port::Outcome::NotFound;
    uint32_t rva = 0;
    std::wstring evidence;
};
struct Sidecar {
    int engine = 0;
    std::wstring hostVersion;
    std::wstring sourceVersion, sourceKey, sourceKind;  // kind: local or shipped
    std::wstring sources;  // the attempt's candidate set, as .portfail's sources
    uint64_t date = 0, lastProbe = 0;
    std::vector<PortedName> names;
};
std::string FormatSidecar(const Sidecar& s);
bool ParseSidecar(const std::string& utf8, Sidecar& out);
bool ProbeDue(const Sidecar& s, uint64_t now);
const wchar_t* OutcomeName(port::Outcome o);
// Why the ported offsets offsets\<key>.txt may no longer be used, or empty when they
// may: they are quarantined, their sidecar does not parse, another engine version
// ported them, or their source build is now denied in port-sources.txt.
std::wstring PortedStale(const std::wstring& offsetsDir, const std::wstring& key, const std::wstring& module,
                         const PortSources& src);
// Deletes offsets\<key>.txt and then its sidecar; the sidecar stays when the offsets
// file could not be deleted, so what is left never passes for real offsets. True when
// the offsets file is gone.
bool DropPorted(const std::wstring& offsetsDir, const std::wstring& key);

// --- the module-write rule ---
// From the engine's results (one per wanted name, in order) builds the offsets file:
// a Ported RVA inside [SizeOfHeaders, SizeOfImage) is kept, everything else is empty.
// Two names on one new RVA that were apart in the old build lose theirs. False, with
// the names in missing, when a required name that the old PDB has is not ported: then
// nothing may be written for the module.
bool BuildPortedOffsets(const std::wstring& module, const CodeView& cv, const std::vector<port::Want>& wanted,
                        const std::vector<port::OldSymbol>& oldSymbols, const std::vector<port::Result>& results,
                        OffsetsFile& out, std::vector<PortedName>& names, std::vector<std::wstring>& missing);
// Sidecar first, then the offsets file, each written whole; order (when given) gets
// the paths in the order written. False when either write failed; then the sidecar
// just written is deleted again.
bool WritePortedFiles(const std::wstring& offsetsPath, const std::string& offsets, const std::wstring& sidecarPath,
                      const std::string& sidecar, std::vector<std::wstring>* order = nullptr);
// "N same, M differ: a, b" between ported entries and the real ones.
std::wstring PortDiff(const OffsetsFile& ported, const OffsetsFile& real, size_t& differ);

// --- offsets\<key>.portfail ---
struct PortFail {
    int engine = 0;
    std::wstring hostVersion;
    std::wstring sources;
    uint64_t at = 0;
    std::wstring why;
    uint64_t retry = kPortFailRetry;  // "retry" in minutes; a record without it holds a day
};
std::string FormatPortFail(const PortFail& f);
bool ParsePortFail(const std::string& utf8, PortFail& out);
// True while a failed attempt with the same engine, host and candidates is younger than
// its retry (a day unless it says otherwise).
bool PortFailBlocks(const PortFail& f, int engine, const std::wstring& hostVersion, const std::wstring& sources,
                    uint64_t now);
// What a skipped attempt reports: when the record's attempt failed, why, and when its
// own retry lets the next one start.
std::wstring PortFailSkipDetail(const PortFail& f);

// --- the crash guard, ported-guard.json in the data folder ---
struct PortGuard {
    std::vector<std::wstring> keys;
    uint64_t armedAt = 0;
};
std::string FormatGuard(const PortGuard& g);
bool ParseGuard(const std::string& utf8, PortGuard& out);
bool ReadGuard(const std::wstring& dataDir, PortGuard& out);
// Armed with exactly these keys (armedAt kept when they are the same); an empty list
// disarms. False when the file could not be written or removed.
bool SetGuard(const std::wstring& dataDir, const std::vector<std::wstring>& keys, uint64_t now);
// For every armed key: offsets\<key>.quarantined (reason), the ported offsets and
// sidecar deleted (DropPorted). Then disarms, except for keys whose offsets file is
// still there. Returns the keys quarantined.
std::vector<std::wstring> QuarantineGuarded(const std::wstring& dataDir, const std::wstring& reason, uint64_t now);
bool IsQuarantined(const std::wstring& offsetsDir, const std::wstring& key);
// The runtime would not start on these ported offsets (initFailed, symbols: a required
// name came out empty). For each key a .portfail with the sidecar's sources, so the
// build is not ported again for a day and waits for its real PDB, then the offsets
// and sidecar go (DropPorted); the guard keeps only keys whose offsets are still
// there. Returns the keys whose offsets went.
std::vector<std::wstring> RetractPorted(const std::wstring& dataDir, const std::vector<std::wstring>& keys,
                                        const std::wstring& why, uint64_t now);
// A name required in symbols-meta.txt (beside exeDir) that the real offsets
// offsets\<key>.txt leave empty while the real offsets of an older build of the same
// module had it: Microsoft took it out of that module, so the runtime's "symbols" may
// be this module's, which porting another one cannot mend. Every offsets file lists
// every name of symbols.txt, so only an older build of the module shows that a name is
// its own. Even that is no proof for a name another module has too (a template or COMDAT
// that both PDBs list, while only the other module's table hooks it): a name with an
// address in the offsets of one of others (this run's other modules, real or ported)
// does not count. Empty when there is none, or when the offsets are ported.
std::wstring LostRequiredName(const std::wstring& dataDir, const std::wstring& exeDir, const std::wstring& key,
                              const std::vector<std::wstring>& others = {});
// A quarantine holds against porting again only for the engine version that wrote it
// (a record that names none holds for every engine).
bool QuarantineBlocks(const std::wstring& offsetsDir, const std::wstring& key, int engine);

// --- port-tmp ---
// port-tmp\<pid>: each process downloads into its own folder, so two hosts or a
// --resolve-symbols on one data folder never delete each other's files.
std::wstring PortTmpDir(const std::wstring& dataDir);
// Deletes what is in port-tmp but a folder whose process still runs.
void CleanStalePortTmp(const std::wstring& dataDir);

// --- verifying a downloaded old DLL ---
// Embedded Authenticode, or a catalog signature this PC knows.
bool SignatureValid(const std::wstring& path, std::wstring& how);
bool FileSha256(const std::wstring& path, std::wstring& hex);

// --- one module ---
enum class PortStatus {
    Written,      // offsets and sidecar written
    Unported,     // a verdict on the sources: a 404, a hash, signature, version or key
                  // mismatch, a local file that disagrees, required names not ported
    Skipped,      // a recent .portfail for the same engine and sources
    Quarantined,  // this build crashed explorer with ported offsets before
    Failed,       // no conclusion, nothing remembered: a download failed (not a 404) or
                  // a local step did (reading a file, dbghelp, the engine, writing)
    Stopped,      // progress asked to stop
};
struct PortRequest {
    std::wstring dataDir;
    std::wstring exeDir;  // symbols-meta.txt, port-sources.txt
    SymbolModule module;
    CodeView cv;
    std::vector<std::wstring> names;  // symbols.txt
    bool noDownload = false;  // caches only: a file that is not cached ends the try as Failed
    bool noWindhawkCache = false;
    // --simulate-404 named this module: its PDB may well be published, so the sidecar
    // gets lastProbe 0 and the next start asks for it at once.
    bool simulated = false;
    ProgressFn progress;
    // The checks of a downloaded DLL, reading the old PDB and the engine run on a
    // worker thread; idle keeps the calling thread serving meanwhile.
    IdleFn idle;
    uint64_t* downloaded = nullptr;  // running byte count for progress
};
struct PortReport {
    std::wstring sourceVersion;
    std::wstring detail;  // for the log
};
PortStatus PortModule(const PortRequest& req, PortReport& report);
// The NEW build port::Run must find: the CodeView its offsets will be labelled with
// (TimeDateStamp, SizeOfImage, RSDS GUID and age).
port::Identity ExpectedNew(const CodeView& cv);
// Runs work on a worker thread while this thread calls idle about every 100 ms (the
// host's messages, forwarded commands, Stop), or just waits without one. False when
// idle asked to stop: work's cancel flag is set, the worker gets a few seconds to
// end, and is otherwise left running (PortWorkerLeftRunning). work must own all it
// touches.
bool RunOnWorker(std::function<void(const std::atomic<bool>& cancel)> work, const IdleFn& idle);
// True once a port was stopped while its worker still ran. The worker owns what it
// touches, but the process should then end with ExitProcess, which ends the worker
// first, rather than run static destructors under it.
bool PortWorkerLeftRunning();

}  // namespace host
