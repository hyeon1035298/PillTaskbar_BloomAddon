# SPDX-License-Identifier: GPL-3.0-only
"""Writes addon.json for the bundle folder: identity, host, and a SHA-256 per file.

Bloom checks every listed hash when it copies the bundle to the run folder, so the
list is exactly the files Bloom needs. The names and version come from the add-on's
addon.conf (through gen/identity.json, tools/make-identity.py), the protocol from
common/addon_protocol.h.

Usage: make-manifest.py <bundle folder> <identity.json>
"""
import hashlib
import json
import os
import re
import sys


def protocol_value():
    header = os.path.join(os.path.dirname(__file__), "..", "common", "addon_protocol.h")
    with open(header, encoding="utf-8") as f:
        text = f.read()
    return int(re.search(r"kProtocol = (\d+);", text).group(1))


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for block in iter(lambda: f.read(1 << 20), b""):
            h.update(block)
    return h.hexdigest()


def main():
    folder, identity = sys.argv[1], sys.argv[2]
    with open(identity, encoding="utf-8") as f:
        conf = json.load(f)
    name, version, protocol = conf["ADDON_NAME"], conf["ADDON_VERSION"], protocol_value()
    schema = conf.get("ADDON_SCHEMA") == "1"
    files = [f"{name}Host.exe", f"{name}Runtime.dll", f"{name}.dll", "symbols.txt", "THIRD_PARTY.md",
             "THIRD_PARTY_NOTICES.txt"] + (["settings.schema.json"] if schema else [])
    missing = [n for n in files if not os.path.isfile(os.path.join(folder, n))]
    if missing:
        raise SystemExit(f"missing in {folder}: {', '.join(missing)}")
    manifest = {
        "schema": 1,
        "id": conf["ADDON_ID"],
        "name": {"ko": conf["ADDON_NAME_KO"], "en": conf["ADDON_NAME_EN"]},
        "about": {
            "ko": conf.get("ADDON_ABOUT_KO", ""),
            "en": conf.get("ADDON_ABOUT_EN", ""),
        },
        "version": version,
        "license": conf["ADDON_LICENSE"],
        "source": conf.get("ADDON_SOURCE", ""),
        "minBloom": conf["ADDON_MIN_BLOOM"],
        "host": {"exe": f"{name}Host.exe", "protocol": protocol},
        "needs": conf.get("ADDON_NEEDS", "").split(),
    }
    if schema:
        manifest["settings"] = "settings.schema.json"
    manifest["files"] = {n: sha256(os.path.join(folder, n)) for n in files}
    out = os.path.join(folder, "addon.json")
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        json.dump(manifest, f, ensure_ascii=False, indent=2)
        f.write("\n")
    print(f"addon.json {version}, protocol {protocol}, {len(files)} files")


if __name__ == "__main__":
    main()
