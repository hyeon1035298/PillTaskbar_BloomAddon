// SPDX-License-Identifier: GPL-3.0-only

#include "taskbar.h"

#include <psapi.h>

#include <algorithm>

#include "util.h"

namespace host {

bool FindTaskbar(Taskbar& out) {
    // Other explorer.exe processes (separate folder windows) never own Shell_TrayWnd,
    // so the window picks the right one.
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) return false;
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(tray, &pid);
    if (!tid || !pid) return false;
    out = {tray, pid, tid};
    return true;
}

bool ExplorerReady(bool hasIsland, uint64_t ageMs) {
    return ageMs >= kExplorerReadyMaxMs || (hasIsland && ageMs >= kExplorerSettleMs);
}

bool TaskbarHasIsland(HWND tray) {
    return FindWindowExW(tray, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr) != nullptr;
}

uint64_t ProcessAgeMs(DWORD pid) {
    uint64_t created = ProcessCreationTime(pid), now = NowTicks();
    if (!created) return UINT64_MAX;
    return now > created ? (now - created) / kTicksPerMs : 0;
}

std::vector<std::wstring> ProcessModules(DWORD pid) {
    std::vector<std::wstring> out;
    Handle p(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid));
    if (!p) return out;
    std::vector<HMODULE> mods(512);
    DWORD needed = 0;
    for (int tries = 0; tries < 4; tries++) {
        if (!EnumProcessModulesEx(p.get(), mods.data(), (DWORD)(mods.size() * sizeof(HMODULE)), &needed,
                                  LIST_MODULES_ALL))
            return out;
        if (needed <= mods.size() * sizeof(HMODULE)) break;
        mods.resize(needed / sizeof(HMODULE) + 64);
    }
    mods.resize(std::min<size_t>(mods.size(), needed / sizeof(HMODULE)));
    wchar_t path[MAX_PATH * 2];
    for (HMODULE m : mods) {
        DWORD n = GetModuleFileNameExW(p.get(), m, path, ARRAYSIZE(path));
        if (n) out.emplace_back(path, n);
    }
    return out;
}

uint64_t ProcessCreationTime(HANDLE process) {
    FILETIME created, exited, kernel, user;
    if (!GetProcessTimes(process, &created, &exited, &kernel, &user)) return 0;
    return ToTicks(created);
}

uint64_t ProcessCreationTime(DWORD pid) {
    Handle p(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
    return p ? ProcessCreationTime(p.get()) : 0;
}

std::wstring ProcessImagePath(HANDLE process) {
    wchar_t buf[MAX_PATH * 2];
    DWORD n = ARRAYSIZE(buf);
    if (!QueryFullProcessImageNameW(process, 0, buf, &n)) return {};
    return std::wstring(buf, n);
}

bool ProcessAlive(DWORD pid, uint64_t created) {
    Handle p(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, pid));
    if (!p || WaitForSingleObject(p.get(), 0) != WAIT_TIMEOUT) return false;
    return created == 0 || ProcessCreationTime(p.get()) == created;
}

}  // namespace host
