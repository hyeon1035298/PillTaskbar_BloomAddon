// SPDX-License-Identifier: GPL-3.0-only
//
// What the add-on's host, its runtime and Bloom agree on. Bloom keeps its own copy of
// the names and codes (AddonProtocol.cs); change both together.
// docs/addon-protocol.md describes the files and the order of events.
//
// The names that differ between add-ons (id, version, file names, the explorer-side
// object prefix, the Windhawk mod ids, the symbol modules) come from the add-on's
// addon.conf through gen/addon_identity.h (tools/make-identity.py).

#pragma once

#include <windows.h>

#include "addon_identity.h"

namespace addon {

constexpr wchar_t kId[] = ADDON_ID;
constexpr wchar_t kVersion[] = ADDON_VERSION;
// addon.json host.protocol. Raise it when a name, file or code below changes meaning.
constexpr DWORD kProtocol = 1;

// In the addon data folder, <Bloom local>\addons\<id>.
constexpr wchar_t kValuesFile[] = L"values.json";
constexpr wchar_t kHostStateFile[] = L"host-state.json";
constexpr wchar_t kRuntimeStateFile[] = L"runtime-state.json";
constexpr wchar_t kHostLog[] = L"host.log";
constexpr wchar_t kRuntimeLog[] = L"runtime.log";
constexpr wchar_t kOffsetsDir[] = L"offsets";
constexpr wchar_t kSymbolsDir[] = L"symbols";

// Next to the host exe (the run copy Bloom makes of the shipped folder).
constexpr wchar_t kRuntimeDll[] = ADDON_NAME L"Runtime.dll";
constexpr wchar_t kModDll[] = ADDON_NAME L".dll";
constexpr wchar_t kSymbolList[] = L"symbols.txt";

// Named objects. %u is the explorer process id. The explorer-side names never carry
// a version, so a newer host can still ask an older runtime to unload.
constexpr wchar_t kHostMutex[] = L"Local\\Bloom.Addon." ADDON_ID L".Host";
constexpr wchar_t kHostStopEvent[] = L"Local\\Bloom.Addon." ADDON_ID L".Stop";
constexpr wchar_t kHostWindowClass[] = L"Bloom.Addon." ADDON_ID L".Host";
constexpr wchar_t kConfigMapping[] = L"Local\\" ADDON_NAME L".Config.%u";
constexpr wchar_t kLoadedEvent[] = L"Local\\" ADDON_NAME L".Loaded.%u";
constexpr wchar_t kUnloadEvent[] = L"Local\\" ADDON_NAME L".Unload.%u";
constexpr wchar_t kUnloadedEvent[] = L"Local\\" ADDON_NAME L".Unloaded.%u";
constexpr wchar_t kRuntimeMutex[] = L"Local\\" ADDON_NAME L".Runtime.%u";

// The Windhawk mod ids of the same mod: while one is on in Windhawk the host exits
// kExitWindhawk and the runtime refuses. Windhawk names the loaded DLL
// "<id>_<version>_<n>.dll".
constexpr const wchar_t* kWindhawkMods[] = {ADDON_WINDHAWK_MODS};

// The modules the host resolves symbols.txt in. inPackage: the Client.Core package
// folder, where a copy explorer has loaded wins; otherwise System32.
struct SymbolModuleName {
    const wchar_t* name;
    bool inPackage;
};
constexpr SymbolModuleName kSymbolModules[] = {ADDON_SYMBOL_MODULES};

// The runtime export the host passes to SetWindowsHookEx(WH_GETMESSAGE).
constexpr char kHookProcName[] = "PillTaskbarHookProc";

// WM_COPYDATA from a second host start to the running one: dwData is this value and
// lpData the second start's command line (UTF-16, no terminator needed).
constexpr ULONG_PTR kCopyDataCommand = 0x50544831;  // 'PTH1'

// Host exit codes. Bloom does not restart the host by itself after exit codes 10 to 15.
enum ExitCode : int {
    kExitOk = 0,
    kExitError = 1,         // bad arguments, missing files, anything unexpected
    kExitCrashes = 10,      // explorer kept crashing after we loaded: safe mode is on
    kExitNoSymbols = 11,    // a required taskbar symbol is not in this Windows build
    kExitWindhawk = 12,     // the same mod is on in Windhawk
    kExitBlocked = 13,      // code integrity (Smart App Control) kept the DLL out
    kExitDownload = 14,     // the symbol files could not be downloaded
    kExitRestart = 15,      // explorer must restart: an earlier mod or runtime image is still mapped in it
};

// Unload budgets. The runtime finishes the mod's callbacks and the wait for threads to
// leave the mod's code within kRuntimeUnloadMs of seeing Unload (then at most
// kRuntimeUnmapMs more for the mod image to go); the host waits kUnloadWaitMs for
// Unloaded, so the first two must stay below it.
constexpr DWORD kRuntimeUnloadMs = 8000;
constexpr DWORD kRuntimeUnmapMs = 1000;
constexpr DWORD kUnloadWaitMs = 10000;

// Written by the host into kConfigMapping before it hooks the taskbar thread; the
// runtime copies it at start and does not look at it again. The fields up to dataDir
// never move, so a runtime that does not match size or protocol can still report its
// refusal ("protocol") in the data folder.
constexpr DWORD kConfigLog = 1;  // Wh_Log on from the start
struct Config {
    DWORD size;      // sizeof(Config)
    DWORD protocol;  // kProtocol
    DWORD flags;     // kConfig*
    DWORD hostPid;
    WCHAR version[32];      // the host's kVersion
    WCHAR dataDir[1024];    // the addon data folder
    WCHAR modPath[1024];    // full path of the mod DLL, empty: runtime only (a test)
};

}  // namespace addon
