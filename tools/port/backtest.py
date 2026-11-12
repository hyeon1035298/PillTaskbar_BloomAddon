# SPDX-License-Identifier: GPL-3.0-only
"""Backtest for the symbol porter: runs port-check over pairs of published builds and
scores every answer against the NEW build's real PDB (dev only).

Usage: backtest.py --port-check <port-check.exe> --cache <dir>[;<dir>...] [--lists symbols.txt ...]
                   [--pairs train|holdout|all|<module>:<old>:<new>,...] [--fetch] [--out <dir>]

The cache holds dl/<module>_<build>/<module> and its PDB (fetch.py layout, --fetch downloads
missing ones from the Microsoft symbol server by the keys below) and truth/<module>_<build>.json
(truth.py, made on demand). Keys are %08X TimeDateStamp + %X SizeOfImage, taken from Winbindex
data (winbindex.m417z.com, GPL-3.0) for the numbered builds.

Scoring, per name:
  correct    ported to the RVA the NEW PDB gives for that name (its first address)
  WRONG      ported anywhere else, or ported while the name no longer exists in NEW
             ("gone"; counted separately too)
  unported   ambiguous / not found / absent
Recall counts wanted names that exist in both builds.
"""
import argparse, json, os, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))

KEYS = {
    "taskbar.dll": {"9168": "AF54E51D302000", "9278": "32859388304000", "9444": "8DBF8CD1304000",
                    "9549": "E8E4B3D1308000", "9711": "769B877B308000", "9818": "617E422B31A000"},
    "twinui.pcshell.dll": {"9168": "1C8AB037999000", "9278": "C5AFA7FC99E000", "9444": "F449374399E000",
                           "9549": "BC86BB4D9A5000", "9818": "8E751D2B9D2000"},
    "Taskbar.View.dll": {"6AA3487A997000": "6AA3487A997000", "6AB99C98983000": "6AB99C98983000"},
    "SystemTray.dll": {"6AA9E58A20A000": "6AA9E58A20A000", "6AA9E607202000": "6AA9E607202000"},
}
# Rules were tuned on "train" only and then frozen; "holdout" pairs were run once after that.
PAIRS = [
    ("taskbar.dll", "9711", "9818", "train"),
    ("taskbar.dll", "9444", "9818", "train"),
    ("twinui.pcshell.dll", "9549", "9818", "train"),
    ("taskbar.dll", "9549", "9711", "holdout"),
    ("taskbar.dll", "9168", "9278", "holdout"),
    ("taskbar.dll", "9278", "9444", "holdout"),
    ("twinui.pcshell.dll", "9168", "9278", "holdout"),
    ("twinui.pcshell.dll", "9278", "9444", "holdout"),
    ("Taskbar.View.dll", "6AA3487A997000", "6AB99C98983000", "holdout"),
    ("SystemTray.dll", "6AA9E58A20A000", "6AA9E607202000", "holdout"),
]


def marks(s):
    return "<" in s or "`" in s or "lambda" in s


def template_name(n):
    """The engine's R3 test: template or lambda marks, ignoring a vftable's own decoration."""
    i = n.find("::`vftable'")
    if i < 0:
        return marks(n)
    s = n.rfind("const ", 0, i)
    cls = n[s + 6 if s >= 0 else 0:i]
    rest = n[i + 11:]
    f = rest.find("{for `")
    iface = rest[f + 6:] if f >= 0 else ""
    return marks(cls) or "<" in iface or "lambda" in iface


def build_dir(cache, module, build):
    """The first cache root that has this build; new downloads go to the first root."""
    for root in cache:
        d = os.path.join(root, "dl", f"{module}_{build}")
        if os.path.exists(os.path.join(d, "rsds.txt")):
            return d
    return os.path.join(cache[0], "dl", f"{module}_{build}")


def truth_file(cache, module, build):
    for root in cache:
        t = os.path.join(root, "truth", f"{module}_{build}.json")
        if os.path.exists(t):
            return t
    return os.path.join(cache[0], "truth", f"{module}_{build}.json")


def pdb_of(d):
    with open(os.path.join(d, "rsds.txt")) as f:
        return f.read().split()[0]


def ensure(cache, module, build, fetch):
    d = build_dir(cache, module, build)
    if not os.path.exists(os.path.join(d, module)) or not os.path.exists(os.path.join(d, "rsds.txt")):
        if not fetch:
            raise SystemExit(f"missing {d} (use --fetch)")
        subprocess.check_call([sys.executable, "-I", os.path.join(HERE, "fetch.py"), os.path.join(cache[0], "dl"),
                               module, build, KEYS[module][build]])
    t = truth_file(cache, module, build)
    if not os.path.exists(t):
        os.makedirs(os.path.dirname(t), exist_ok=True)
        subprocess.check_call([sys.executable, "-I", os.path.join(HERE, "truth.py"), d, module, t])
    return d, t


def run_check(exe, cache, module, ob, nb, wanted, all_names, out):
    do = build_dir(cache, module, ob)
    dn = build_dir(cache, module, nb)
    args = [exe, os.path.join(do, module), os.path.join(do, pdb_of(do)), os.path.join(dn, module), wanted,
            "--json", out]
    if all_names:
        args.append("--all")
    t = time.time()
    subprocess.run(args, check=True, stdout=subprocess.DEVNULL)
    wall = time.time() - t
    with open(out, encoding="utf-8") as f:
        j = json.load(f)
    j["wall_s"] = wall
    return j


def grade(r, truth):
    if r["outcome"] != "ported":
        return r["outcome"]
    t = truth.get(r["name"])
    if t is None:
        return "WRONG-gone"
    return "correct" if t[0] == r["rva"] else "WRONG"


def score_pair(args, module, ob, nb, wanted_file, tag):
    _, tn = ensure(args.cache, module, nb, args.fetch)
    ensure(args.cache, module, ob, args.fetch)
    with open(tn, encoding="utf-8") as f:
        truth = json.load(f)["names"]
    os.makedirs(args.out, exist_ok=True)
    w = run_check(args.port_check, args.cache, module, ob, nb, wanted_file, False,
                  os.path.join(args.out, f"{module}_{ob}_{nb}_wanted.json"))
    p = run_check(args.port_check, args.cache, module, ob, nb, wanted_file, True,
                  os.path.join(args.out, f"{module}_{ob}_{nb}_all.json"))
    rep = {"pair": f"{module} {ob}->{nb}", "set": tag, "functions": [p["functions_old"], p["functions_new"]],
           "engine_ms": p["ms"], "pdb_ms": p["pdb_ms"], "wall_s": round(p["wall_s"], 2)}
    # wanted names
    wrows, wc = [], {}
    present = ported_ok = 0
    for r in w["results"]:
        g = grade(r, truth)
        wc[g] = wc.get(g, 0) + 1
        both = r["old_rva"] is not None and r["name"] in truth
        present += both
        ported_ok += g == "correct"
        if r["old_rva"] is not None:
            wrows.append({"name": r["name"], "grade": g, "rva": r["rva"],
                          "truth": truth.get(r["name"], [None])[0], "evidence": r["evidence"]})
    rep["wanted"] = {"counts": wc, "present_in_both": present, "recall": f"{ported_ok}/{present}", "rows": wrows}
    # population
    buckets = {}
    wrong = []
    for r in p["results"]:
        if r["kind"] not in ("func", "vftable", "global"):
            continue
        b = r["kind"] + ("-template" if template_name(r["name"]) else "")
        g = grade(r, truth)
        c = buckets.setdefault(b, {})
        c[g] = c.get(g, 0) + 1
        if g.startswith("WRONG"):
            wrong.append({"name": r["name"][:200], "bucket": b, "grade": g, "old": hex(r["old_rva"]),
                          "rva": hex(r["rva"]), "truth": hex(truth[r["name"]][0]) if r["name"] in truth else None,
                          "evidence": r["evidence"]})
    rep["population"] = buckets
    rep["population_wrong"] = wrong
    return rep


def summary_line(rep):
    pop = rep["population"]

    def n(bucket, g):
        return pop.get(bucket, {}).get(g, 0)

    nt = sum(n(b, g) for b in ("func", "vftable", "global") for g in ("WRONG", "WRONG-gone"))
    tt = sum(n(b, g) for b in ("func-template", "vftable-template", "global-template") for g in ("WRONG", "WRONG-gone"))
    ok_nt = sum(n(b, "correct") for b in ("func", "vftable", "global"))
    ok_t = sum(n(b, "correct") for b in ("func-template", "vftable-template", "global-template"))
    wc = rep["wanted"]["counts"]
    return (f"{rep['set']:8s} {rep['pair']:46s} wanted WRONG {wc.get('WRONG', 0) + wc.get('WRONG-gone', 0)} "
            f"recall {rep['wanted']['recall']:>6s} | population non-template {ok_nt} ok / {nt} WRONG, "
            f"template {ok_t} ok / {tt} WRONG | funcs {rep['functions'][0]}/{rep['functions'][1]} "
            f"engine {rep['engine_ms']} ms, PDB {rep['pdb_ms']} ms")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port-check", required=True)
    ap.add_argument("--cache", required=True, help="cache roots separated by ';', the first gets new files")
    ap.add_argument("--lists", nargs="*", default=[])
    ap.add_argument("--pairs", default="train")
    ap.add_argument("--fetch", action="store_true")
    ap.add_argument("--out", default=None)
    args = ap.parse_args()
    args.cache = [c for c in args.cache.split(";") if c]
    args.out = args.out or os.path.join(args.cache[0], "out")
    if args.pairs in ("train", "holdout", "all"):
        pairs = [p for p in PAIRS if args.pairs == "all" or p[3] == args.pairs]
    else:
        pairs = [tuple(x.split(":")) + ("adhoc",) for x in args.pairs.split(",")]
    names = []
    for l in args.lists:
        with open(l, encoding="utf-8-sig") as f:
            for n in f.read().splitlines():
                if n and n not in names:
                    names.append(n)
    os.makedirs(args.out, exist_ok=True)
    wanted_file = os.path.join(args.out, "wanted.txt")
    with open(wanted_file, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(names) + "\n")
    reps = []
    for module, ob, nb, tag in pairs:
        rep = score_pair(args, module, ob, nb, wanted_file, tag)
        reps.append(rep)
        print(summary_line(rep), flush=True)
        for r in rep["wanted"]["rows"]:
            if r["grade"].startswith("WRONG"):
                print("   WANTED", r)
    with open(os.path.join(args.out, f"backtest_{args.pairs if len(args.pairs) < 20 else 'adhoc'}.json"), "w",
              encoding="utf-8") as f:
        json.dump(reps, f, indent=1)


if __name__ == "__main__":
    main()
