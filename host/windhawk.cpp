// SPDX-License-Identifier: GPL-3.0-only

#include "windhawk.h"

#include <windows.h>

#include <tlhelp32.h>

#include "addon_protocol.h"
#include "util.h"

namespace host {

WindhawkEngine FindWindhawkEngine() {
    WindhawkEngine e;
    Handle snap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (snap.get() != INVALID_HANDLE_VALUE && Process32FirstW(snap.get(), &pe)) {
        do {
            if (EqualsNoCase(pe.szExeFile, L"windhawk.exe")) {
                e.running = true;
                break;
            }
        } while (Process32NextW(snap.get(), &pe));
    }
    if (SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT)) {
        if (SC_HANDLE svc = OpenServiceW(scm, L"windhawk", SERVICE_QUERY_CONFIG)) {
            DWORD needed = 0;
            QueryServiceConfigW(svc, nullptr, 0, &needed);
            std::vector<BYTE> buffer(needed);
            auto* config = reinterpret_cast<QUERY_SERVICE_CONFIGW*>(buffer.data());
            if (needed && QueryServiceConfigW(svc, config, needed, &needed))
                e.autoStart = config->dwStartType <= SERVICE_AUTO_START;
            CloseServiceHandle(svc);
        }
        CloseServiceHandle(scm);
    }
    return e;
}

WindhawkConflict WindhawkRule(bool modOn, bool modInstalled, const WindhawkEngine& engine) {
    WindhawkConflict c;
    if (!modInstalled) {
        c.detail = L"not installed in Windhawk";
    } else if (!modOn) {
        c.detail = L"installed in Windhawk, disabled";
    } else {
        // Only Windhawk's engine loads the mod: it runs now, or the service brings it
        // back at the next boot.
        c.registryOn = engine.running || engine.autoStart;
        c.detail = engine.running     ? L"installed in Windhawk, enabled, windhawk.exe running"
                   : engine.autoStart ? L"installed in Windhawk, enabled, its service starts at boot"
                                      : L"installed in Windhawk, enabled, but Windhawk is not running and its "
                                        L"service is not set to start by itself";
    }
    return c;
}

WindhawkConflict CheckWindhawkRegistry() {
    bool installed = false, on = false;
    for (const wchar_t* id : addon::kWindhawkMods) {
        HKEY key;
        std::wstring path = std::wstring(L"SOFTWARE\\Windhawk\\Engine\\Mods\\") + id;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key) != ERROR_SUCCESS)
            continue;
        DWORD disabled = 0, size = sizeof(disabled), type = 0;
        LSTATUS r = RegQueryValueExW(key, L"Disabled", nullptr, &type, (BYTE*)&disabled, &size);
        RegCloseKey(key);
        installed = true;
        // A missing value means enabled, the same as Windhawk reads it.
        on = on || !(r == ERROR_SUCCESS && type == REG_DWORD && disabled != 0);
    }
    return WindhawkRule(on, installed, on ? FindWindhawkEngine() : WindhawkEngine{});
}

void CheckWindhawkModules(const std::vector<std::wstring>& explorerModules, WindhawkConflict& c) {
    for (auto& p : explorerModules) {
        for (const wchar_t* id : addon::kWindhawkMods) {
            if (StartsWithNoCase(FileName(p), std::wstring(id) + L"_")) {
                c.loaded = true;
                c.detail += L"; explorer has " + FileName(p);
                return;
            }
        }
    }
}

}  // namespace host
