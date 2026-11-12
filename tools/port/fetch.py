# SPDX-License-Identifier: GPL-3.0-only
"""Downloads a binary + its PDB from msdl into dl/<name>_<build>/.
Usage: fetch.py <dl_root> <name> <build> <key>
"""
import os, struct, sys, time, urllib.request

UA = "PillTaskbarPortCheck/1"


def get(url, dest):
    req = urllib.request.Request(url, headers={"User-Agent": UA})
    t = time.time()
    with urllib.request.urlopen(req, timeout=300) as r, open(dest + ".part", "wb") as f:
        n = 0
        while True:
            b = r.read(1 << 20)
            if not b:
                break
            f.write(b)
            n += len(b)
    os.replace(dest + ".part", dest)
    print(f"  {os.path.basename(dest)}: {n/1e6:.1f} MB in {time.time()-t:.1f}s", flush=True)


def rsds(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    optsz = struct.unpack_from("<H", data, pe + 20)[0]
    opt = pe + 24
    dbg_rva, dbg_size = struct.unpack_from("<II", data, opt + 112 + 6 * 8)
    secs = opt + optsz

    def off(rva):
        for i in range(nsec):
            s = secs + i * 40
            vs, va, _, raw = struct.unpack_from("<IIII", data, s + 8)
            if va <= rva < va + max(vs, 1):
                return rva - va + raw

    for i in range(dbg_size // 28):
        d = off(dbg_rva) + i * 28
        typ, size, rva, ptr = struct.unpack_from("<IIII", data, d + 12)
        if typ == 2 and data[ptr:ptr + 4] == b"RSDS":
            g = data[ptr + 4:ptr + 20]
            age = struct.unpack_from("<I", data, ptr + 20)[0]
            pdb = data[ptr + 24:data.index(b"\0", ptr + 24)].decode()
            d1, d2, d3 = struct.unpack_from("<IHH", g)
            return pdb, "%08X%04X%04X%s%X" % (d1, d2, d3, g[8:].hex().upper(), age)


def main():
    root, name, build, key = sys.argv[1:5]
    d = os.path.join(root, f"{name}_{build}")
    os.makedirs(d, exist_ok=True)
    dll = os.path.join(d, name)
    if not os.path.exists(dll):
        get(f"https://msdl.microsoft.com/download/symbols/{name}/{key}/{name}", dll)
    pdb, gid = rsds(open(dll, "rb").read())
    print(f"  {name} {build}: {pdb} {gid}")
    p = os.path.join(d, pdb)
    if not os.path.exists(p):
        get(f"https://msdl.microsoft.com/download/symbols/{pdb}/{gid}/{pdb}", p)
    with open(os.path.join(d, "rsds.txt"), "w") as f:
        f.write(f"{pdb} {gid}\n")


main()
