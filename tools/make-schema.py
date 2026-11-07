# SPDX-License-Identifier: GPL-3.0-only
"""Reads the mod's Windhawk settings block and writes what the add-on build needs.

  make-schema.py --defaults <settings_defaults.h> <pill-taskbar.wh.cpp>
      The C++ table the runtime answers with when values.json lacks a key
      (Windhawk's engine answers 0; its UI writes every default instead).
  make-schema.py --schema <settings.schema.json> <pill-taskbar.wh.cpp>
      The settings page description Bloom reads: groups, keys, types, defaults,
      ranges and choices, Korean text from the YAML and English from names-en.tsv.
  make-schema.py --check <pill-taskbar.wh.cpp>
      Prints the keys by kind, checks the schema against the YAML and exits
      non-zero on anything it cannot read or match.

  --values <values.json> (with --defaults)
      A JSON object whose values replace the YAML defaults of the keys it names.
  --with-files (with --schema)
      Also the ==BloomFileSettings== items (below), for a Bloom that reads their kinds.

The block is the small YAML subset the mod uses: groups "- group:", leaves
"  - key: value" with $name/$description/$options under them. A leaf's kind comes
from its default: true/false is a switch, an integer is a number, anything else a
string; a string with $options is a choice whose value is an option key.
A group's own $name/$description sit at two spaces, a leaf's at four.
A mod without groups has its leaves at the left edge ("- key: value", their
$name/$options at two spaces); its keys carry no group and it has no schema.
Folded text ("key: >-" and the more indented lines under it) reads as one line.

names-en.tsv holds the English text, one "<id>\t<text>" row per Korean text:
"group:<group>" and "group:<group>#about" for groups, "<group.key>" and
"<group.key>#about" for leaves, "<group.key>=<option>" for option labels.

An optional "==BloomFileSettings==" block after the settings holds, as a JSON list
in a comment, items Bloom keeps in a file of the add-on's data folder rather than
in values.json (the mod writes them too). Each is a schema item as Bloom reads it
plus its "group", with its Korean and English text in place; the schema appends
it to that group. They are no Windhawk settings: no defaults table, no names-en.
"""
import argparse
import json
import os
import re
import sys

GROUP = re.compile(r"^- (\w+):\s*$")
LEAF = re.compile(r"^  - (\w+):\s*(.*?)\s*$")
OPTION = re.compile(r"^    - (\w+):\s*(.*?)\s*$")
OPTIONS = re.compile(r"^    \$options:\s*$")
GROUP_TEXT = re.compile(r"^  \$(name|description):\s*(.*?)\s*$")
LEAF_TEXT = re.compile(r"^    \$(name|description):\s*(.*?)\s*$")
FLAT_LEAF = re.compile(r"^- (\w+):\s*(\S.*?)\s*$")
FLAT_OPTION = re.compile(r"^  - (\w+):\s*(.*?)\s*$")
FLAT_OPTIONS = re.compile(r"^  \$options:\s*$")
FLAT_TEXT = re.compile(r"^  \$(name|description):\s*(.*?)\s*$")
FOLDED = re.compile(r"^(\s*)(.*:)\s*[>|]-?\s*$")
NAMES_EN = os.path.join(os.path.dirname(os.path.abspath(__file__)), "names-en.tsv")
SCHEMA_TYPES = {"bool": "bool", "int": "int", "choice": "choice", "string": "text"}


def settings_block(source):
    m = re.search(r"// ==WindhawkModSettings==\s*/\*(.*?)\*/\s*// ==/WindhawkModSettings==", source, re.S)
    if not m:
        raise SystemExit("no ==WindhawkModSettings== block")
    return unfold(m.group(1).splitlines())


def unfold(lines):
    """Joins each folded value ("key: >-" and the more indented lines under it) into one line."""
    out, i = [], 0
    while i < len(lines):
        m = FOLDED.match(lines[i])
        if not m:
            out.append(lines[i])
            i += 1
            continue
        indent, parts, i = len(m.group(1)), [], i + 1
        while i < len(lines) and lines[i].strip() and len(lines[i]) - len(lines[i].lstrip()) > indent:
            parts.append(lines[i].strip())
            i += 1
        out.append(f"{m.group(1)}{m.group(2)} {' '.join(parts)}")
    return out


def scalar(text):
    if text in ("true", "false"):
        return "bool", text == "true"
    if re.fullmatch(r"-?\d+", text):
        return "int", int(text)
    if len(text) >= 2 and text[0] == text[-1] == "'":
        return "string", text[1:-1].replace("''", "'")
    if len(text) >= 2 and text[0] == text[-1] == '"':
        return "string", text[1:-1]
    return "string", text


FILE_ITEM_FIELDS = ("group", "key", "type", "default", "file", "field", "name")


def file_items(source):
    """The items of the optional ==BloomFileSettings== block."""
    m = re.search(r"// ==BloomFileSettings==\s*/\*(.*?)\*/\s*// ==/BloomFileSettings==", source, re.S)
    if not m:
        return []
    try:
        items = json.loads(m.group(1))
    except json.JSONDecodeError as e:
        raise SystemExit(f"BloomFileSettings: {e}")
    if not isinstance(items, list):
        raise SystemExit("BloomFileSettings: not a list")
    for item in items:
        missing = [f for f in FILE_ITEM_FIELDS if f not in item]
        if missing:
            raise SystemExit(f"BloomFileSettings: {item.get('key')!r} lacks {', '.join(missing)}")
        texts = [item["name"]] + ([item["about"]] if "about" in item else [])
        texts += [t for f in item.get("fields", []) for t in (f.get("name"), f.get("about")) if t]
        if any(not (isinstance(t, dict) and t.get("ko") and t.get("en")) for t in texts):
            raise SystemExit(f"BloomFileSettings: {item['key']}: a text without ko or en")
    return items


def text(raw):
    return str(scalar(raw)[1]) if raw else ""


def parse(source, groups=None):
    """Leaves in YAML order. When groups is a list it is filled with each group's id
    and texts; every leaf also carries its group, texts and option labels."""
    items, group, leaf, in_options = [], None, None, False
    if groups is None:
        groups = []
    for line in settings_block(source):
        if not line.strip():
            continue
        flat = leaf is not None and leaf["group"] is None
        if group is None and (m := FLAT_LEAF.match(line)):
            kind, value = scalar(m.group(2))
            leaf = {"key": m.group(1), "kind": kind, "default": value, "options": [],
                    "group": None, "text": {}, "labels": {}}
            items.append(leaf)
            in_options = False
        elif flat and FLAT_OPTIONS.match(line):
            in_options = True
        elif flat and in_options and (m := FLAT_OPTION.match(line)):
            leaf["options"].append(m.group(1))
            leaf["labels"][m.group(1)] = text(m.group(2))
        elif flat and (m := FLAT_TEXT.match(line)):
            leaf["text"][m.group(1)] = text(m.group(2))
            in_options = False
        elif m := GROUP.match(line):
            if items and items[-1]["group"] is None:
                raise SystemExit(f"a group after leaves without one: {line!r}")
            group, leaf, in_options = m.group(1), None, False
            groups.append({"id": group, "text": {}})
        elif m := LEAF.match(line):
            if group is None:
                raise SystemExit(f"leaf outside a group: {line!r}")
            kind, value = scalar(m.group(2))
            leaf = {"key": f"{group}.{m.group(1)}", "kind": kind, "default": value, "options": [],
                    "group": group, "text": {}, "labels": {}}
            items.append(leaf)
            in_options = False
        elif OPTIONS.match(line):
            in_options = True
        elif in_options and (m := OPTION.match(line)):
            leaf["options"].append(m.group(1))
            leaf["labels"][m.group(1)] = text(m.group(2))
        elif m := GROUP_TEXT.match(line):
            if group is None:
                raise SystemExit(f"group text outside a group: {line!r}")
            groups[-1]["text"][m.group(1)] = text(m.group(2))
            leaf, in_options = None, False
        elif (m := LEAF_TEXT.match(line)) and leaf is not None:
            leaf["text"][m.group(1)] = text(m.group(2))
            in_options = False
        elif line.lstrip().startswith("$"):
            in_options = False
        else:
            raise SystemExit(f"unread settings line: {line!r}")
    for item in items:
        if item["options"]:
            item["kind"] = "choice"
            if item["default"] not in item["options"]:
                raise SystemExit(f"{item['key']}: default {item['default']!r} is not an option")
    keys = [i["key"] for i in items]
    if len(keys) != len(set(keys)):
        raise SystemExit("a key appears twice")
    return items


def ranges(source):
    """Number ranges from the mod's Clamped(L"group.key", lo, hi) reads."""
    found = {}
    for key, lo, hi in re.findall(r'Clamped\(L"([\w.]+)", (-?\d+), (-?\d+)\)', source):
        found[key] = (int(lo), int(hi))
    return found


def check(items, found):
    problems = []
    for item in items:
        if item["kind"] != "int":
            continue
        span = found.get(item["key"])
        if not span:
            problems.append(f"{item['key']}: no Clamped range")
        elif not span[0] <= item["default"] <= span[1]:
            problems.append(f"{item['key']}: default {item['default']} outside {span}")
    for key in found:
        if key not in {i["key"] for i in items}:
            problems.append(f"{key}: Clamped for a key the YAML lacks")
    return problems


def read_names(path):
    names = {}
    with open(path, encoding="utf-8") as f:
        for n, line in enumerate(f, 1):
            line = line.rstrip("\n")
            if not line.strip():
                continue
            if line.count("\t") != 1:
                raise SystemExit(f"{path}:{n}: expected <id><TAB><english>")
            key, english = line.split("\t")
            if not key or not english.strip():
                raise SystemExit(f"{path}:{n}: empty id or text")
            if key in names:
                raise SystemExit(f"{path}:{n}: {key} appears twice")
            names[key] = english
    return names


def build_schema(items, groups, found, names, files=()):
    """The settings.schema.json object. Exits naming every English text that is
    missing and every table id that matches no YAML text."""
    used, missing = set(), []
    if any(i["group"] is None for i in items):
        raise SystemExit("settings without groups have no schema")

    def pair(korean, english_id):
        used.add(english_id)
        if english_id not in names:
            missing.append(english_id)
        return {"ko": korean, "en": names.get(english_id, "")}

    out_groups = []
    for g in groups:
        if "name" not in g["text"]:
            raise SystemExit(f"group {g['id']} has no $name")
        entry = {"id": g["id"], "name": pair(g["text"]["name"], f"group:{g['id']}")}
        if "description" in g["text"]:
            entry["about"] = pair(g["text"]["description"], f"group:{g['id']}#about")
        entry["items"] = []
        out_groups.append(entry)
    by_id = {g["id"]: g for g in out_groups}
    for item in items:
        key = item["key"]
        if "name" not in item["text"]:
            raise SystemExit(f"{key} has no $name")
        entry = {"key": key, "type": SCHEMA_TYPES[item["kind"]], "default": item["default"]}
        if item["kind"] == "int":
            if key not in found:
                raise SystemExit(f"{key}: no Clamped range")
            entry["min"], entry["max"] = found[key]
        if item["kind"] == "choice":
            entry["options"] = [{"value": o, "name": pair(item["labels"][o], f"{key}={o}")}
                                for o in item["options"]]
        entry["name"] = pair(item["text"]["name"], key)
        if "description" in item["text"]:
            entry["about"] = pair(item["text"]["description"], f"{key}#about")
        by_id[item["group"]]["items"].append(entry)
    keys = {i["key"] for i in items}
    for item in files:
        if item["group"] not in by_id:
            raise SystemExit(f"{item['key']}: no group {item['group']}")
        if item["key"] in keys:
            raise SystemExit(f"{item['key']}: also a YAML key")
        keys.add(item["key"])
        by_id[item["group"]]["items"].append({k: v for k, v in item.items() if k != "group"})
    unused = sorted(set(names) - used)
    if missing or unused:
        lines = [f"names-en.tsv: no English for {i}" for i in missing]
        lines += [f"names-en.tsv: {i} matches no YAML text" for i in unused]
        raise SystemExit("\n".join(lines))
    return {"schema": 1, "groups": out_groups}


def check_schema(schema, items, found):
    """Compares a schema read back from JSON with the YAML leaves and Clamped ranges."""
    problems = []
    leaves = [e for g in schema["groups"] for e in g["items"] if "file" not in e]
    yaml = {i["key"]: i for i in items}
    if [e["key"] for e in leaves] != [i["key"] for i in items]:
        problems.append("schema keys differ from the YAML keys or their order")

    def texts(where, obj):
        if not (obj.get("ko") and obj.get("en")):
            problems.append(f"{where}: missing ko or en")

    for g in schema["groups"]:
        texts(f"group {g['id']}", g["name"])
        if "about" in g:
            texts(f"group {g['id']} about", g["about"])
    for e in leaves:
        item = yaml.get(e["key"])
        if item is None:
            continue
        if e["default"] != item["default"] or type(e["default"]) is not type(item["default"]):
            problems.append(f"{e['key']}: default {e['default']!r} != YAML {item['default']!r}")
        if e["type"] == "int":
            if (e.get("min"), e.get("max")) != found.get(e["key"]):
                problems.append(f"{e['key']}: range differs from Clamped")
            elif not e["min"] <= e["default"] <= e["max"]:
                problems.append(f"{e['key']}: default outside {e['min']}..{e['max']}")
        if e["type"] == "choice":
            if [o["value"] for o in e["options"]] != item["options"]:
                problems.append(f"{e['key']}: option values differ from $options")
            for o in e["options"]:
                texts(f"{e['key']}={o['value']}", o["name"])
        texts(e["key"], e["name"])
        if "about" in e:
            texts(f"{e['key']} about", e["about"])
        if ("about" in e) != ("description" in item["text"]):
            problems.append(f"{e['key']}: about does not follow $description")
    return problems


def write_schema(schema, path):
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        json.dump(schema, f, ensure_ascii=False, indent=2)
        f.write("\n")


def cpp_string(text):
    return 'L"' + text.replace("\\", "\\\\").replace('"', '\\"') + '"'


def apply_values(items, path):
    """Replaces the defaults of the keys the JSON object names; each value must fit its
    setting (a bool, an integer, a string, or one of a choice's options)."""
    with open(path, encoding="utf-8") as f:
        values = json.load(f)
    if not isinstance(values, dict):
        raise SystemExit(f"{path}: not a JSON object")
    by_key = {i["key"]: i for i in items}
    for key, value in values.items():
        item = by_key.get(key)
        if item is None:
            raise SystemExit(f"{path}: {key} is not a setting of the mod")
        kind = item["kind"]
        fits = (isinstance(value, bool) if kind == "bool"
                else isinstance(value, int) and not isinstance(value, bool) if kind == "int"
                else isinstance(value, str) and (kind != "choice" or value in item["options"]))
        if not fits:
            raise SystemExit(f"{path}: {key} = {value!r} does not fit a {kind} setting")
        item["default"] = value


def write_defaults(items, path, source_name):
    lines = [
        f"// Generated by tools/make-schema.py from {source_name}. Do not edit.",
        "#pragma once",
        "",
        "static const PillAddonDefault kSettingDefaults[] = {",
    ]
    for item in items:
        if item["kind"] == "bool":
            lines.append(f"    {{{cpp_string(item['key'])}, {int(item['default'])}, nullptr}},")
        elif item["kind"] == "int":
            lines.append(f"    {{{cpp_string(item['key'])}, {item['default']}, nullptr}},")
        else:
            lines.append(f"    {{{cpp_string(item['key'])}, 0, {cpp_string(item['default'])}}},")
    lines += ["};", ""]
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--defaults")
    ap.add_argument("--schema")
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--values")
    ap.add_argument("--with-files", action="store_true")
    ap.add_argument("source")
    args = ap.parse_args()
    with open(args.source, encoding="utf-8") as f:
        source = f.read()
    groups = []
    items = parse(source, groups)
    files = file_items(source)
    found = ranges(source)
    kinds = {}
    for item in items:
        kinds[item["kind"]] = kinds.get(item["kind"], 0) + 1
    summary = ", ".join(f"{k} {n}" for k, n in sorted(kinds.items()))
    if args.values and not args.defaults:
        raise SystemExit("--values goes with --defaults")
    if args.defaults:
        if args.values:
            apply_values(items, args.values)
        write_defaults(items, args.defaults, os.path.basename(args.source))
        print(f"{len(items)} settings ({summary}) -> {args.defaults}")
    if args.schema:
        problems = check(items, found)
        if problems:
            raise SystemExit("\n".join(problems))
        kept = files if args.with_files else ()
        schema = build_schema(items, groups, found, read_names(NAMES_EN), kept)
        write_schema(schema, args.schema)
        print(f"{len(groups)} groups, {len(items)} settings, {len(kept)} in files -> {args.schema}")
    if args.check:
        print(f"{len(items)} settings ({summary}), {len(found)} ranges")
        for item in items:
            extra = f" {item['options']}" if item["options"] else ""
            span = f" {found[item['key']]}" if item["key"] in found else ""
            print(f"  {item['key']} {item['kind']} = {item['default']!r}{span}{extra}")
        problems = check(items, found)
        schema = json.loads(json.dumps(build_schema(items, groups, found, read_names(NAMES_EN), files)))
        problems += check_schema(schema, items, found)
        leaves = [e for g in schema["groups"] for e in g["items"]]
        counts = {t: sum(e["type"] == t for e in leaves) for t in ("bool", "int", "choice", "text")}
        print(f"schema: {len(schema['groups'])} groups, {len(leaves)} settings "
              f"({', '.join(f'{t} {n}' for t, n in counts.items())}), "
              f"{sum('min' in e for e in leaves)} int ranges, "
              f"{sum(len(e.get('options', [])) for e in leaves)} options, "
              f"{sum('about' in g for g in schema['groups']) + sum('about' in e for e in leaves)} abouts")
        for item in files:
            print(f"  {item['key']} {item['type']} in {item['file']}:{item['field']}")
        for problem in problems:
            print("PROBLEM", problem)
        return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
