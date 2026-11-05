# SPDX-License-Identifier: GPL-3.0-only
"""Writes symbols.txt: every symbol name the mod may ask HookSymbols for, one per line.

The mod writes each name as a raw string literal LR"(...)" and uses raw strings for
nothing else, so the list is every such literal in the source, in source order,
without repeats. The host resolves all of them in each taskbar module; which ones
are required is decided in explorer by the mod's own hook tables.

Usage: symbol-list.py <pill-taskbar.wh.cpp> <symbols.txt>
"""
import re
import sys


def names(source):
    seen = []
    for name in re.findall(r'LR"\((.*?)\)"', source, re.S):
        if "\n" in name or "#" in name:
            raise SystemExit(f"not a symbol name: {name[:80]!r}")
        if name not in seen:
            seen.append(name)
    return seen


def main():
    src, out = sys.argv[1], sys.argv[2]
    with open(src, encoding="utf-8") as f:
        found = names(f.read())
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write("".join(n + "\n" for n in found))
    print(f"{len(found)} symbols -> {out}")


if __name__ == "__main__":
    main()
