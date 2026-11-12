// SPDX-License-Identifier: GPL-3.0-only
//
// <name>Host.exe (PillTaskbarHost.exe): prepares the symbols, puts the runtime DLL into
// the explorer that owns the taskbar, watches explorer and Bloom, and takes the
// runtime out again. It shows no window. docs/addon-protocol.md is the contract.

#include <windows.h>

#include "addon_protocol.h"
#include "args.h"
#include "commands.h"
#include "ported.h"
#include "util.h"

using namespace host;

namespace {

const wchar_t kUsage[] =
    L"usage:\n"
    L"  " ADDON_NAME L"Host --serve <BloomPid> --bloom <exe> --bloom-local <dir> [--reset-safe-mode] [--log]\n"
    L"                  [--mod <file>|none] [--dry-run]\n"
    L"  " ADDON_NAME L"Host --stop\n"
    L"  " ADDON_NAME L"Host --safe-mode [--bloom-local <dir>]\n"
    L"  " ADDON_NAME L"Host --resolve-symbols --bloom-local <dir> [--no-download] [--no-windhawk-cache] [--dump <dir>]\n"
    L"                  [--simulate-404 <module>]\n"
    L"  " ADDON_NAME L"Host --self-test";

}  // namespace

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
    // DLLs the host loads by name come from System32 only, never the current folder.
    SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);

    Args a;
    if (!ParseArgs(SplitCommandLine(GetCommandLineW()), a)) {
        EnablePrint();
        Print(ADDON_NAME L"Host: " + a.error);
        Print(kUsage);
        return addon::kExitError;
    }
    // --serve's stdout may be a pipe Bloom never reads, so only the check modes print.
    if (a.command != Command::Serve) EnablePrint();
    int code = addon::kExitError;
    switch (a.command) {
        case Command::Serve: code = RunServe(a); break;
        case Command::Stop: code = RunStop(a); break;
        case Command::SafeMode: code = RunSafeMode(a); break;
        case Command::ResolveSymbols: code = RunResolveSymbols(a); break;
        case Command::SelfTest: code = RunSelfTest(); break;
        default: break;
    }
    // A port stopped while its worker still ran: ExitProcess ends that thread before
    // anything else, so no static destructor runs under it.
    if (PortWorkerLeftRunning()) ExitProcess((UINT)code);
    return code;
}
