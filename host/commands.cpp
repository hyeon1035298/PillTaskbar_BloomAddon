// SPDX-License-Identifier: GPL-3.0-only
//
// The short commands: --stop, --safe-mode and --resolve-symbols.

#include "addon_protocol.h"
#include "commands.h"
#include "state.h"
#include "symbols.h"
#include "taskbar.h"
#include "util.h"

namespace host {

namespace {

// The host needs up to 10 s to unload, so allow a little more.
constexpr DWORD kHostExitWaitMs = 15000;

void StopRunningHost(const wchar_t* command) {
    if (!ForwardToHost(command)) Log(L"the host window did not answer; setting the Stop event");
    // The event works even when the window message did not get through.
    Handle stop(OpenEventW(EVENT_MODIFY_STATE, FALSE, addon::kHostStopEvent));
    if (stop) SetEvent(stop.get());
    Log(WaitForHostExit(kHostExitWaitMs) ? L"the host has stopped" : L"the host is still running");
}

}  // namespace

int RunStop(const Args&) {
    if (HostRunning()) {
        StopRunningHost(L"--stop");
        return addon::kExitOk;
    }
    Log(L"no host running");
    return UnloadOrphanRuntime() ? addon::kExitOk : addon::kExitError;
}

int RunSafeMode(const Args& a) {
    if (HostRunning()) {
        StopRunningHost(L"--safe-mode");
    } else {
        Log(L"no host running");
        UnloadOrphanRuntime();
    }
    // Whatever the host managed, the file ends up saying safe mode.
    std::wstring dataDir = DataDirFor(a.bloomLocal.empty() ? DefaultBloomLocal() : a.bloomLocal);
    if (!CreateDirs(dataDir)) return addon::kExitError;
    StateFile f;
    f.Init(dataDir, false);
    f.Load();
    f.state().safeMode = true;
    if (f.state().phase != L"failed") f.state().phase = L"stopped";
    bool ok = f.Save();
    Log(ok ? L"safe mode on" : L"could not write host-state.json");
    return ok ? addon::kExitOk : addon::kExitError;
}

int RunResolveSymbols(const Args& a) {
    std::wstring dataDir = DataDirFor(a.bloomLocal);
    if (!CreateDirs(dataDir)) {
        Print(L"could not create the data folder");
        return addon::kExitError;
    }
    OpenLog(dataDir);
    Log(L"start: --resolve-symbols%ls%ls%ls%ls", a.noDownload ? L" --no-download" : L"",
        a.noWindhawkCache ? L" --no-windhawk-cache" : L"", a.simulate404.empty() ? L"" : L" --simulate-404 ",
        a.simulate404.c_str());
    // A running host owns host-state.json; only report progress there when none is.
    StateFile state;
    state.Init(dataDir, HostRunning());
    state.Load();

    std::vector<std::wstring> mods;
    Taskbar t;
    if (FindTaskbar(t)) mods = ProcessModules(t.pid);
    SymbolOptions o;
    o.dataDir = dataDir;
    o.symbolList = JoinPath(ExeDir(), addon::kSymbolList);
    o.modules = AddonModules(mods);
    o.noDownload = a.noDownload;
    o.noWindhawkCache = a.noWindhawkCache;
    o.dumpDir = a.dumpDir;
    o.simulate404 = a.simulate404;
    ULONGLONG lastWrite = 0, lastPrint = 0;
    o.progress = [&](uint64_t done, uint64_t total) {
        ULONGLONG now = GetTickCount64();
        if (now - lastWrite >= 250 || done == total) {
            lastWrite = now;
            state.state().phase = L"symbols";
            state.state().done = done;
            state.state().total = total;
            state.Save();
        }
        if (now - lastPrint >= 2000) {
            lastPrint = now;
            Print(Format(L"  %llu / %llu bytes", done, total));
        }
        return true;
    };
    ULONGLONG t0 = GetTickCount64();
    SymbolReport rep;
    int r = PrepareSymbols(o, &rep);
    Log(L"symbols: exit %d in %llu ms", r, GetTickCount64() - t0);
    for (auto& m : rep.modules)
        Print(Format(L"%ls: %ls", m.module.c_str(),
                     m.exit != addon::kExitOk ? Format(L"exit %d", m.exit).c_str()
                     : m.ported               ? (L"ported from " + m.portedFrom).c_str()
                                              : L"ok"));
    std::wstring reason = r == addon::kExitOk ? L"" : !rep.reason.empty() ? rep.reason : ExitReason(r);
    Print(Format(L"symbols: exit %d%ls%ls", r, reason.empty() ? L"" : L", ", reason.c_str()));
    // Not a running host: no explorer uses these offsets yet, so the guard stays as it is.
    state.state().symbolsSource = r == addon::kExitOk ? (rep.PortedKeys().empty() ? L"pdb" : L"ported") : L"";
    state.state().portedFrom = r == addon::kExitOk ? rep.PortedFrom() : L"";
    state.SetPhase(r == addon::kExitOk ? L"stopped" : L"failed", reason);
    return r;
}

}  // namespace host
