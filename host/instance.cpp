// SPDX-License-Identifier: GPL-3.0-only

#include "addon_protocol.h"
#include "commands.h"
#include "inject.h"
#include "util.h"

namespace host {

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
    Taskbar t;
    if (!FindTaskbar(t) || !RuntimeLoadedIn(t.pid)) return true;
    Explorer e;
    if (!OpenExplorer(t, e) || !OpenEvents(e, false)) return false;
    Log(L"unloading a runtime no host looks after (explorer %lu)", t.pid);
    SetEvent(e.unload.get());
    HANDLE hs[] = {e.unloaded.get(), e.process.get()};
    return WaitForMultipleObjects(2, hs, FALSE, addon::kUnloadWaitMs) < WAIT_OBJECT_0 + 2;
}

}  // namespace host
