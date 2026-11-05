// SPDX-License-Identifier: GPL-3.0-only
//
// The host command line (docs/addon-protocol.md section 2) plus the two offline
// check modes, --dry-run and --self-test.

#pragma once

#include <windows.h>

#include <string>
#include <vector>

namespace host {

enum class Command { None, Serve, Stop, SafeMode, ResolveSymbols, SelfTest };

struct Args {
    Command command = Command::None;
    DWORD bloomPid = 0;
    std::wstring bloomExe;    // --bloom, optional
    std::wstring bloomLocal;  // --bloom-local
    std::wstring mod;         // --mod file name; empty: runtime only (--mod none)
    std::wstring dumpDir;     // --dump
    bool resetSafeMode = false;
    bool log = false;
    bool noDownload = false;
    bool noWindhawkCache = false;
    bool dryRun = false;
    std::wstring error;       // why parsing failed
};

// Splits a command line the way the C runtime does. A leading program path (any
// token not starting with "--") is dropped, so both GetCommandLineW() and a bare
// argument string work.
std::vector<std::wstring> SplitCommandLine(const std::wstring& line);
bool ParseArgs(const std::vector<std::wstring>& argv, Args& out);

// <bloom-local>\addons\pill-taskbar
std::wstring DataDirFor(const std::wstring& bloomLocal);
// %LOCALAPPDATA%\Bloom, for --stop and --safe-mode when no --bloom-local is given.
std::wstring DefaultBloomLocal();

}  // namespace host
