# SPDX-License-Identifier: GPL-3.0-only
"""Backtest for the symbol porter: runs port-check over pairs of published builds and
scores every answer against the NEW build's real PDB (dev only).

Usage: backtest.py --port-check <port-check.exe> --cache <dir>[;<dir>...] [--lists [<add-on>=]symbols.txt ...]
                   [--pairs train|holdout|fresh|all|<set>,<set>|<module>:<old>:<new>,...] [--fetch] [--out <dir>]

Each list is one add-on's symbols.txt, named by <add-on>= (else by its folder), and
<add-on>:<module>,<module>= limits its per add-on numbers to the modules it hooks; a
symbols-meta.txt beside it says which names are required.

The cache holds dl/<module>_<build>/<module> and its PDB (fetch.py layout, --fetch downloads
missing ones from the Microsoft symbol server by the keys below) and truth/<module>_<build>.json
(truth.py, made on demand). Keys are %08X TimeDateStamp + %X SizeOfImage, taken from Winbindex
data (winbindex.m417z.com, GPL-3.0) for the numbered builds.

Scoring, per name:
  correct    ported to the RVA the NEW PDB gives for that name (its first address)
  WRONG      ported anywhere else, or ported while the name no longer exists in NEW
             ("gone"; counted separately too)
  unported   ambiguous / not found / absent
Recall counts wanted names that exist in both builds. Per add-on it also counts the
required names, and "on" says whether the host would write the module: every required
name of the add-on that the OLD PDB has is ported, after the host's rule that names
apart in OLD may not share a NEW address (host/ported.cpp BuildPortedOffsets).

MIDL names enum types after their place in the IDL (__MIDL___MIDL_itf_<x>_<nnnn>_<nnnn>),
so a rebuild can renumber them. "normalised" scoring counts a port as correct when the
name is gone in NEW only because those numbers changed: exactly one name that is new in
NEW equals it with the numbers masked, and it sits at the ported RVA. Both the raw and
the normalised numbers are reported.
"""
import argparse, json, os, re, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))

KEYS = {
    "taskbar.dll": {"8655": "22DDC5D530E000", "8875": "A4EA993A2FC000", "8972": "60900ECC302000",
                    "9168": "AF54E51D302000", "9278": "32859388304000", "9444": "8DBF8CD1304000",
                    "9549": "E8E4B3D1308000", "9711": "769B877B308000", "9818": "617E422B31A000"},
    "twinui.pcshell.dll": {"8655": "70E71F6E95F000", "8875": "15E71BEE996000", "8972": "D9344DA3999000",
                           "9168": "1C8AB037999000", "9278": "C5AFA7FC99E000", "9444": "F449374399E000",
                           "9549": "BC86BB4D9A5000", "9818": "8E751D2B9D2000"},
    "Taskbar.View.dll": {"6AA3487A997000": "6AA3487A997000", "6AB99C98983000": "6AB99C98983000"},
    "SystemTray.dll": {"6AA9E58A20A000": "6AA9E58A20A000", "6AA9E607202000": "6AA9E607202000"},
}
# Rules were tuned on "train" only and then frozen; "holdout" pairs were run once after that.
# The last three "train" pairs were first run at integration, where they showed wrong
# answers outside the wanted names; the rules that fix those were tuned on them. "fresh"
# pairs (adjacent months before 9168, never used before) were run with those rules frozen.
# No rule was tuned on them; one candidate rule (a lone vftable slot proving a name) was
# dropped there, as it added wrong answers on twinui 8655->8875.
PAIRS = [
    ("taskbar.dll", "9711", "9818", "train"),
    ("taskbar.dll", "9444", "9818", "train"),
    ("twinui.pcshell.dll", "9549", "9818", "train"),
    ("twinui.pcshell.dll", "9444", "9549", "train"),
    ("taskbar.dll", "9444", "9549", "train"),
    ("taskbar.dll", "9444", "9711", "train"),
    ("taskbar.dll", "9549", "9711", "holdout"),
    ("taskbar.dll", "9168", "9278", "holdout"),
    ("taskbar.dll", "9278", "9444", "holdout"),
    ("twinui.pcshell.dll", "9168", "9278", "holdout"),
    ("twinui.pcshell.dll", "9278", "9444", "holdout"),
    ("Taskbar.View.dll", "6AA3487A997000", "6AB99C98983000", "holdout"),
    ("SystemTray.dll", "6AA9E58A20A000", "6AA9E607202000", "holdout"),
    ("taskbar.dll", "8972", "9168", "fresh"),
    ("taskbar.dll", "8655", "8875", "fresh"),
    ("taskbar.dll", "8875", "8972", "fresh"),
    ("twinui.pcshell.dll", "8972", "9168", "fresh"),
    ("twinui.pcshell.dll", "8655", "8875", "fresh"),
    ("twinui.pcshell.dll", "8875", "8972", "fresh"),
]

MIDL = re.compile(r"(__MIDL___MIDL_itf_\w*?)((?:_\d{4})+)(?!\w)")


def midl_mask(n):
    return MIDL.sub(lambda m: m.group(1) + re.sub(r"\d", "0", m.group(2)), n)


def marks(s):
    return "<" in s or "`" in s or "lambda" in s


def template_name(n):
    """The engine's R3 test: template or lambda marks, ignoring a vftable's own decoration."""
    i = n.find("::`vftable'")
    if i < 0:
        return marks(n)
    # only the leading "const ": a later one belongs to a template argument
    cls = n[6 if n.startswith("const ") else 0:i]
    rest = n[i + 11:]
    f = rest.find("{for `")
    iface = rest[f + 6:] if f >= 0 else ""
    return marks(cls) or "<" in iface or "lambda" in iface


# WIL's per-feature names, numbered or named (Feature_61276672__descriptor,
# Feature_SystemLightTheme__descriptor, Feature_<id>__private_featureState, ...): the
# engine must never port them (NeverPorted), so any that comes out ported is reported.
WIL_FEATURE = re.compile(r"(?<![A-Za-z0-9_])Feature_[A-Za-z0-9_]+?(__descriptor|__private_[A-Za-z0-9_]+|_logged_traits)(?![A-Za-z0-9_])")


def wil_feature_name(n):
    return bool(WIL_FEATURE.search(n)) or "wil::details::FeatureImpl<" in n or "__WilFeatureTraits_" in n


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


class Renamed:
    """NEW names that OLD does not have, by their MIDL-masked form."""

    def __init__(self, old_truth, truth):
        self.by_mask = {}
        for n, rvas in truth.items():
            if "__MIDL___MIDL_itf_" in n and n not in old_truth:
                self.by_mask.setdefault(midl_mask(n), []).append(rvas[0])

    def same(self, name, rva):
        if "__MIDL___MIDL_itf_" not in name:
            return False
        c = self.by_mask.get(midl_mask(name), [])
        return len(c) == 1 and c[0] == rva


def grade(r, truth, renamed=None):
    """correct / WRONG / WRONG-gone / an unported outcome; with renamed, a port that is
    WRONG-gone only by MIDL numbering is "correct-midl"."""
    if r["outcome"] != "ported":
        return r["outcome"]
    t = truth.get(r["name"])
    if t is None:
        return "correct-midl" if renamed and renamed.same(r["name"], r["rva"]) else "WRONG-gone"
    return "correct" if t[0] == r["rva"] else "WRONG"


def addon_recall(results, truth, addons, module):
    """Per add-on: wanted and required names in both builds, how many are correct, and
    whether the host would write the module (every required name of the OLD PDB ported)."""
    by_name = {r["name"]: r for r in results}
    out = {}
    for addon, (names, required, modules) in addons.items():
        if modules and module.lower() not in modules:
            continue
        rows = [by_name[n] for n in names if n in by_name and by_name[n]["old_rva"] is not None]
        if not rows:
            continue
        ported = {r["name"]: r for r in rows if r["outcome"] == "ported"}
        # the host's rule: names apart in OLD lose a NEW address they share
        at = {}
        for r in ported.values():
            at.setdefault(r["rva"], []).append(r)
        for rs in at.values():
            if len(rs) > 1 and len({r["old_rva"] for r in rs}) > 1:
                for r in rs:
                    ported.pop(r["name"], None)
        both = [r for r in rows if r["name"] in truth]
        ok = [r for r in both if r["name"] in ported and truth[r["name"]][0] == r["rva"]]
        req_both = [r for r in both if required.get(r["name"], True)]
        req_ok = [r for r in ok if required.get(r["name"], True)]
        req_old = [r for r in rows if required.get(r["name"], True)]
        out[addon] = {"recall": f"{len(ok)}/{len(both)}", "required": f"{len(req_ok)}/{len(req_both)}",
                      "on": all(r["name"] in ported for r in req_old),
                      "missing_required": [r["name"] for r in req_old if r["name"] not in ported]}
    return out


def score_pair(args, module, ob, nb, wanted_file, tag):
    _, tn = ensure(args.cache, module, nb, args.fetch)
    _, to = ensure(args.cache, module, ob, args.fetch)
    with open(tn, encoding="utf-8") as f:
        truth = json.load(f)["names"]
    with open(to, encoding="utf-8") as f:
        renamed = Renamed(json.load(f)["names"], truth)
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
    rep["addons"] = addon_recall(w["results"], truth, args.addons, module)
    # population
    buckets = {}
    wrong = []
    for r in p["results"]:
        if r["kind"] not in ("func", "vftable", "global"):
            continue
        b = r["kind"] + ("-template" if template_name(r["name"]) else "")
        g = grade(r, truth, renamed)
        c = buckets.setdefault(b, {})
        c[g] = c.get(g, 0) + 1
        if g.startswith("WRONG") or g == "correct-midl":
            wrong.append({"name": r["name"][:200], "bucket": b, "grade": g, "old": hex(r["old_rva"]),
                          "rva": hex(r["rva"]), "truth": hex(truth[r["name"]][0]) if r["name"] in truth else None,
                          "evidence": r["evidence"]})
    rep["population"] = buckets
    rep["population_wrong"] = wrong
    rep["wil_ported"] = [r["name"] for r in p["results"] if r["outcome"] == "ported" and wil_feature_name(r["name"])]
    return rep


def summary_line(rep):
    pop = rep["population"]

    def n(bucket, g):
        return pop.get(bucket, {}).get(g, 0)

    nts, ts = ("func", "vftable", "global"), ("func-template", "vftable-template", "global-template")
    midl_nt = sum(n(b, "correct-midl") for b in nts)
    nt = sum(n(b, g) for b in nts for g in ("WRONG", "WRONG-gone")) + midl_nt
    tt = sum(n(b, g) for b in ts for g in ("WRONG", "WRONG-gone", "correct-midl"))
    tt_norm = sum(n(b, g) for b in ts for g in ("WRONG", "WRONG-gone"))
    ok_nt = sum(n(b, "correct") for b in nts)
    ok_t = sum(n(b, "correct") for b in ts)
    wc = rep["wanted"]["counts"]
    addons = " ".join(f"{a}:{v['recall']},req {v['required']}{'' if v['on'] else ' OFF'}"
                      for a, v in rep["addons"].items())
    return (f"{rep['set']:8s} {rep['pair']:46s} wanted WRONG {wc.get('WRONG', 0) + wc.get('WRONG-gone', 0)} "
            f"recall {rep['wanted']['recall']:>6s} | non-template {ok_nt} ok / {nt} WRONG raw, "
            f"{nt - midl_nt} normalised | template {ok_t} ok / {tt} WRONG raw, {tt_norm} normalised | "
            f"engine {rep['engine_ms']} ms | {addons}"
            + (f" | WIL feature names ported {len(rep['wil_ported'])}" if rep.get("wil_ported") else ""))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port-check", required=True)
    ap.add_argument("--cache", required=True, help="cache roots separated by ';', the first gets new files")
    ap.add_argument("--lists", nargs="*", default=[])
    ap.add_argument("--pairs", default="train", help="sets (train, holdout, fresh, all; comma separated) or pairs")
    ap.add_argument("--fetch", action="store_true")
    ap.add_argument("--out", default=None)
    args = ap.parse_args()
    args.cache = [c for c in args.cache.split(";") if c]
    args.out = args.out or os.path.join(args.cache[0], "out")
    sets = args.pairs.split(",")
    if all(x in ("train", "holdout", "fresh", "all") for x in sets):
        pairs = [p for p in PAIRS if "all" in sets or p[3] in sets]
    else:
        pairs = [tuple(x.split(":")) + ("adhoc",) for x in sets]
    names = []
    args.addons = {}
    for spec in args.lists:
        addon, _, l = spec.rpartition("=")
        addon, _, mods = addon.partition(":")
        addon = addon or os.path.basename(os.path.dirname(os.path.abspath(l)))
        modules = {m.lower() for m in mods.split(",") if m}
        with open(l, encoding="utf-8-sig") as f:
            own = [n for n in f.read().splitlines() if n]
        required = {}
        meta = os.path.join(os.path.dirname(os.path.abspath(l)), "symbols-meta.txt")
        if os.path.exists(meta):
            with open(meta, encoding="utf-8-sig") as f:
                for line in f.read().splitlines():
                    cols = line.split("\t")
                    if len(cols) >= 2:
                        required[cols[0]] = cols[-1] == "req"
        args.addons[addon] = (own, required, modules)
        for n in own:
            if n not in names:
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
        for a, v in rep["addons"].items():
            for n in v["missing_required"]:
                print(f"   {a} required, not ported: {n[:150]}")
        for n in rep["wil_ported"]:
            print(f"   WIL feature name ported: {n[:150]}")
    label = args.pairs.replace(",", "+") if len(args.pairs) < 30 else "adhoc"
    with open(os.path.join(args.out, f"backtest_{label}.json"), "w", encoding="utf-8") as f:
        json.dump(reps, f, indent=1)


if __name__ == "__main__":
    main()
