// SPDX-License-Identifier: GPL-3.0-only
//
// port::ResolveAll: every public of an OLD build's PDB, named exactly as Resolve in
// symbols.cpp names them (same dbghelp, options, undecoration and FixPtr64), so a
// ported table answers with the names the mods ask for.

#include <windows.h>

#include <dbghelp.h>

#include <algorithm>
#include <cstring>
#include <unordered_map>

#include "port.h"
#include "symbols.h"
#include "util.h"

namespace port {

namespace {

// System32's dbghelp only, loaded at run time like symbols.cpp. Not thread safe: call
// from one thread, never at the same time as host::PrepareSymbols.
struct DbgHelp {
    decltype(&SymSetOptions) setOptions = nullptr;
    decltype(&SymInitializeW) initialize = nullptr;
    decltype(&SymLoadModuleExW) loadModule = nullptr;
    decltype(&SymEnumSymbolsW) enumSymbols = nullptr;
    decltype(&SymGetModuleInfoW64) moduleInfo = nullptr;
    decltype(&SymUnloadModule64) unloadModule = nullptr;
    decltype(&SymCleanup) cleanup = nullptr;
    decltype(&UnDecorateSymbolNameW) undecorate = nullptr;
    HMODULE mod = nullptr;

    template <typename T>
    bool Get(T& fn, const char* name) {
        fn = (T)(void*)GetProcAddress(mod, name);
        return fn != nullptr;
    }
    bool Load() {
        if (mod) return undecorate != nullptr;
        mod = LoadLibraryExW(L"dbghelp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!mod) return false;
        return Get(setOptions, "SymSetOptions") && Get(initialize, "SymInitializeW") &&
               Get(loadModule, "SymLoadModuleExW") && Get(enumSymbols, "SymEnumSymbolsW") &&
               Get(moduleInfo, "SymGetModuleInfoW64") && Get(unloadModule, "SymUnloadModule64") &&
               Get(cleanup, "SymCleanup") && Get(undecorate, "UnDecorateSymbolNameW");
    }
};

DbgHelp g_dbghelp;

struct EnumContext {
    DbgHelp* dbg;
    ULONG64 base;
    std::vector<OldSymbol>* out;
    std::unordered_map<std::wstring, std::vector<uint32_t>> seen;  // addresses given per name
};

BOOL CALLBACK OnSymbol(PSYMBOL_INFOW si, ULONG, PVOID ctxPtr) {
    auto* ctx = (EnumContext*)ctxPtr;
    std::wstring name(si->Name, si->NameLen);
    if (!name.empty() && name[0] == L'?') {
        // The same flags and fix-up as symbols.cpp OnSymbol.
        wchar_t out[8192];
        if (!ctx->dbg->undecorate(name.c_str(), out, ARRAYSIZE(out), 0x20800)) return TRUE;
        name = host::FixPtr64(out);
    }
    if (si->Address < ctx->base || si->Address - ctx->base > 0xFFFFFFFFull) return TRUE;
    // Each distinct address of a name once, in enumeration order: the first is the one the
    // host resolves, and a second one tells the engine the name is not one function.
    uint32_t rva = (uint32_t)(si->Address - ctx->base);
    auto& at = ctx->seen[name];
    if (std::find(at.begin(), at.end(), rva) != at.end()) return TRUE;
    at.push_back(rva);
    ctx->out->push_back({std::move(name), rva});
    return TRUE;
}

}  // namespace

bool ResolveAll(const std::wstring& pdbPath, const std::wstring& dllPath, std::vector<OldSymbol>& out,
                std::wstring& error) {
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
    if (!g_dbghelp.Load()) {
        error = host::Format(L"dbghelp.dll from System32 did not load (%lu)", GetLastError());
        return false;
    }
    DbgHelp& d = g_dbghelp;
    d.setOptions(SYMOPT_PUBLICS_ONLY | SYMOPT_EXACT_SYMBOLS | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS |
                 SYMOPT_IGNORE_NT_SYMPATH | SYMOPT_DEFERRED_LOADS);
    // A key for dbghelp's bookkeeping, distinct from the one symbols.cpp uses.
    HANDLE proc = (HANDLE)(ULONG_PTR)0x50544832;
    if (!d.initialize(proc, host::DirName(pdbPath).c_str(), FALSE)) {
        error = host::Format(L"SymInitialize failed (%lu)", GetLastError());
        return false;
    }
    const ULONG64 base = 0x180000000ull;
    bool ok = false;
    if (d.loadModule(proc, nullptr, dllPath.c_str(), nullptr, base, cv.sizeOfImage, nullptr, 0)) {
        EnumContext ctx{&d, base, &out, {}};
        d.enumSymbols(proc, base, L"*", OnSymbol, &ctx);
        IMAGEHLP_MODULEW64 mi{};
        mi.SizeOfStruct = sizeof(mi);
        bool infoOk = d.moduleInfo(proc, base, &mi);
        ok = infoOk && mi.SymType == SymPdb && memcmp(&mi.PdbSig70, &cv.guid, sizeof(GUID)) == 0 &&
             mi.PdbAge == cv.age;
        if (!ok) {
            error = host::Format(L"dbghelp did not load the matching PDB (type %d)", infoOk ? (int)mi.SymType : -1);
            out.clear();
        }
        d.unloadModule(proc, base);
    } else {
        error = host::Format(L"SymLoadModuleEx failed (%lu)", GetLastError());
    }
    d.cleanup(proc);
    return ok;
}

}  // namespace port
