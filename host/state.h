// SPDX-License-Identifier: GPL-3.0-only
//
// host-state.json and runtime-state.json (docs/addon-protocol.md section 1) and the
// crash rules that decide when safe mode turns on.

#pragma once

#include <windows.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace host {

struct LoadedRecord {
    DWORD pid = 0;
    uint64_t created = 0;  // explorer process creation time, FILETIME
    uint64_t at = 0;       // when the host wrote it, just before injecting
};

struct HostState {
    std::wstring version;
    DWORD pid = 0;
    std::wstring phase = L"starting";
    std::wstring reason;
    uint64_t done = 0, total = 0;
    bool safeMode = false;
    std::optional<LoadedRecord> loaded;
    std::vector<uint64_t> crashes;  // FILETIME of each counted crash
    uint64_t at = 0;
};

std::string SerializeHostState(const HostState& s);
bool ParseHostState(const std::string& utf8, HostState& out);

struct RuntimeState {
    std::wstring version;
    DWORD pid = 0;
    std::wstring state;
    std::wstring reason;
    uint64_t at = 0;
};
bool ParseRuntimeState(const std::string& utf8, RuntimeState& out);

// The data-folder files. Load returns defaults when the file is missing or broken.
class StateFile {
public:
    void Init(const std::wstring& dataDir, bool readOnly);
    HostState& state() { return state_; }
    void Load();
    // Stamps version, pid and time, drops crashes older than the window, writes.
    bool Save();
    void SetPhase(const std::wstring& phase, const std::wstring& reason = {});
    bool ReadRuntime(RuntimeState& out) const;

private:
    std::wstring path_, runtimePath_;
    bool readOnly_ = false;
    HostState state_;
};

// Crash rules: a crash counts when explorer died with an NTSTATUS
// error code or left an Application Error event, within 5 minutes of injecting; two
// counted crashes within 10 minutes turn safe mode on.
constexpr uint64_t kCrashAfterInjectWindow = 5 * 60 * 10000000ull;
constexpr uint64_t kCrashCountWindow = 10 * 60 * 10000000ull;
constexpr size_t kCrashesForSafeMode = 2;

bool IsCrashExitCode(DWORD code);
bool CrashCounts(uint64_t injectedAt, uint64_t diedAt);
void PruneCrashes(std::vector<uint64_t>& crashes, uint64_t now);
// Adds a counted crash and says whether safe mode should turn on.
bool RecordCrash(HostState& s, uint64_t diedAt, uint64_t now);

}  // namespace host
