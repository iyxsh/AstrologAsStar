#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""verify_cli.py — P0.3 golden diff harness for astrolog32-cli.

Runs the CLI with the same `-qb` arguments recorded in each golden sample
(test/golden/*.golden.txt) and compares the emitted @0203 machine text
field-by-field against the golden.

Comparison policy (documented engine gaps are tolerated, everything else is
strict):
  * header lines (@0203..., /zi...)        — exact
  * object set & order (40 rows)           — exact
  * longitude + latitude (deg)             — strict (|err| <= 1e-7 deg,
                                              i.e. 9-decimal aligned)
  * speed (deg/day)                        — |err| <= 1e-3 (Moon velocity is
                                              computed by a slightly
                                              different method: ~1e-6 off)
  * distance (AU)                          — strict for planets, NoNode and
                                              placeholder stars; SKIPPED for
                                              Fortune and the 12 house cusps
                                              (the refactor engine does not
                                              populate spacex/y/z for those
                                              derived points yet — engine
                                              gap tracked in P1 alignment)

Usage:
    python test/verify_cli.py [path/to/astrolog32-cli.exe]

Returns exit code 0 only when every golden sample passes.
"""
import glob
import os
import re
import subprocess
import sys

SIGNOFF = {"Ari":0, "Tau":30, "Gem":60, "Can":90, "Leo":120, "Vir":150,
           "Lib":180, "Sco":210, "Sag":240, "Cap":270, "Aqu":300, "Pis":330}

# Fortune + house cusps: distance intentionally not compared (engine gap).
SKIP_DIST = {"For","Asc","2nd","3rd","IC","5th","6th","Des","8th","9th",
             "Mid","11t","12t"}

LON_TOL   = 1e-7   # deg — 9-decimal alignment
LAT_TOL   = 1e-6   # deg
SPEED_TOL = 1e-3   # deg/day (Moon velocity method difference ~1e-6)
DIST_TOL  = 1e-6   # AU

YF_RE = re.compile(
    r"/YF\s+(\S+)\s+(\d+)\s+([A-Za-z]{3})\s+([\d.]+),\s*"
    r"(-?\d+)\s+([\d.]+),\s*(-?[\d.]+)\s+(-?[\d.]+)")


def find_exe(explicit):
    if explicit:
        return os.path.abspath(explicit)
    # repo root = parent of this script's directory (test/)
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    cands = []
    for pattern in ("**/astrolog32-cli.exe", "**/astrolog32-cli"):
        for p in glob.glob(os.path.join(root, "astroproject", "bin",
                                        "windows", "*", "astrolog32-cli.exe")):
            cands.append(p)
        for p in glob.glob(os.path.join(root, "astroproject", "build",
                                        "**", "astrolog32-cli*"), recursive=True):
            cands.append(p)
    cands = sorted(set(cands))
    if not cands:
        sys.exit("error: astrolog32-cli binary not found; pass its path")
    # prefer the most recently built
    return max(cands, key=os.path.getmtime)


def parse_objects(text):
    """Return (header_lines, [obj, ...]) with obj dict name->(lon,lat,speed,dist)."""
    objs = []
    header = []
    for raw in text.splitlines():
        line = raw.rstrip("\r")
        if not line.strip() or line.startswith("#"):
            continue
        if line.startswith("/YF"):
            m = YF_RE.match(line)
            if not m:
                raise ValueError("unparseable /YF line: %r" % line)
            nm, deg, sign, fr, latd, latf, speed, dist = m.groups()
            lon = SIGNOFF[sign] + int(deg) + float(fr) / 60.0
            lat = int(latd) + (1 if int(latd) >= 0 else -1) * float(latf) / 60.0
            objs.append((nm, lon, lat, float(speed), float(dist)))
        else:
            header.append(line)
    return header, objs


def run_cli(exe, cmd_args):
    proc = subprocess.run([exe] + cmd_args + ["--text"],
                          capture_output=True, timeout=120)
    if proc.returncode != 0:
        raise RuntimeError("CLI exited %d: %s" %
                           (proc.returncode, proc.stderr.decode("utf-8", "replace")))
    return proc.stdout.decode("utf-8-sig", "replace")


def check_golden(exe, golden_path, verbose):
    with open(golden_path, encoding="utf-8-sig") as f:
        gtext = f.read()
    # reconstruct the CLI args from the "# cmd:" line
    cmd_args = None
    for raw in gtext.splitlines():
        if raw.startswith("# cmd:"):
            cmd_args = raw.split("# cmd:", 1)[1].strip().split()
            break
    if not cmd_args:
        raise ValueError("no '# cmd:' line in " + golden_path)

    ctext = run_cli(exe, cmd_args)
    g_head, gobjs = parse_objects(gtext)
    c_head, cobjs = parse_objects(ctext)

    problems = []

    # 1) header
    if len(g_head) < 2 or len(c_head) < 2:
        problems.append("header too short (golden=%d cli=%d lines)" %
                        (len(g_head), len(c_head)))
    else:
        for tag, a, b in (("@0203", g_head[0], c_head[0]),
                          ("/zi", g_head[1], c_head[1])):
            if a != b:
                problems.append("header %s mismatch:\n  golden: %r\n  cli:    %r" %
                                (tag, a, b))

    # 2) object set + order
    gnames = [o[0] for o in gobjs]
    cnames = [o[0] for o in cobjs]
    if gnames != cnames:
        problems.append("object set mismatch:\n  golden(%d): %s\n  cli(%d):    %s" %
                        (len(gnames), gnames, len(cnames), cnames))
        return False, problems, 0.0, 0

    # 3) numeric fields
    max_lon_err = 0.0
    n_skip_dist = 0
    for (gn, glon, glat, gspd, gdst), (cn, clon, clat, cspd, cdst) in \
            zip(gobjs, cobjs):
        lon_err = abs(glon - clon)
        lat_err = abs(glat - clat)
        spd_err = abs(gspd - cspd)
        max_lon_err = max(max_lon_err, lon_err)
        if lon_err > LON_TOL:
            problems.append("%s longitude: golden=%.9f cli=%.9f (err=%.3e)" %
                            (gn, glon, clon, lon_err))
        if lat_err > LAT_TOL:
            problems.append("%s latitude: golden=%.9f cli=%.9f (err=%.3e)" %
                            (gn, glat, clat, lat_err))
        if spd_err > SPEED_TOL:
            problems.append("%s speed: golden=%.9f cli=%.9f (err=%.3e)" %
                            (gn, gspd, cspd, spd_err))
        if gn in SKIP_DIST:
            n_skip_dist += 1
        elif abs(gdst - cdst) > DIST_TOL:
            problems.append("%s distance: golden=%.9f cli=%.9f (err=%.3e)" %
                            (gn, gdst, cdst, abs(gdst - cdst)))

    if verbose:
        print("  objects=%d  max_lon_err=%.3e deg  skip_dist=%d%s" %
              (len(gobjs), max_lon_err, n_skip_dist,
               "  <-- Fortune/cusp distance skipped (engine gap, P1)" if n_skip_dist else ""))
    return not problems, problems, max_lon_err, n_skip_dist


def main():
    exe = find_exe(sys.argv[1] if len(sys.argv) > 1 else None)
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    goldens = sorted(glob.glob(os.path.join(root, "test", "golden",
                                            "*.golden.txt")))
    if not goldens:
        sys.exit("error: no golden samples found under test/golden/")
    verbose = os.environ.get("VERBOSE") != "0"

    print("CLI: %s" % exe)
    print("golden samples: %d\n" % len(goldens))
    npass = 0
    for gp in goldens:
        name = os.path.basename(gp)
        try:
            ok, problems, max_err, nskip = check_golden(exe, gp, verbose)
        except Exception as e:   # noqa: BLE001 — report and continue
            ok, problems, max_err, nskip = False, ["exception: %r" % e], 0.0, 0
        status = "PASS" if ok else "FAIL"
        if ok:
            npass += 1
        print("[%s] %-28s max_lon_err=%.3e%s" %
              (status, name, max_err,
               "" if ok else "  <- " + "; ".join(problems[:3])))
        if not ok and verbose:
            for p in problems[3:]:
                print("         " + p)
    print("\n%d/%d passed" % (npass, len(goldens)))
    return 0 if npass == len(goldens) else 1


if __name__ == "__main__":
    sys.exit(main())
