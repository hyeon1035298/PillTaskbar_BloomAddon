// SPDX-License-Identifier: GPL-3.0-only
//
// Finding the explorer that owns the taskbar, and reading things about it from
// outside: its modules and its process creation time.

#pragma once

#include <windows.h>

#include <cstdint>
#include <string>
#include <vector>

namespace host {

struct Taskbar {
    HWND tray = nullptr;  // Shell_TrayWnd
    DWORD pid = 0;
    DWORD tid = 0;        // the thread that owns Shell_TrayWnd
};

bool FindTaskbar(Taskbar& out);

// A new explorer builds its XAML taskbar after Shell_TrayWnd appears. It counts as
// ready once the taskbar holds its XAML island and explorer is kExplorerSettleMs old,
// or, for a taskbar that never gets an island, once it is kExplorerReadyMaxMs old.
constexpr uint64_t kExplorerSettleMs = 3000;
constexpr uint64_t kExplorerReadyMaxMs = 15000;
bool ExplorerReady(bool hasIsland, uint64_t ageMs);
// Whether Shell_TrayWnd holds its XAML island window.
bool TaskbarHasIsland(HWND tray);
// Milliseconds since the process started; UINT64_MAX when it cannot be opened.
uint64_t ProcessAgeMs(DWORD pid);
// Full paths of the modules loaded in a process (empty when it cannot be read).
std::vector<std::wstring> ProcessModules(DWORD pid);
// Process creation time as FILETIME ticks, 0 when the process cannot be opened.
uint64_t ProcessCreationTime(HANDLE process);
uint64_t ProcessCreationTime(DWORD pid);
std::wstring ProcessImagePath(HANDLE process);
// True when a process with this id and creation time still runs.
bool ProcessAlive(DWORD pid, uint64_t created);

}  // namespace host
