# SPDX-License-Identifier: GPL-3.0-only
"""Ground truth: every public symbol of <dll> from its PDB, named the way the host names it
(host/symbols.cpp OnSymbol: UnDecorateSymbolNameW flags 0x20800, then FixPtr64), with
System32 dbghelp and the host's SymSetOptions. Writes JSON {name: [rva, ...]} in enumeration
order (the host keeps the first) plus {rva: flags}.
Usage: truth.py <dir with dll+pdb> <dll name> <out.json>
"""
import ctypes, ctypes.wintypes as wt, json, os, sys, time

dbg = ctypes.WinDLL(os.path.join(os.environ["SystemRoot"], "System32", "dbghelp.dll"))


class SYMBOL_INFOW(ctypes.Structure):
    _fields_ = [("SizeOfStruct", wt.ULONG), ("TypeIndex", wt.ULONG), ("Reserved", ctypes.c_uint64 * 2),
                ("Index", wt.ULONG), ("Size", wt.ULONG), ("ModBase", ctypes.c_uint64), ("Flags", wt.ULONG),
                ("Value", ctypes.c_uint64), ("Address", ctypes.c_uint64), ("Register", wt.ULONG),
                ("Scope", wt.ULONG), ("Tag", wt.ULONG), ("NameLen", wt.ULONG), ("MaxNameLen", wt.ULONG),
                ("Name", wt.WCHAR * 1)]


CB = ctypes.WINFUNCTYPE(wt.BOOL, ctypes.POINTER(SYMBOL_INFOW), wt.ULONG, ctypes.c_void_p)
dbg.SymSetOptions.argtypes = [wt.DWORD]
dbg.SymInitializeW.argtypes = [wt.HANDLE, wt.LPCWSTR, wt.BOOL]
dbg.SymLoadModuleExW.restype = ctypes.c_uint64
dbg.SymLoadModuleExW.argtypes = [wt.HANDLE, wt.HANDLE, wt.LPCWSTR, wt.LPCWSTR, ctypes.c_uint64, wt.DWORD,
                                 ctypes.c_void_p, wt.DWORD]
dbg.SymEnumSymbolsW.argtypes = [wt.HANDLE, ctypes.c_uint64, wt.LPCWSTR, CB, ctypes.c_void_p]
dbg.UnDecorateSymbolNameW.argtypes = [wt.LPCWSTR, wt.LPWSTR, wt.DWORD, wt.DWORD]
dbg.UnDecorateSymbolNameW.restype = wt.DWORD

SYMOPT = 0x00004000 | 0x00000400 | 0x00000200 | 0x00000100 | 0x00200000 | 0x00000004
# PUBLICS_ONLY | EXACT_SYMBOLS | FAIL_CRITICAL_ERRORS | NO_PROMPTS | IGNORE_NT_SYMPATH | DEFERRED_LOADS


def fix_ptr64(n):
    n = n.replace("const __ptr64", "const ").replace("volatile __ptr64", "volatile ")
    return n.replace(" __ptr64", "")


def main():
    d, name, out = sys.argv[1:4]
    t = time.time()
    dbg.SymSetOptions(SYMOPT)
    proc = wt.HANDLE(0x50544831)
    if not dbg.SymInitializeW(proc, d, False):
        raise SystemExit("SymInitialize failed %d" % ctypes.GetLastError())
    base = 0x180000000
    size = os.path.getsize(os.path.join(d, name))  # dbghelp reads the header anyway
    if not dbg.SymLoadModuleExW(proc, None, os.path.join(d, name), None, base, 0, None, 0):
        raise SystemExit("SymLoadModuleEx failed %d" % ctypes.GetLastError())
    names, flags = {}, {}
    buf = ctypes.create_unicode_buffer(8192)

    def on(si, _sz, _ctx):
        s = si.contents
        nm = ctypes.wstring_at(ctypes.addressof(s) + SYMBOL_INFOW.Name.offset, s.NameLen)
        if nm.startswith("?"):
            if not dbg.UnDecorateSymbolNameW(nm, buf, 8192, 0x20800):
                return True
            nm = fix_ptr64(buf.value)
        rva = s.Address - base
        names.setdefault(nm, []).append(rva)
        flags[rva] = s.Flags
        return True

    cb = CB(on)
    dbg.SymEnumSymbolsW(proc, base, "*", cb, None)
    dbg.SymCleanup(proc)
    json.dump({"names": names, "flags": flags}, open(out, "w"))
    print(f"{name} {d[-4:]}: {len(names)} names, {len(flags)} addresses, {time.time()-t:.1f}s")


main()
