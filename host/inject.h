// SPDX-License-Identifier: GPL-3.0-only
//
// The mechanics of docs/addon-protocol.md section 4: the Config mapping and the three
// explorer-side events, the WH_GETMESSAGE hook that brings the runtime into the
// taskbar thread, and the unload handshake.

#pragma once

#include <windows.h>

#include <functional>
#include <string>
#include <vector>

#include "addon_protocol.h"
#include "taskbar.h"
#include "util.h"

namespace host {

// Waits on handles while the host window keeps getting its messages. Returns the
// index of the signaled handle, kWaitTimeout, or (when honorStop) kWaitStop as soon
// as a stop has been asked for.
constexpr int kWaitTimeout = -1;
constexpr int kWaitStop = -2;
using WaitFn = std::function<int(const std::vector<HANDLE>& handles, DWORD ms, bool honorStop)>;

// A kernel object name with the explorer pid filled in.
std::wstring ObjectName(const wchar_t* format, DWORD pid);

// True when a runtime in this explorer has already said it is loaded.
bool RuntimeLoadedIn(DWORD pid);

struct Explorer {
    Taskbar taskbar;
    Handle process;     // SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION
    uint64_t created = 0;
    Handle mapping;     // Config, kept until the runtime has copied it
    Handle loaded, unload, unloaded;
    uint64_t injectedAt = 0;
    bool runtimeIn = false;  // a runtime is in this explorer and has not unloaded
};

bool OpenExplorer(const Taskbar& t, Explorer& e);
// Opens the three events of an existing runtime (attach) or creates them.
bool OpenEvents(Explorer& e, bool create);
// Creates the Config mapping and fills it.
bool CreateConfig(Explorer& e, const addon::Config& config);

enum class HookResult { Loaded, NotLoaded, HookFailed, Stopped, ExplorerGone };
// Loads the runtime into the host for its hook procedure, hooks the taskbar thread,
// nudges it with WM_NULL every 500 ms until Loaded (10 s), then unhooks.
HookResult HookUntilLoaded(Explorer& e, const std::wstring& runtimePath, const WaitFn& wait, DWORD& error);

// Sets Unload and waits for Unloaded. False on timeout; explorer is left alone.
bool UnloadRuntime(Explorer& e, DWORD timeoutMs, const WaitFn& wait);

}  // namespace host
