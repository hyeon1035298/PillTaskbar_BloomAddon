// SPDX-License-Identifier: GPL-3.0-only
//
// Entry points for each host command, and the single-instance plumbing they share.

#pragma once

#include <windows.h>

#include <string>

#include "args.h"

namespace host {

int RunServe(const Args& a);
int RunStop(const Args& a);
int RunSafeMode(const Args& a);
int RunResolveSymbols(const Args& a);
int RunSelfTest();

// --- single instance (docs/addon-protocol.md section 2) ---
bool HostRunning();
// Hands a command line to the running host's hidden window by WM_COPYDATA.
bool ForwardToHost(const std::wstring& commandLine);
// Waits until the running host lets go of its mutex.
bool WaitForHostExit(DWORD ms);
// Unloads the runtimes no host looks after any more (their host died): explorer's and
// ShellHost's, also for an add-on without ShellHost.exe in ADDON_TARGETS (one an
// earlier version left there).
bool UnloadOrphanRuntime();

}  // namespace host
