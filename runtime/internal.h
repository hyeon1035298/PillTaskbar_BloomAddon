// SPDX-License-Identifier: GPL-3.0-only
//
// Shared between the runtime's source files. Not seen by mods.

#pragma once

#define PILL_RUNTIME_BUILD
#include "pill_runtime_api.h"

#include "addon_protocol.h"

#include <atomic>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace rt {

extern HMODULE g_self;
extern addon::Config g_config;  // copied from the host's mapping at start

// <data folder>\<name>
std::wstring DataPath(std::wstring_view name);
// Serving ShellHost.exe rather than explorer (addon::kConfigPanel): its own state file
// and log.
inline bool InPanel() {
    return (g_config.flags & addon::kConfigPanel) != 0;
}

std::string Utf8(std::wstring_view text);
std::wstring Wide(std::string_view text);
std::string IsoNow();

// Reads a whole file, retrying briefly while another process replaces it. On failure
// *missing (when given) says whether the file or its folder does not exist, as opposed
// to a file that is there but could not be read.
bool ReadFileBytes(const std::wstring& path, std::string& out, bool* missing = nullptr);
// Writes through a temporary file in the same folder and renames it into place.
bool ReplaceFile(const std::wstring& path, std::string_view bytes);

// runtime-state.json for Bloom and the host.
void WriteState(PCWSTR state, PCWSTR reason = L"");

namespace log {
void Open(const std::wstring& path);
void Close();
// The runtime's own lines; always written.
void Line(PCWSTR format, ...);
// Wh_Log; written only while the mod's logging is on.
void Mod(PCWSTR format, va_list args);
extern std::atomic<bool> g_modLog;
}  // namespace log

struct JsonValue {
    enum Kind { kNull, kBool, kNumber, kString, kOther } kind = kNull;
    long long number = 0;  // kBool: 0 or 1
    std::wstring text;
};
using JsonObject = std::unordered_map<std::wstring, JsonValue>;
// A top-level object; nested objects and arrays are skipped as kOther.
bool ParseJsonObject(std::string_view utf8, JsonObject& out);
std::string JsonString(std::wstring_view text);

namespace settings {
enum class Reread { kSame, kChanged, kFailed };
// Rereads values.json. kFailed: the file is there but could not be read; the last
// values stay. The first read of a run answers kSame unless it failed.
Reread Reload();
// Forgets everything read so far, for a new run in an image that stayed mapped.
void Reset();
// The mod's YAML defaults, copied: they must outlive the mod DLL's data.
void SetDefaults(const PillAddonDefault* defaults, size_t count);
// A "$" runtime value from values.json, or fallback when absent or not a number.
int RuntimeInt(PCWSTR name, int fallback);
}  // namespace settings

// An address range of code, [start, end).
struct CodeRange {
    uintptr_t start;
    uintptr_t end;
};

namespace hooks {
bool Initialize();
void Uninitialize();
// Before a mod DLL's Init: starts MinHook afresh, then allows new hooks and refuses
// ApplyHookOperations. False when hooks of an earlier mod load are still there.
bool BeginMod();
// After Init returned TRUE: applies what Init queued and allows ApplyHookOperations.
void EndInit();
// After BeforeUninit: refuses new hooks and switches every hook off. False when a
// hook could not be switched off even on a second try.
bool DisableAll();
// After the threads have left the mod: frees the trampolines of every hook the mod
// made. False when one could not be removed even on a second try.
bool RemoveAll();
// The trampoline slots of the current hooks; a thread may be running in them.
void AddTrampolines(std::vector<CodeRange>& out);
// A required symbol was missing in this mod load (runtime-state reason "symbols").
extern std::atomic<bool> g_symbolsFailed;
}  // namespace hooks

namespace threads {
// The executable sections of a mapped image.
void AddImageCode(HMODULE module, std::vector<CodeRange>& out);
// Waits until no other thread of this process runs in the ranges or has a return
// address into them on its stack, checking every 400 ms. False when one still does
// at the deadline (GetTickCount64), or when the check itself could not run.
bool WaitUntilOut(const std::vector<CodeRange>& ranges, ULONGLONG deadline);
}  // namespace threads

}  // namespace rt
