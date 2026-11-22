// SPDX-License-Identifier: GPL-3.0-only

#include "addon_protocol.h"
#include "commands.h"
#include "inject.h"
#include "panel.h"
#include "util.h"

namespace host {

namespace {

// Sets Unload in a runtime t's process holds while no host looks after it. True when
// there is none; false when there is one whose events cannot be opened.
bool AskOrphan(const Taskbar& t, const wchar_t* where, Explorer& e) {
    if (!RuntimeLoadedIn(t.pid)) return true;
    if (!OpenExplorer(t, e) || !OpenEvents(e, false)) return false;
    Log(L"unloading a runtime no host looks after (%ls %lu)", where, t.pid);
    AskUnload(e);
    e.runtimeIn = true;
    return true;
}

// False when an asked runtime has not unloaded by deadline (GetTickCount64).
bool WaitOrphan(const Explorer& e, ULONGLONG deadline) {
    if (!e.runtimeIn) return true;
    HANDLE hs[] = {e.unloaded.get(), e.process.get()};
    return WaitForMultipleObjects(2, hs, FALSE, MsUntil(deadline)) < WAIT_OBJECT_0 + 2;
}

}  // namespace

bool HostRunning() {
    Handle m(OpenMutexW(SYNCHRONIZE, FALSE, addon::kHostMutex));
    return (bool)m;
}

bool ForwardToHost(const std::wstring& commandLine) {
    // The first host creates its window before anything slow, so a short wait covers
    // a host that is still starting.
    HWND w = nullptr;
    for (int i = 0; i < 50 && !w; i++) {
        w = FindWindowW(addon::kHostWindowClass, nullptr);
        if (!w) Sleep(100);
    }
    if (!w) return false;
    COPYDATASTRUCT cds{addon::kCopyDataCommand, (DWORD)(commandLine.size() * sizeof(wchar_t)),
                       (void*)commandLine.data()};
    DWORD_PTR result = 0;
    return SendMessageTimeoutW(w, WM_COPYDATA, 0, (LPARAM)&cds, SMTO_ABORTIFHUNG, 5000, &result) && result;
}

bool WaitForHostExit(DWORD ms) {
    Handle m(OpenMutexW(SYNCHRONIZE, FALSE, addon::kHostMutex));
    if (!m) return true;
    DWORD r = WaitForSingleObject(m.get(), ms);
    if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED) {
        ReleaseMutex(m.get());
        return true;
    }
    return false;
}

bool UnloadOrphanRuntime() {
    // Explorer's and ShellHost's: both are asked first and then share one wait, as when
    // a host stops. ShellHost is looked at even by an add-on that no longer goes there,
    // for a runtime an earlier version of it left behind.
    Explorer ex, panel;
    Taskbar t;
    bool ok = !FindTaskbar(t) || AskOrphan(t, L"explorer", ex);
    if (FindShellHost(t)) ok = AskOrphan(t, L"ShellHost", panel) && ok;
    ULONGLONG deadline = GetTickCount64() + addon::kUnloadWaitMs;
    ok = WaitOrphan(ex, deadline) && ok;
    return WaitOrphan(panel, deadline) && ok;
}

}  // namespace host
