# SPDX-License-Identifier: GPL-3.0-only
"""Writes symbols.txt: every symbol name the mod may ask HookSymbols for, one per line.

The mod writes each name as a raw string literal LR"(...)" and uses raw strings for
nothing else, so the list is every such literal in the source, in source order,
without repeats. A mod that includes another mod file of its folder
(#include "<name>.wh.cpp") has that file's names at the include. The host resolves
all of them in each taskbar module; which ones are required is decided in explorer
by the mod's own hook tables.

Usage: symbol-list.py <pill-taskbar.wh.cpp> <symbols.txt>
"""
import os
import re
import sys

INCLUDE = re.compile(r'^#include "([^"/\\]+\.wh\.cpp)"[ \t]*$', re.M)


def read(path, seen=()):
    """The source with each #include of a mod file next to it replaced by that file."""
    if path in seen:
        raise SystemExit(f"{path} includes itself")
    with open(path, encoding="utf-8") as f:
        source = f.read()
    here = os.path.dirname(path)
    return INCLUDE.sub(lambda m: read(os.path.join(here, m.group(1)), (*seen, path)), source)


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
    found = names(read(os.path.abspath(src)))
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write("".join(n + "\n" for n in found))
    print(f"{len(found)} symbols -> {out}")


if __name__ == "__main__":
    main()
