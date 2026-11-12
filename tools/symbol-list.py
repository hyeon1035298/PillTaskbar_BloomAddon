# SPDX-License-Identifier: GPL-3.0-only
"""Writes symbols.txt: every symbol name the mod may ask HookSymbols for, one per line,
and next to it symbols-meta.txt: the same names with <TAB>req or <TAB>opt.

The mod writes each name as a raw string literal LR"(...)" and uses raw strings for
nothing else, so the list is every such literal in the source, in source order,
without repeats. A mod that includes another mod file of its folder
(#include "<name>.wh.cpp") has that file's names at the include. The host resolves
all of them in each taskbar module; which ones are required is decided in explorer
by the mod's own hook tables.

symbols-meta.txt is for the host's symbol porting, which writes a module only when
every required name it can know of ported. A name is "opt" only when every
SYMBOL_HOOK entry that holds it ({{names}, &original, hook, optional}) sets its fourth
field to the literal true; anything else (no fourth field, false, an expression, a
name outside such an entry) makes it "req".

Usage: symbol-list.py <pill-taskbar.wh.cpp> <symbols.txt> [<symbols-meta.txt>]
       (the meta file defaults to symbols-meta.txt beside symbols.txt)
"""
import os
import re
import sys

INCLUDE = re.compile(r'^#include "([^"/\\]+\.wh\.cpp)"[ \t]*$', re.M)
RAW = re.compile(r'LR"\((.*?)\)"', re.S)


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
    for name in RAW.findall(source):
        if "\n" in name or "#" in name:
            raise SystemExit(f"not a symbol name: {name[:80]!r}")
        if name not in seen:
            seen.append(name)
    return seen


def code_marks(source):
    """Positions of {, }, (, ), [, ] and , outside comments, strings and character
    literals, and the span and text of each raw symbol string."""
    marks, raws = [], []
    i, n = 0, len(source)
    while i < n:
        c = source[i]
        if source.startswith("//", i):
            j = source.find("\n", i)
            i = n if j < 0 else j
        elif source.startswith("/*", i):
            j = source.find("*/", i + 2)
            i = n if j < 0 else j + 2
        elif source.startswith('LR"(', i):
            j = source.find(')"', i + 4)
            if j < 0:
                break
            raws.append((i, j + 2, source[i + 4:j]))
            i = j + 2
        elif source.startswith('R"', i) and not source[i - 1:i].isalnum():
            # Another raw string: R"delim( ... )delim"
            k = source.find("(", i + 2)
            delim = source[i + 2:k]
            j = source.find(")" + delim + '"', k)
            i = n if j < 0 else j + len(delim) + 2
        elif c in "\"'":
            j = i + 1
            while j < n and source[j] != c:
                j += 2 if source[j] == "\\" else 1
            i = j + 1
        else:
            if c in "{}()[],":
                marks.append((i, c))
            i += 1
    return marks, raws


def groups(marks):
    """Every bracket group as [open char, start, end, top-level comma positions]."""
    out, stack = [], []
    for pos, c in marks:
        if c in "{([":
            stack.append([c, pos, None, []])
        elif c in "})]":
            if stack:
                g = stack.pop()
                g[2] = pos
                out.append(g)
        elif stack:
            stack[-1][3].append(pos)
    return out


def flags(source):
    """name -> True when some entry needs it."""
    marks, raws = code_marks(source)
    braces = [g for g in groups(marks) if g[0] == "{"]
    required = {}
    for start, end, name in raws:
        inner = min((g for g in braces if g[1] < start and end <= g[2]), key=lambda g: g[2] - g[1], default=None)
        entry = None
        if inner is not None:
            entry = min((g for g in braces if g[1] < inner[1] and inner[2] < g[2]), key=lambda g: g[2] - g[1],
                        default=None)
        optional = False
        if entry is not None:
            cuts = [entry[1]] + entry[3] + [entry[2]]
            args = [source[a + 1:b].strip() for a, b in zip(cuts, cuts[1:])]
            # The names group must be the entry's first field.
            if inner[2] < cuts[1] and len(args) >= 4:
                optional = args[3] == "true"
        required[name] = required.get(name, False) or not optional
    return required


def main():
    src, out = sys.argv[1], sys.argv[2]
    meta = sys.argv[3] if len(sys.argv) > 3 else os.path.join(os.path.dirname(os.path.abspath(out)),
                                                               "symbols-meta.txt")
    source = read(os.path.abspath(src))
    found = names(source)
    req = flags(source)
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write("".join(n + "\n" for n in found))
    with open(meta, "w", encoding="utf-8", newline="\n") as f:
        f.write("".join(f"{n}\t{'req' if req.get(n, True) else 'opt'}\n" for n in found))
    optional = sum(1 for n in found if not req.get(n, True))
    print(f"{len(found)} symbols -> {out}, {optional} of them optional -> {meta}")


if __name__ == "__main__":
    main()
