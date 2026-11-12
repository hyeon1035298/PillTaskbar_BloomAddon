# SPDX-License-Identifier: GPL-3.0-only
"""Writes an add-on's port-sources.txt: the lines of tools/port-sources.txt (build and
deny lines) for the modules the add-on resolves, in their order. An add-on with none
of them listed still gets the file, holding only its header, so addon.json always
lists it.

Usage: port-sources.py <tools/port-sources.txt> <out file> <module>...
       (modules as in ADDON_SYMBOL_MODULES; a ":core" suffix is ignored)
"""
import sys


def main():
    src, out, modules = sys.argv[1], sys.argv[2], [m.split(":")[0].lower() for m in sys.argv[3:]]
    keep = []
    with open(src, encoding="utf-8") as f:
        for line in f:
            words = line.split("#", 1)[0].split()
            if not words:
                continue
            module = words[1] if words[0] == "deny" and len(words) > 1 else words[0]
            if module.lower() in modules:
                keep.append(line.rstrip("\r\n"))
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write("# Older published builds the host may port symbols from (tools/port-sources.txt).\n")
        f.write("".join(line + "\n" for line in keep))
    print(f"port-sources.txt: {len(keep)} lines for {' '.join(modules)}")


if __name__ == "__main__":
    main()
