# SPDX-License-Identifier: GPL-3.0-only
"""Pre-commit check for the public PillTaskbar repository.

Usage: check-public.py [--root DIR] [--private FILE] [--exclude DIR]... [FILE...]

With no FILE it takes `git ls-files` (when that lists anything), else walks the root
and skips the private folders. Errors make the exit code 1, warnings do not.

Errors: personal values (user name, profile path, git identity, plus the patterns in
the optional --private file, one per line, kept outside the repository), local user
paths, e-mail addresses, KakaoTalk mentions, .pdb files, offset files, a filled
usage::kLogos table, files over 5 MB outside third_party.
Warnings: source files without an SPDX-License-Identifier line in their first 15 lines
(a Windhawk mod keeps its metadata block first).
"""
import argparse
import os
import re
import subprocess
import sys
from collections import Counter

MAX_BYTES = 5 * 1024 * 1024
PRIVATE_DIRS = {"docs", ".work", "build", ".git", "__pycache__"}
SOURCE_EXTS = {".cpp", ".h", ".hpp", ".c", ".py", ".sh", ".ps1", ".yml", ".yaml"}
IMAGE_TLDS = {"png", "jpg", "jpeg", "gif", "webp", "svg", "ico", "bmp"}
SAFE_EMAIL_DOMAINS = ("users.noreply.github.com", "example.com", "example.org")
SAFE_USER_NAMES = {"public", "default", "all users", "name", "user", "username", "you", "yourname"}

USER_PATH = re.compile(r"(?:[A-Za-z]:[\\/]+|/[A-Za-z]/)Users[\\/]+([^\\/\s\"'<>|*?:]+)", re.I)
EMAIL = re.compile(r"[A-Za-z0-9._%+-]+@([A-Za-z0-9-]+(?:\.[A-Za-z0-9-]+)*\.([A-Za-z]{2,}))")
KAKAO = re.compile(r"kakao", re.I)
# 1#module#stamp-size#name#rva#... as the host writes its offset files.
OFFSET_LINE = re.compile(r"^\d+#[^#\r\n]+#[0-9A-Za-z]+-[0-9A-Za-z]+#")
LOGO_ENTRY = re.compile(r'\{\s*L"[^"]*"\s*,\s*((?:L"[^"\n]*"\s*)+)\}')

findings = []  # (severity, kind, path, line, text)


def add(sev, kind, path, line, text=""):
    findings.append((sev, kind, path, line, text.strip()[:100]))


def git_value(*args):
    try:
        out = subprocess.run(["git", *args], capture_output=True, text=True, timeout=10)
        return out.stdout.strip() if out.returncode == 0 else ""
    except (OSError, subprocess.SubprocessError):
        return ""


def personal_patterns(private_file):
    """Run-time values only: nothing personal is written into this script."""
    vals = {}
    profile = os.environ.get("USERPROFILE", "")
    for label, v in (("USERNAME", os.environ.get("USERNAME", "")),
                     ("USERPROFILE", profile),
                     ("profile folder", os.path.basename(profile.rstrip("\\/"))),
                     # The global identity is the private one; a repository may set
                     # its own public name and noreply address on purpose.
                     ("git user.name", git_value("config", "--global", "user.name")),
                     ("git user.email", git_value("config", "--global", "user.email"))):
        if len(v) >= 3:
            vals[v] = label
    if profile:
        vals[profile.replace("\\", "/")] = "USERPROFILE"
    if private_file:
        with open(private_file, encoding="utf-8") as f:
            for raw in f:
                v = raw.strip()
                if v and not v.startswith("#") and len(v) >= 2:
                    vals[v] = "private file"
    # Whole-word match, so a short name does not hit "Sleep" or "FileExists".
    return [(re.compile(r"(?<![A-Za-z0-9])" + re.escape(v) + r"(?![A-Za-z0-9])", re.I), label)
            for v, label in vals.items()]


def list_files(root, explicit, excludes):
    if explicit:
        return [os.path.relpath(os.path.abspath(p), root) for p in explicit]
    out = subprocess.run(["git", "-C", root, "ls-files"], capture_output=True, text=True)
    if out.returncode == 0 and out.stdout.strip():
        return out.stdout.splitlines()
    files = []
    for d, dirs, names in os.walk(root):
        dirs[:] = [x for x in dirs if x not in excludes]
        files += [os.path.relpath(os.path.join(d, n), root) for n in names]
    return sorted(files)


def check_file(root, rel, patterns):
    rel_slash = rel.replace("\\", "/")
    path = os.path.join(root, rel)
    parts = rel_slash.split("/")
    ext = os.path.splitext(rel_slash)[1].lower()
    in_third_party = parts[0] == "third_party"
    if not os.path.isfile(path):
        return
    size = os.path.getsize(path)
    if ext == ".pdb":
        add("error", "pdb", rel_slash, 0)
    if "offsets" in parts[:-1]:
        add("error", "offsets-dir", rel_slash, 0)
    if size > MAX_BYTES and not in_third_party:
        add("error", "big-file", rel_slash, 0, f"{size / 1048576:.1f} MB")
    if size > MAX_BYTES:
        return
    with open(path, "rb") as f:
        data = f.read()
    if b"\0" in data[:8192]:
        return
    text = data.decode("utf-8", errors="replace")
    lines = text.splitlines()
    name = os.path.basename(rel_slash)
    if rel_slash == "tools/check-public.py":  # its own patterns would match themselves
        return

    if ext == ".txt" and "_" in name and lines and OFFSET_LINE.match(lines[0]):
        add("error", "offsets-file", rel_slash, 1, lines[0][:60])
    if ext in SOURCE_EXTS and not in_third_party and not any("SPDX-License-Identifier" in l for l in lines[:15]):
        add("warn", "no-spdx", rel_slash, 1)

    for n, line in enumerate(lines, 1):
        for rx, label in patterns:
            if rx.search(line):
                add("error", "personal", rel_slash, n, f"{label}: {line}")
        for m in USER_PATH.finditer(line):
            who = m.group(1)
            if who.lower() not in SAFE_USER_NAMES and not re.search(r"[<>%${}]", who):
                add("error", "user-path", rel_slash, n, line)
        if not in_third_party:  # upstream authors' addresses stay in their notices
            for m in EMAIL.finditer(line):
                if m.group(2).lower() in IMAGE_TLDS or m.group(1).lower().endswith(SAFE_EMAIL_DOMAINS):
                    continue
                add("error", "email", rel_slash, n, line)
        if KAKAO.search(line):
            add("error", "kakao", rel_slash, n, line)

    if ext in (".cpp", ".h", ".hpp") and "kLogos[]" in text:
        start = text.index("kLogos[]")
        end = text.find("};", start)
        block = text[start:end]
        for m in LOGO_ENTRY.finditer(block):
            body = re.sub(r'L"|"|\s', "", m.group(1))
            if body:
                line_no = text.count("\n", 0, start + m.start()) + 1
                add("error", "logo-path", rel_slash, line_no, f"kLogos entry with {len(body)} chars of path data")


def main():
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser()
    ap.add_argument("files", nargs="*")
    ap.add_argument("--root", default=os.path.join(here, ".."))
    ap.add_argument("--private", help="file with extra patterns, one per line, outside the repository")
    ap.add_argument("--exclude", action="append", default=[], help="extra folder name to skip when walking")
    a = ap.parse_args()
    root = os.path.abspath(a.root)
    patterns = personal_patterns(a.private)
    files = list_files(root, a.files, PRIVATE_DIRS | set(a.exclude))
    for rel in files:
        check_file(root, rel, patterns)
    for sev, kind, path, line, text in findings:
        where = f"{path}:{line}" if line else path
        print(f"{sev.upper():5} {kind:12} {where}  {text}")
    kinds = Counter((sev, kind) for sev, kind, *_ in findings)
    print(f"\nchecked {len(files)} files")
    for (sev, kind), c in sorted(kinds.items()):
        print(f"  {sev:5} {kind}: {c}")
    errors = sum(1 for f in findings if f[0] == "error")
    print("FAIL" if errors else "OK", f"({errors} errors, {len(findings) - errors} warnings)")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
