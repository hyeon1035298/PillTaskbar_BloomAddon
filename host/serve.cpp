// SPDX-License-Identifier: GPL-3.0-only
//
// --serve: prepare symbols, put the runtime into the taskbar's explorer, watch
// explorer and Bloom, and take the runtime out again (docs/addon-protocol.md
// section 4). --dry-run walks the same checks and stops before touching explorer.

#include <algorithm>
#include <deque>

#include "addon_protocol.h"
#include "autohide.h"
#include "commands.h"
#include "eventlog.h"
#include "inject.h"
#include "state.h"
#include "symbols.h"
#include "taskbar.h"
#include "util.h"
#include "windhawk.h"

namespace host {

namespace {

constexpr DWORD kBloomGraceMs = 10000;
constexpr DWORD kAfterCrashDelayMs = 5000;
constexpr DWORD kRuntimeStartMs = 60000;
constexpr DWORD kTaskbarPollMs = 1000;
// While the runtime is in: how often the host looks whether Windhawk can now load the
// same mod (windhawk.exe started, or its service set to start by itself).
constexpr DWORD kWindhawkPollMs = 5000;
// Returned by steps that leave the main loop running.
constexpr int kContinue = -100;

const wchar_t* ReasonFor(int code) {
    switch (code) {
        case addon::kExitCrashes: return L"crashes";
        case addon::kExitNoSymbols: return L"noSymbols";
        case addon::kExitWindhawk: return L"windhawk";
        case addon::kExitBlocked: return L"blocked";
        case addon::kExitDownload: return L"download";
        case addon::kExitRestart: return L"restart";
        default: return L"error";
    }
}

// Checks that pid is a running Bloom.exe, and the same file as --bloom when given.
bool OpenBloom(DWORD pid, const std::wstring& exe, Handle& out, std::wstring& why) {
    Handle p(OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
    if (!p || WaitForSingleObject(p.get(), 0) != WAIT_TIMEOUT) {
        why = Format(L"process %lu is not running", pid);
        return false;
    }
    std::wstring image = ProcessImagePath(p.get());
    if (!EqualsNoCase(FileName(image), L"Bloom.exe")) {
        why = Format(L"process %lu is %ls, not Bloom.exe", pid, FileName(image).c_str());
        return false;
    }
    if (!exe.empty()) {
        wchar_t full[MAX_PATH * 2];
        DWORD n = GetFullPathNameW(exe.c_str(), ARRAYSIZE(full), full, nullptr);
        if (!n || n >= ARRAYSIZE(full) || !EqualsNoCase(full, image)) {
            why = Format(L"process %lu is a different Bloom.exe than --bloom", pid);
            return false;
        }
    }
    out = std::move(p);
    return true;
}

std::wstring FullPath(const std::wstring& p) {
    wchar_t buf[MAX_PATH * 2];
    DWORD n = GetFullPathNameW(p.c_str(), ARRAYSIZE(buf), buf, nullptr);
    return n && n < ARRAYSIZE(buf) ? std::wstring(buf, n) : p;
}

// Lets a progress callback act at most every ms milliseconds.
struct Throttle {
    ULONGLONG last = 0;
    bool Due(ULONGLONG ms) {
        ULONGLONG now = GetTickCount64();
        if (last && now - last < ms) return false;
        last = now;
        return true;
    }
};

class Host {
public:
    explicit Host(const Args& a) : args_(a) {
        exeDir_ = ExeDir();
        dataDir_ = FullPath(DataDirFor(a.bloomLocal));
    }

    int Run();
    int DryRun();

    // Window procedure side.
    void OnCommand(std::wstring line) { commands_.push_back(std::move(line)); }
    void OnTaskbarCreated() { taskbarCreated_ = true; }
    UINT taskbarCreatedMsg = 0;

private:
    int AcquireInstance();
    bool CreateHostWindow();
    void ProcessCommands();
    bool StopPending();
    int Wait(const std::vector<HANDLE>& handles, DWORD ms, bool honorStop);
    WaitFn Waiter() {
        return [this](const std::vector<HANDLE>& h, DWORD ms, bool stop) { return Wait(h, ms, stop); };
    }

    int Finish(int code, const std::wstring& reason = {});
    // code: the exit code when nothing else decides it (safe mode asked wins).
    int Shutdown(int code = addon::kExitOk);
    void CountLeftoverLoaded();
    int Symbols(const std::vector<std::wstring>& explorerModules, bool forDryRun);
    int StartExplorer(const Taskbar& t);
    int Inject();
    int WaitRuntimeReady();
    int OnExplorerExit();
    int RecheckPendingCrash();
    bool TakeOverBloom(DWORD pid, const std::wstring& exe);
    addon::Config MakeConfig() const;
    std::wstring RuntimePath() const { return JoinPath(exeDir_, addon::kRuntimeDll); }
    std::wstring ModPath() const { return args_.mod.empty() ? std::wstring() : JoinPath(exeDir_, args_.mod); }

    Args args_;
    std::wstring exeDir_, dataDir_;
    StateFile state_;
    Handle mutex_, stopEvent_, bloom_;
    HWND window_ = nullptr;
    std::deque<std::wstring> commands_;
    bool stopAsked_ = false, safeModeAsked_ = false;
    bool taskbarCreated_ = false;
    bool wakeOnTaskbar_ = false;      // only the main loop's wait returns on TaskbarCreated

    Explorer ex_;
    bool haveExplorer_ = false;
    bool delayNext_ = false;          // the last explorer crashed: wait after the new taskbar
    ULONGLONG injectNotBefore_ = 0;
    ULONGLONG graceStart_ = 0;        // Bloom went away at this tick; 0 when Bloom runs
    ULONGLONG windhawkCheckedAt_ = 0;  // the last Windhawk look while the runtime is in
    struct Pending {
        DWORD pid = 0;
        uint64_t created = 0, injectedAt = 0;
    } pendingCrash_;                  // an exit whose crash event may still be on its way
};

Host* g_host = nullptr;

LRESULT CALLBACK HostWndProc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_COPYDATA && g_host) {
        auto* cds = (const COPYDATASTRUCT*)lp;
        if (cds && cds->dwData == addon::kCopyDataCommand && cds->lpData && cds->cbData % sizeof(wchar_t) == 0 &&
            cds->cbData <= 64 * 1024) {
            g_host->OnCommand(std::wstring((const wchar_t*)cds->lpData, cds->cbData / sizeof(wchar_t)));
            return TRUE;
        }
        return FALSE;
    }
    // TaskbarCreated only wakes the main loop; it looks for Shell_TrayWnd itself.
    if (g_host && msg && msg == g_host->taskbarCreatedMsg) {
        g_host->OnTaskbarCreated();
        return 0;
    }
    return DefWindowProcW(w, msg, wp, lp);
}

bool Host::CreateHostWindow() {
    taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HostWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = addon::kHostWindowClass;
    if (!RegisterClassExW(&wc)) return false;
    // Top level, never shown: a message-only window would not get the TaskbarCreated
    // broadcast.
    window_ = CreateWindowExW(WS_EX_TOOLWINDOW, addon::kHostWindowClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr,
                              nullptr, wc.hInstance, nullptr);
    return window_ != nullptr;
}

int Host::AcquireInstance() {
    mutex_.Reset(CreateMutexW(nullptr, TRUE, addon::kHostMutex));
    if (!mutex_) return addon::kExitError;
    if (GetLastError() != ERROR_ALREADY_EXISTS) return kContinue;

    StateFile other;
    other.Init(dataDir_, true);
    other.Load();
    const std::wstring& running = other.state().version;
    if (running.empty() || running == addon::kVersion) {
        bool sent = ForwardToHost(GetCommandLineW());
        Log(sent ? L"a host is running; handed it this command line" : L"a host is running but did not answer");
        return sent ? addon::kExitOk : addon::kExitError;
    }
    // An add-on update: the old host steps down and this one takes over.
    Log(L"host %ls is running; asking it to stop", running.c_str());
    ForwardToHost(L"--stop");
    DWORD r = WaitForSingleObject(mutex_.get(), 15000);
    if (r != WAIT_OBJECT_0 && r != WAIT_ABANDONED) {
        Log(L"the old host did not stop");
        return addon::kExitError;
    }
    return kContinue;
}

void Host::ProcessCommands() {
    while (!commands_.empty()) {
        std::wstring line = std::move(commands_.front());
        commands_.pop_front();
        Args a;
        if (!ParseArgs(SplitCommandLine(line), a)) {
            Log(L"ignored a forwarded command: %ls", a.error.c_str());
            continue;
        }
        switch (a.command) {
            case Command::Stop:
                Log(L"stop asked for");
                stopAsked_ = true;
                break;
            case Command::SafeMode:
                Log(L"safe mode asked for");
                stopAsked_ = safeModeAsked_ = true;
                break;
            case Command::Serve:
                TakeOverBloom(a.bloomPid, a.bloomExe);
                break;
            default: Log(L"ignored a forwarded command"); break;
        }
    }
}

bool Host::TakeOverBloom(DWORD pid, const std::wstring& exe) {
    Handle h;
    std::wstring why;
    if (!OpenBloom(pid, exe, h, why)) {
        Log(L"forwarded --serve ignored: %ls", why.c_str());
        return false;
    }
    bloom_ = std::move(h);
    args_.bloomPid = pid;
    graceStart_ = 0;
    Log(L"now serving Bloom %lu", pid);
    return true;
}

bool Host::StopPending() {
    return stopAsked_ || (stopEvent_ && WaitForSingleObject(stopEvent_.get(), 0) == WAIT_OBJECT_0);
}

int Host::Wait(const std::vector<HANDLE>& handles, DWORD ms, bool honorStop) {
    ULONGLONG start = GetTickCount64();
    std::vector<HANDLE> all = handles;
    if (honorStop && stopEvent_) all.push_back(stopEvent_.get());
    while (true) {
        ProcessCommands();
        if (honorStop && StopPending()) return kWaitStop;
        DWORD left = INFINITE;
        if (ms != INFINITE) {
            ULONGLONG spent = GetTickCount64() - start;
            left = spent >= ms ? 0 : (DWORD)(ms - spent);
        }
        DWORD r = MsgWaitForMultipleObjects((DWORD)all.size(), all.data(), FALSE, left, QS_ALLINPUT);
        if (r < WAIT_OBJECT_0 + handles.size()) return (int)(r - WAIT_OBJECT_0);
        if (r == WAIT_OBJECT_0 + all.size()) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (wakeOnTaskbar_ && taskbarCreated_) {
                taskbarCreated_ = false;
                return kWaitTimeout;
            }
            continue;
        }
        if (r < WAIT_OBJECT_0 + all.size()) continue;  // the stop event: handled above
        if (r == WAIT_TIMEOUT) return kWaitTimeout;
        Sleep(50);  // WAIT_FAILED; should not happen, but never spin
        if (left == 0) return kWaitTimeout;
    }
}

addon::Config Host::MakeConfig() const {
    addon::Config c{};
    c.size = sizeof(c);
    c.protocol = addon::kProtocol;
    c.flags = args_.log ? addon::kConfigLog : 0;
    c.hostPid = GetCurrentProcessId();
    wcsncpy(c.version, addon::kVersion, ARRAYSIZE(c.version) - 1);
    wcsncpy(c.dataDir, dataDir_.c_str(), ARRAYSIZE(c.dataDir) - 1);
    wcsncpy(c.modPath, ModPath().c_str(), ARRAYSIZE(c.modPath) - 1);
    return c;
}

int Host::Finish(int code, const std::wstring& reason) {
    HostState& s = state_.state();
    if (code == addon::kExitCrashes) s.safeMode = true;
    std::wstring why = reason.empty() && code != addon::kExitOk ? std::wstring(ReasonFor(code)) : reason;
    bool failed = code != addon::kExitOk && !(code == addon::kExitCrashes && safeModeAsked_);
    state_.SetPhase(failed ? L"failed" : L"stopped", why);
    Log(L"exit %d%ls%ls", code, why.empty() ? L"" : L", ", why.c_str());
    return code;
}

// Takes the runtime out (if one is in) and ends the host.
int Host::Shutdown(int code) {
    state_.SetPhase(L"stopping");
    std::wstring reason;
    // A stop that cut the hook wait short can race a runtime that was just arriving.
    if (haveExplorer_ && !ex_.runtimeIn && ex_.injectedAt && ex_.loaded &&
        Wait({ex_.loaded.get()}, 500, false) == 0)
        ex_.runtimeIn = true;
    if (!ex_.runtimeIn) state_.state().loaded.reset();
    if (haveExplorer_ && ex_.runtimeIn) {
        if (UnloadRuntime(ex_, addon::kUnloadWaitMs, Waiter())) {
            Log(L"runtime unloaded");
            state_.state().loaded.reset();
        } else {
            // Explorer is left alone; the runtime goes with the next explorer restart.
            Log(L"runtime did not unload within %lu ms", addon::kUnloadWaitMs);
            reason = L"unloadTimeout";
        }
    }
    if (safeModeAsked_) return Finish(addon::kExitCrashes, reason);
    return Finish(code, reason);
}

// A host that died with explorer leaves "loaded" behind; count that crash now.
void Host::CountLeftoverLoaded() {
    HostState& s = state_.state();
    if (!s.loaded) return;
    LoadedRecord l = *s.loaded;
    if (ProcessAlive(l.pid, l.created)) return;  // still there: the attach path decides
    uint64_t when = 0;
    if (FindAppCrashEvent(l.pid, l.created, l.at, l.at + kCrashAfterInjectWindow, when) && CrashCounts(l.at, when)) {
        Log(L"explorer %lu crashed after the last injection (found in the event log)", l.pid);
        if (RecordCrash(s, when, NowTicks())) s.safeMode = true;
    }
    s.loaded.reset();
    state_.Save();
}

int Host::Symbols(const std::vector<std::wstring>& explorerModules, bool forDryRun) {
    SymbolOptions o;
    o.dataDir = dataDir_;
    o.symbolList = JoinPath(exeDir_, addon::kSymbolList);
    o.modules = AddonModules(explorerModules);
    o.noDownload = args_.noDownload;
    o.noWindhawkCache = args_.noWindhawkCache;
    Throttle t;
    bool phaseSet = false;
    o.progress = [&](uint64_t done, uint64_t total) {
        if (!forDryRun) {
            HostState& s = state_.state();
            s.done = done;
            s.total = total;
            // At most four writes a second; Bloom only draws a bar from it.
            if (!phaseSet || t.Due(250) || done == total) {
                phaseSet = true;
                s.phase = L"symbols";
                state_.Save();
            }
        } else if (t.Due(1000)) {
            Print(Format(L"  downloading: %llu / %llu bytes", done, total));
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) DispatchMessageW(&msg);
        ProcessCommands();
        return forDryRun || !StopPending();
    };
    return PrepareSymbols(o);
}

int Host::StartExplorer(const Taskbar& t) {
    if (!OpenExplorer(t, ex_)) {
        Log(L"could not open explorer %lu (%lu)", t.pid, GetLastError());
        return kContinue;
    }
    std::vector<std::wstring> mods = ProcessModules(t.pid);
    WindhawkConflict wh = CheckWindhawkRegistry();
    CheckWindhawkModules(mods, wh);
    if (wh.any()) {
        Log(L"Windhawk: %ls", wh.detail.c_str());
        return addon::kExitWindhawk;
    }
    int r = Symbols(mods, false);
    if (r == kSymbolsStopped) return kContinue;  // the main loop sees the stop
    if (r != addon::kExitOk) return r;

    if (RuntimeLoadedIn(t.pid)) {
        RuntimeState rs;
        bool known = state_.ReadRuntime(rs) && rs.pid == t.pid;
        bool live = known && (rs.state == L"running" || rs.state == L"starting" || rs.state == L"reloading" ||
                              rs.state == L"off");
        if (live && rs.version == addon::kVersion && OpenEvents(ex_, false)) {
            HostState& s = state_.state();
            uint64_t at = s.loaded && s.loaded->pid == t.pid && s.loaded->created == ex_.created ? s.loaded->at
                                                                                                  : NowTicks();
            s.loaded = LoadedRecord{t.pid, ex_.created, at};
            ex_.injectedAt = at;
            ex_.runtimeIn = true;
            Log(L"runtime %ls already in explorer %lu (%ls); taking it over", rs.version.c_str(), t.pid,
                rs.state.c_str());
            haveExplorer_ = true;
            if (rs.state == L"starting") return WaitRuntimeReady();
            state_.SetPhase(L"running");
            KeepAutoHideForIsland(dataDir_);
            return kContinue;
        }
        if (!known || rs.state != L"stopped") {
            Log(L"explorer %lu has runtime %ls; unloading it first", t.pid,
                known ? rs.version.c_str() : L"(unknown)");
            if (!OpenEvents(ex_, false) || !UnloadRuntime(ex_, addon::kUnloadWaitMs, Waiter())) {
                Log(L"the old runtime did not unload");
                return Finish(addon::kExitError, L"unloadTimeout");
            }
        }
    }
    haveExplorer_ = true;
    return Inject();
}

int Host::Inject() {
    HostState& s = state_.state();
    // Written before anything reaches explorer, so a crash that also takes the host
    // down is still counted at the next start.
    ex_.injectedAt = NowTicks();
    s.loaded = LoadedRecord{ex_.taskbar.pid, ex_.created, ex_.injectedAt};
    state_.SetPhase(L"injecting");
    Log(L"injecting into explorer %lu, thread %lu", ex_.taskbar.pid, ex_.taskbar.tid);

    if (!CreateConfig(ex_, MakeConfig()) || !OpenEvents(ex_, true)) {
        Log(L"could not create the shared objects (%lu)", GetLastError());
        s.loaded.reset();
        return addon::kExitError;
    }
    DWORD err = 0;
    HookResult hr = HookUntilLoaded(ex_, RuntimePath(), Waiter(), err);
    switch (hr) {
        case HookResult::Loaded: break;
        case HookResult::Stopped:
        case HookResult::ExplorerGone: return kContinue;  // the main loop handles both
        case HookResult::HookFailed:
        case HookResult::NotLoaded: {
            s.loaded.reset();
            ex_.mapping.Reset();
            std::wstring which;
            if (FindCodeIntegrityBlock({addon::kRuntimeDll, addon::kModDll, FileName(ModPath())}, which)) {
                Log(L"code integrity blocked %ls", which.c_str());
                return addon::kExitBlocked;
            }
            if (hr == HookResult::HookFailed)
                Log(L"could not hook the taskbar thread (%lu)", err);
            else
                Log(L"the runtime did not report Loaded within 10 s");
            return addon::kExitError;
        }
    }
    Log(L"runtime loaded");
    return WaitRuntimeReady();
}

int Host::WaitRuntimeReady() {
    ULONGLONG start = GetTickCount64();
    DWORD pid = ex_.taskbar.pid;
    std::wstring last;
    while (GetTickCount64() - start < kRuntimeStartMs) {
        RuntimeState rs;
        // A file older than this injection is from an earlier runtime in the same
        // explorer; the seconds-only timestamps get two seconds of slack.
        if (state_.ReadRuntime(rs) && rs.pid == pid && rs.at + 2 * kTicksPerSecond >= ex_.injectedAt) {
            if (rs.state != last) Log(L"runtime state: %ls%ls%ls", rs.state.c_str(), rs.reason.empty() ? L"" : L" ",
                                      rs.reason.c_str());
            last = rs.state;
            if (rs.state == L"running" || rs.state == L"off" || rs.state == L"reloading") {
                ex_.mapping.Reset();
                state_.SetPhase(L"running");
                KeepAutoHideForIsland(dataDir_);
                return kContinue;
            }
            if (rs.state == L"initFailed") {
                UnloadRuntime(ex_, addon::kUnloadWaitMs, Waiter());
                if (!ex_.runtimeIn) state_.state().loaded.reset();
                return rs.reason == L"symbols" ? addon::kExitNoSymbols : addon::kExitError;
            }
            if (rs.state == L"refused") {
                // A refusing runtime keeps Loaded and its reason until it is asked to go.
                UnloadRuntime(ex_, addon::kUnloadWaitMs, Waiter());
                if (!ex_.runtimeIn) state_.state().loaded.reset();
                if (rs.reason == L"windhawk") return addon::kExitWindhawk;
                if (rs.reason == L"restart") return addon::kExitRestart;
                return addon::kExitError;
            }
            if (rs.state == L"stopped") {
                ex_.runtimeIn = false;
                state_.state().loaded.reset();
                return addon::kExitError;
            }
        }
        int r = Wait({ex_.process.get()}, 250, true);
        if (r == 0 || r == kWaitStop) return kContinue;  // the main loop handles both
    }
    Log(L"the runtime did not leave starting within %lu s", kRuntimeStartMs / 1000);
    UnloadRuntime(ex_, addon::kUnloadWaitMs, Waiter());
    if (!ex_.runtimeIn) state_.state().loaded.reset();
    return addon::kExitError;
}

int Host::OnExplorerExit() {
    DWORD code = 0;
    GetExitCodeProcess(ex_.process.get(), &code);
    FILETIME created, exited, kernel, user;
    uint64_t diedAt = GetProcessTimes(ex_.process.get(), &created, &exited, &kernel, &user) ? ToTicks(exited)
                                                                                             : NowTicks();
    HostState& s = state_.state();
    bool ours = ex_.injectedAt != 0;
    bool crash = IsCrashExitCode(code);
    uint64_t when = diedAt;
    if (!crash && ours) crash = FindAppCrashEvent(ex_.taskbar.pid, ex_.created, ex_.injectedAt, NowTicks(), when);
    Log(L"explorer %lu exited with 0x%08lX%ls", ex_.taskbar.pid, code, crash ? L" (crash)" : L"");
    s.loaded.reset();
    pendingCrash_ = {};
    bool counts = ours && crash && CrashCounts(ex_.injectedAt, diedAt);
    if (ours && !crash && diedAt - ex_.injectedAt <= kCrashAfterInjectWindow)
        pendingCrash_ = {ex_.taskbar.pid, ex_.created, ex_.injectedAt};
    ex_ = Explorer{};
    haveExplorer_ = false;
    delayNext_ = false;
    injectNotBefore_ = 0;
    if (counts) {
        if (RecordCrash(s, diedAt, NowTicks())) {
            Log(L"explorer crashed twice within 10 minutes; safe mode on");
            return Finish(addon::kExitCrashes);
        }
        delayNext_ = true;
    }
    state_.SetPhase(L"waiting");
    return kContinue;
}

// Application Error events can land a little after the process is gone, so an exit
// that looked clean is looked up once more before the next injection.
int Host::RecheckPendingCrash() {
    Pending p = pendingCrash_;
    pendingCrash_ = {};
    if (!p.pid) return kContinue;
    uint64_t when = 0;
    if (!FindAppCrashEvent(p.pid, p.created, p.injectedAt, NowTicks(), when) || !CrashCounts(p.injectedAt, when))
        return kContinue;
    Log(L"explorer %lu did crash (event log)", p.pid);
    if (RecordCrash(state_.state(), when, NowTicks())) {
        Log(L"explorer crashed twice within 10 minutes; safe mode on");
        return Finish(addon::kExitCrashes);
    }
    state_.Save();
    delayNext_ = true;
    return kContinue;
}

int Host::Run() {
    int r = AcquireInstance();
    if (r != kContinue) return r;
    stopEvent_.Reset(CreateEventW(nullptr, TRUE, FALSE, addon::kHostStopEvent));
    // A Stop left set by an earlier session would end this host at once.
    if (stopEvent_) ResetEvent(stopEvent_.get());
    g_host = this;
    if (!CreateHostWindow()) {
        Log(L"could not create the host window (%lu)", GetLastError());
        return addon::kExitError;
    }

    state_.Init(dataDir_, false);
    state_.Load();
    HostState& s = state_.state();
    s.reason.clear();
    if (args_.resetSafeMode) {
        s.safeMode = false;
        s.crashes.clear();
        Log(L"safe mode and crash list cleared");
    }
    CountLeftoverLoaded();
    if (s.safeMode) {
        Log(L"safe mode is on; not injecting");
        return Finish(addon::kExitCrashes);
    }
    state_.SetPhase(L"starting");

    std::wstring why;
    if (!OpenBloom(args_.bloomPid, args_.bloomExe, bloom_, why)) {
        Log(L"%ls", why.c_str());
        return Finish(addon::kExitError);
    }
    if (!FileExists(RuntimePath()) || (!args_.mod.empty() && !FileExists(ModPath()))) {
        Log(L"%ls or the mod DLL is missing next to the host", addon::kRuntimeDll);
        return Finish(addon::kExitError);
    }
    WindhawkConflict wh = CheckWindhawkRegistry();
    if (wh.any()) {
        Log(L"Windhawk: %ls", wh.detail.c_str());
        return Finish(addon::kExitWindhawk);
    }
    state_.SetPhase(L"waiting");

    while (true) {
        ProcessCommands();
        if (StopPending()) return Shutdown();
        ULONGLONG now = GetTickCount64();
        if (graceStart_ && now - graceStart_ >= kBloomGraceMs) {
            Log(L"Bloom did not come back; stopping");
            return Shutdown();
        }
        Taskbar t;
        if (!haveExplorer_ && FindTaskbar(t)) {
            if (delayNext_ && !injectNotBefore_) {
                injectNotBefore_ = now + kAfterCrashDelayMs;
                Log(L"new taskbar after a crash; waiting %lu ms", kAfterCrashDelayMs);
            }
            if (now >= injectNotBefore_) {
                r = RecheckPendingCrash();
                if (r != kContinue) return r;
                if (!delayNext_ || injectNotBefore_) {
                    delayNext_ = false;
                    injectNotBefore_ = 0;
                    r = StartExplorer(t);
                    if (r != kContinue) {
                        // Leave nothing behind when giving up with a runtime inside.
                        if (haveExplorer_ && ex_.runtimeIn) UnloadRuntime(ex_, addon::kUnloadWaitMs, Waiter());
                        if (!ex_.runtimeIn) state_.state().loaded.reset();
                        return state_.state().phase == L"failed" ? r : Finish(r);
                    }
                    // On to the wait: it watches the new explorer, or polls again in a
                    // second when explorer could not be opened.
                }
            }
        }

        std::vector<HANDLE> hs;
        int bloomIdx = -1, explorerIdx = -1;
        if (bloom_ && !graceStart_) {
            bloomIdx = (int)hs.size();
            hs.push_back(bloom_.get());
        }
        if (haveExplorer_) {
            explorerIdx = (int)hs.size();
            hs.push_back(ex_.process.get());
        }
        DWORD timeout = haveExplorer_ ? (ex_.runtimeIn ? kWindhawkPollMs : INFINITE) : kTaskbarPollMs;
        if (graceStart_) timeout = std::min<DWORD>(timeout, (DWORD)(kBloomGraceMs - std::min<ULONGLONG>(now - graceStart_, kBloomGraceMs)));
        if (injectNotBefore_ > now) timeout = std::min<DWORD>(timeout, (DWORD)(injectNotBefore_ - now));
        wakeOnTaskbar_ = !haveExplorer_;
        r = Wait(hs, timeout, true);
        wakeOnTaskbar_ = false;
        if (r >= 0 && r == bloomIdx) {
            Log(L"Bloom %lu exited; waiting %lu ms for a new one", args_.bloomPid, kBloomGraceMs);
            bloom_.Reset();
            graceStart_ = GetTickCount64();
        } else if (r >= 0 && r == explorerIdx) {
            r = OnExplorerExit();
            if (r != kContinue) return r;
        }
        // Windhawk started after the load (or its service was set to start by itself)
        // with the same mod on: take the runtime out before both hook the same code.
        if (haveExplorer_ && ex_.runtimeIn && GetTickCount64() - windhawkCheckedAt_ >= kWindhawkPollMs) {
            windhawkCheckedAt_ = GetTickCount64();
            WindhawkConflict wh = CheckWindhawkRegistry();
            if (wh.any()) {
                Log(L"Windhawk: %ls; unloading", wh.detail.c_str());
                return Shutdown(addon::kExitWindhawk);
            }
        }
    }
}

int Host::DryRun() {
    Print(Format(L"dry run: " ADDON_NAME L"Host %ls, protocol %lu", addon::kVersion, addon::kProtocol));
    Print(L"host running: " + std::wstring(HostRunning() ? L"yes (a real start would hand it the command line)"
                                                         : L"no"));
    state_.Init(dataDir_, true);
    state_.Load();
    HostState& s = state_.state();
    Print(Format(L"host-state: safeMode=%ls crashes=%zu loaded=%ls%ls", s.safeMode ? L"true" : L"false",
                 s.crashes.size(), s.loaded ? Format(L"pid %lu", s.loaded->pid).c_str() : L"null",
                 args_.resetSafeMode ? L" (--reset-safe-mode would clear safeMode and crashes)" : L""));
    if (s.loaded && !ProcessAlive(s.loaded->pid, s.loaded->created)) {
        uint64_t when;
        bool crash = FindAppCrashEvent(s.loaded->pid, s.loaded->created, s.loaded->at,
                                       s.loaded->at + kCrashAfterInjectWindow, when);
        Print(Format(L"  leftover loaded: explorer gone, crash event %ls", crash ? L"found" : L"not found"));
    }

    Handle bloom;
    std::wstring why;
    Print(OpenBloom(args_.bloomPid, args_.bloomExe, bloom, why) ? Format(L"Bloom %lu: ok (Bloom.exe)", args_.bloomPid)
                                                                : L"Bloom check failed: " + why);
    Print(Format(L"files: %ls %ls, mod %ls %ls, %ls %ls", addon::kRuntimeDll,
                 FileExists(RuntimePath()) ? L"present" : L"MISSING",
                 args_.mod.empty() ? L"(none)" : args_.mod.c_str(),
                 args_.mod.empty() ? L"" : (FileExists(ModPath()) ? L"present" : L"MISSING"), addon::kSymbolList,
                 FileExists(JoinPath(exeDir_, addon::kSymbolList)) ? L"present" : L"MISSING"));

    WindhawkConflict wh = CheckWindhawkRegistry();
    Taskbar t;
    if (!FindTaskbar(t)) {
        Print(L"Windhawk: " + wh.detail);
        Print(L"taskbar: Shell_TrayWnd not found; a real start would wait for TaskbarCreated");
        return addon::kExitOk;
    }
    Print(Format(L"taskbar: Shell_TrayWnd %p, explorer pid %lu, thread %lu", (void*)t.tray, t.pid, t.tid));
    std::vector<std::wstring> mods = ProcessModules(t.pid);
    CheckWindhawkModules(mods, wh);
    Print(Format(L"explorer modules: %zu", mods.size()));
    Print(Format(L"Windhawk: %ls -> %ls", wh.detail.c_str(), wh.any() ? L"CONFLICT, a real start exits 12"
                                                                         : L"no conflict"));
    for (auto& m : AddonModules(mods)) Print(L"module " + m.name + L": " + m.path);

    ULONGLONG t0 = GetTickCount64();
    int r = Symbols(mods, true);
    Print(Format(L"symbols: exit %d in %llu ms", r, GetTickCount64() - t0));

    bool loaded = RuntimeLoadedIn(t.pid);
    RuntimeState rs;
    bool known = state_.ReadRuntime(rs);
    Print(Format(L"existing runtime: Loaded event %ls; runtime-state.json %ls", loaded ? L"set" : L"not set",
                 known ? Format(L"pid %lu version %ls state %ls", rs.pid, rs.version.c_str(), rs.state.c_str()).c_str()
                       : L"absent"));

    if (s.safeMode && !args_.resetSafeMode) {
        Print(L"would do: exit 10 (safe mode is on)");
    } else if (wh.any()) {
        Print(L"would do: exit 12 without injecting");
    } else if (r != addon::kExitOk) {
        Print(Format(L"would do: exit %d (symbols)", r));
    } else if (loaded && known && rs.pid == t.pid && rs.version == addon::kVersion) {
        Print(L"would do: take over the running runtime");
    } else {
        Print(Format(L"would do: %lswrite loaded, create %ls and the Loaded/Unload/Unloaded events, hook thread %lu "
                     L"with %ls, post WM_NULL until Loaded",
                     loaded ? L"unload the old runtime, " : L"",
                     ObjectName(addon::kConfigMapping, t.pid).c_str(), t.tid, addon::kRuntimeDll));
    }
    return addon::kExitOk;
}

}  // namespace

int RunServe(const Args& a) {
    std::wstring dataDir = FullPath(DataDirFor(a.bloomLocal));
    if (a.dryRun) EnablePrint();
    if (!CreateDirs(dataDir)) {
        Print(L"could not create the data folder");
        return addon::kExitError;
    }
    OpenLog(dataDir);
    Log(L"start: --serve%ls, version %ls", a.dryRun ? L" --dry-run" : L"", addon::kVersion);
    Host h(a);
    return a.dryRun ? h.DryRun() : h.Run();
}

}  // namespace host
