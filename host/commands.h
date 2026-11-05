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
// Unloads a runtime that no host looks after any more (its host died).
bool UnloadOrphanRuntime();

}  // namespace host
