// SPDX-License-Identifier: GPL-3.0-only

#include "args.h"

#include <shellapi.h>

#include "addon_protocol.h"
#include "util.h"

namespace host {

std::vector<std::wstring> SplitCommandLine(const std::wstring& line) {
    std::vector<std::wstring> out;
    // CommandLineToArgvW treats the first token as a program path with its own
    // quoting rules; a dummy program name keeps every real token on the normal rules.
    std::wstring full = L"x " + line;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(full.c_str(), &argc);
    if (!argv) return out;
    for (int i = 1; i < argc; i++) out.emplace_back(argv[i]);
    LocalFree(argv);
    if (!out.empty() && out[0].rfind(L"--", 0) != 0) out.erase(out.begin());
    return out;
}

namespace {

bool SetCommand(Args& a, Command c) {
    if (a.command != Command::None && a.command != c) {
        a.error = L"more than one command";
        return false;
    }
    a.command = c;
    return true;
}

bool ParsePid(const std::wstring& s, DWORD& pid) {
    if (s.empty() || s.size() > 10) return false;
    uint64_t v = 0;
    for (wchar_t c : s) {
        if (c < L'0' || c > L'9') return false;
        v = v * 10 + (c - L'0');
    }
    if (v == 0 || v > 0xFFFFFFFFull) return false;
    pid = (DWORD)v;
    return true;
}

// A mod DLL is a file name in the host's own folder, never a path.
bool ValidModName(const std::wstring& s) {
    return !s.empty() && s.find_first_of(L"\\/:") == std::wstring::npos && s != L"." && s != L"..";
}

}  // namespace

bool ParseArgs(const std::vector<std::wstring>& argv, Args& a) {
    a = Args{};
    a.mod = addon::kModDll;
    bool modSeen = false;
    for (size_t i = 0; i < argv.size(); i++) {
        const std::wstring& s = argv[i];
        auto value = [&](std::wstring& dst) {
            if (i + 1 >= argv.size() || argv[i + 1].rfind(L"--", 0) == 0) {
                a.error = s + L" needs a value";
                return false;
            }
            dst = argv[++i];
            return true;
        };
        if (s == L"--serve") {
            std::wstring pid;
            if (!SetCommand(a, Command::Serve) || !value(pid)) return false;
            if (!ParsePid(pid, a.bloomPid)) {
                a.error = L"--serve needs a process id";
                return false;
            }
        } else if (s == L"--stop") {
            if (!SetCommand(a, Command::Stop)) return false;
        } else if (s == L"--safe-mode") {
            if (!SetCommand(a, Command::SafeMode)) return false;
        } else if (s == L"--resolve-symbols") {
            if (!SetCommand(a, Command::ResolveSymbols)) return false;
        } else if (s == L"--self-test") {
            if (!SetCommand(a, Command::SelfTest)) return false;
        } else if (s == L"--bloom") {
            if (!value(a.bloomExe)) return false;
        } else if (s == L"--bloom-local") {
            if (!value(a.bloomLocal)) return false;
        } else if (s == L"--dump") {
            if (!value(a.dumpDir)) return false;
        } else if (s == L"--simulate-404") {
            if (!value(a.simulate404)) return false;
        } else if (s == L"--mod") {
            std::wstring m;
            if (!value(m)) return false;
            modSeen = true;
            if (m == L"none") {
                a.mod.clear();
            } else if (ValidModName(m)) {
                a.mod = m;
            } else {
                a.error = L"--mod takes a file name in the host folder or none";
                return false;
            }
        } else if (s == L"--reset-safe-mode") {
            a.resetSafeMode = true;
        } else if (s == L"--log") {
            a.log = true;
        } else if (s == L"--no-download") {
            a.noDownload = true;
        } else if (s == L"--no-windhawk-cache") {
            a.noWindhawkCache = true;
        } else if (s == L"--dry-run") {
            a.dryRun = true;
        } else {
            a.error = L"unknown argument " + s;
            return false;
        }
    }
    switch (a.command) {
        case Command::None:
            a.error = L"no command";
            return false;
        case Command::Serve:
            if (a.bloomLocal.empty()) {
                a.error = L"--serve needs --bloom-local";
                return false;
            }
            break;
        case Command::ResolveSymbols:
            if (a.bloomLocal.empty()) {
                a.error = L"--resolve-symbols needs --bloom-local";
                return false;
            }
            break;
        default: break;
    }
    if (a.dryRun && a.command != Command::Serve) {
        a.error = L"--dry-run goes with --serve";
        return false;
    }
    // A test switch: never honoured by --serve, which serves the real explorer.
    if (!a.simulate404.empty() && a.command != Command::ResolveSymbols) {
        a.error = L"--simulate-404 goes with --resolve-symbols";
        return false;
    }
    if (modSeen && a.command != Command::Serve) {
        a.error = L"--mod goes with --serve";
        return false;
    }
    return true;
}

std::wstring DataDirFor(const std::wstring& bloomLocal) {
    return JoinPath(JoinPath(bloomLocal, L"addons"), addon::kId);
}

std::wstring DefaultBloomLocal() {
    wchar_t buf[MAX_PATH * 2];
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", buf, ARRAYSIZE(buf));
    if (n == 0 || n >= ARRAYSIZE(buf)) return {};
    return JoinPath(buf, L"Bloom");
}

}  // namespace host
