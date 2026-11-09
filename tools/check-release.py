# SPDX-License-Identifier: GPL-3.0-only
"""check-release.py <repo> : checks an add-on's latest GitHub release the way Bloom's AddonDownload does: bloom-addon.json, the zip's SHA-256
and size, addon.json's id/version/minBloom, and every file addon.json lists (present, same SHA-256)."""
import hashlib
import io
import json
import sys
import urllib.request
import zipfile

repo = sys.argv[1]
base = f"https://github.com/hyeon1035298/{repo}/releases"


def get(url):
    with urllib.request.urlopen(url, timeout=60) as r:
        return r.read()


info = json.loads(get(f"{base}/latest/download/bloom-addon.json"))
data = get(f"{base}/download/v{info['version']}/{info['zip']}")
problems = []
if hashlib.sha256(data).hexdigest() != info["sha256"].lower():
    problems.append("zip sha256")
if len(data) != info["size"]:
    problems.append("zip size")
z = zipfile.ZipFile(io.BytesIO(data))
m = json.loads(z.read("addon.json"))
for k in ("id", "version", "minBloom"):
    if m.get(k) != info.get(k):
        problems.append(f"addon.json {k}")
names = set(z.namelist())
for n, h in m.get("files", {}).items():
    if n not in names:
        problems.append(f"missing {n}")
    elif hashlib.sha256(z.read(n)).hexdigest() != h.lower():
        problems.append(f"hash {n}")
print(repo, info["version"], "OK" if not problems else problems, "| settings:", m.get("settings"), "| files:", len(m.get("files", {})))
