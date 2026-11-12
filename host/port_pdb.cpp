// SPDX-License-Identifier: GPL-3.0-only
//
// port::ResolveAll: every public of an OLD build's PDB through host::EnumPublics, the
// walk host::Resolve takes too, so a ported table answers with the names the mods ask
// for.

#include <windows.h>

#include <algorithm>
#include <unordered_map>

#include "port.h"
#include "symbols.h"
#include "util.h"

namespace port {

bool ResolveAll(const std::wstring& pdbPath, const std::wstring& dllPath, std::vector<OldSymbol>& out,
                std::wstring& error, const std::atomic<bool>* cancel) {
    out.clear();
    host::CodeView cv;
    if (!host::ReadCodeView(dllPath, cv)) {
        error = L"no RSDS record in " + host::FileName(dllPath);
        return false;
    }
    if (!host::EqualsNoCase(host::FileName(pdbPath), cv.pdbName)) {
        error = L"the PDB file must be named " + cv.pdbName;
        return false;
    }
    // Each distinct address of a name once, in enumeration order: the first is the one the
    // host resolves, and a second one tells the engine the name is not one function.
    std::unordered_map<std::wstring, std::vector<uint32_t>> seen;
    bool stopped = false;
    bool ok = host::EnumPublics(dllPath, cv, pdbPath, [&](std::wstring& name, DWORD rva) {
        if (cancel && cancel->load(std::memory_order_relaxed)) return false;
        auto& at = seen[name];
        if (std::find(at.begin(), at.end(), rva) != at.end()) return true;
        at.push_back(rva);
        out.push_back({std::move(name), rva});
        return true;
    }, error, nullptr, &stopped);
    if (stopped) {
        error = L"stopped";
        ok = false;
    }
    if (!ok) out.clear();
    return ok;
}

}  // namespace port
