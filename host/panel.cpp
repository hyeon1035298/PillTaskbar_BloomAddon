// SPDX-License-Identifier: GPL-3.0-only

#include "panel.h"

#include <vector>

#include "util.h"

namespace host {

bool FindShellHost(Taskbar& out) {
    wchar_t dir[MAX_PATH];
    UINT n = GetSystemDirectoryW(dir, ARRAYSIZE(dir));
    if (!n || n >= ARRAYSIZE(dir)) return false;
    struct Search {
        std::wstring image;
        std::vector<DWORD> other;  // pids already found to be something else
        Taskbar found;
    } search{JoinPath(std::wstring(dir, n), L"ShellHost.exe"), {}, {}};
    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            auto& s = *(Search*)param;
            DWORD pid = 0;
            DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
            if (!tid || !pid) return TRUE;
            for (DWORD o : s.other)
                if (o == pid) return TRUE;
            Handle p(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
            if (!p || !EqualsNoCase(ProcessImagePath(p.get()), s.image)) {
                s.other.push_back(pid);
                return TRUE;
            }
            s.found = {hwnd, pid, tid};
            return FALSE;
        },
        (LPARAM)&search);
    if (!search.found.pid) return false;
    out = search.found;
    return true;
}

}  // namespace host
