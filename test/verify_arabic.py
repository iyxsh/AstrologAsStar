#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""verify_arabic.py — A5 阿拉伯点引擎 python 二次 oracle 交叉对拍。

独立实现原版公式 DSL（与 arabic_parts.cpp 同语义）：
  读 astrolog32-cli @0203 行星/宫位行 → 复算 177 点 → 与 astrolog32_unit_arabic
  打印的引擎 rPart 逐点比较（|err|<=1e-7 度）。

用法:
  python test/verify_arabic.py [cli.exe] [unit_arabic.exe]
"""
import os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLI  = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
    ROOT, "astroproject", "bin", "windows", "Debug", "astrolog32-cli.exe")
UNIT = sys.argv[2] if len(sys.argv) > 2 else os.path.join(
    ROOT, "astroproject", "bin", "windows", "Debug", "astrolog32_unit_arabic.exe")

SIGNOFF = {"Ari":0,"Tau":30,"Gem":60,"Can":90,"Leo":120,"Vir":150,
           "Lib":180,"Sco":210,"Sag":240,"Cap":270,"Aqu":300,"Pis":330}
# 行星/交点行名 → 对象号；宫头行名 → 宫号（1..12）
PLAN = {"Sun":1,"Moo":2,"Mer":3,"Ven":4,"Mar":5,"Jup":6,"Sat":7,
        "Ura":8,"Nep":9,"Plu":10,"NoN":16,"For":19}
CUSP = {"Asc":1,"2nd":2,"3rd":3,"IC":4,"5th":5,"6th":6,"Des":7,
        "8th":8,"9th":9,"Mid":10,"11t":11,"12t":12}
RULES = [0,5,4,3,2,1,3,4,5,6,7,7,6]      # 星座号→守护星（planet.cpp rules[]）
# 对象号 → 星座号（Z2Sign: (int)deg/30+1，直接对度数用）
ASC, SUN, MOO = 22, 1, 2

CHARTS = [("bj", ["-qb","7","4","1958","12:01","0","8","-116:23","39:54"]),
          ("la", ["-qb","7","20","1969","20:17","1","-7","118:15","34:03"])]

YF = re.compile(r"/YF\s+(\S+)\s+(\d+)\s+([A-Za-z]{3})\s+([\d.]+),\s*"
                r"(-?\d+)\s+([\d.]+),\s*(-?[\d.]+)\s+(-?[\d.]+)")

def chart_state(cli, args):
    p = subprocess.run([cli] + args + ["--text"], capture_output=True, timeout=120)
    assert p.returncode == 0, p.stderr[:300]
    lon = {}               # 对象号 → 黄经（度）
    cusp = [0.0] * 13      # cusp[h] h=1..12
    for raw in p.stdout.decode("utf-8-sig", "replace").splitlines():
        m = YF.match(raw.rstrip("\r"))
        if not m:
            continue
        nm, deg, sign, fr = m.group(1), int(m.group(2)), m.group(3), float(m.group(4))
        v = SIGNOFF[sign] + deg + fr / 60.0
        if nm in PLAN:
            lon[PLAN[nm]] = v
        elif nm in CUSP:
            h = CUSP[nm]
            cusp[h] = v
            lon[21 + h] = v          # oAsc=22=cusp1 … oCusp12th=33=cusp12
    assert 22 in lon and all(cusp[h] for h in range(1, 13)), "chart parse failed"
    # 对象落宫：cusp 升序 + 0°Ari 环绕线性化（cusp[h]<cusp[1] 时视为 +360°）
    def house_no(deg):
        base = cusp[1]
        y = deg
        if y < base:
            y += 360.0
        sel = 1
        for h in range(1, 13):
            v = cusp[h]
            if v < base:
                v += 360.0
            if v <= y:
                sel = h
        return sel
    lon["_house"] = house_no
    lon["_cusp"] = cusp
    return lon

def compute_all(lon, day):
    cusp, house_no = lon["_cusp"], lon["_house"]
    rp = [-360.0] * 177
    for i in range(177):
        f = g_form[i]
        rBit = [0.0, 0.0, 0.0]
        ok = True
        for j in range(3):
            g = f[j * 3:j * 3 + 3]
            mid = g[1]
            if mid == " ":
                k = ASC
            elif mid == "F":
                k = 0
            elif mid == "S":
                k = -1
            else:
                k = (ord(mid) - 48) * 10 + (ord(g[2]) - 48) if g[2] != " " else (ord(mid) - 48)
            mod = g[0]
            if mod == "h":
                rCur = cusp[k]
            elif mod == "r":
                rCur = lon[RULES[(int(cusp[k]) // 30) % 12 + 1]]
            elif mod == "j":
                rCur = cusp[k] + 10.0
            elif mod == "H":
                rCur = cusp[house_no(lon[k])]
            elif mod == "R":
                rCur = lon[RULES[(int(cusp[house_no(lon[k])]) // 30) % 12 + 1]]
            elif mod == "D":
                rCur = lon[RULES[(int(lon[k]) // 30) % 12 + 1]]
            elif "0" <= mod <= "3":
                rCur = float((ord(mod) - 48) * 100 + k)
            else:
                if k < 1:
                    if rp[-k] < 0.0:
                        ok = False; break
                    rCur = rp[-k]
                else:
                    rCur = lon.get(k)
                    if rCur is None:
                        ok = False; break
            rBit[j] = rCur
        if not ok:
            continue
        rCur = rBit[1] - rBit[2]
        if f[9] == "F":
            # (nArabicNight<0 && day) || (nArabicNight==0 && !day) — nArabicNight 默认 0
            if day == 0:
                rCur = -rCur
        rp[i] = (rCur + rBit[0]) % 360.0
    return rp

def load_table():
    rows = []
    for line in open(os.path.join(ROOT, "astroproject", "src", "core",
                                  "arabic_parts_data.inc"), encoding="utf-8"):
        m = re.match(r'\s*\{\s*L"(.*)",\s*L"(.*)"\s*\},', line)
        if m:
            rows.append(m.group(1))
    assert len(rows) == 177
    return rows

def parse_engine(text):
    blocks = []            # [{day, parts:{i:lon}}]
    cur = None
    for raw in text.splitlines():
        line = raw.strip()
        if line.startswith("DAY "):
            if cur: blocks.append(cur)
            cur = {"day": int(line[4]), "parts": {}}
        elif line.startswith("P ") and cur is not None:
            _, i, v = line.split()
            cur["parts"][int(i)] = float(v)
    if cur: blocks.append(cur)
    return blocks

def main():
    global g_form
    g_form = load_table()
    eng = parse_engine(open(sys.argv[3], encoding="utf-8").read() if len(sys.argv) > 3
                       else subprocess.run([UNIT], capture_output=True, timeout=120)
                       .stdout.decode("utf-8", "replace"))
    assert len(eng) == len(CHARTS), (len(eng), len(CHARTS))
    worst = 0.0
    nbad = 0
    ncmp = 0
    for (cid, args), e in zip(CHARTS, eng):
        lon = chart_state(CLI, args)
        py = compute_all(lon, e["day"])
        nbad_chart = 0
        nvalid = 0
        for i, pv in enumerate(py):
            if pv < 0.0:
                continue
            nvalid += 1
            ev = e["parts"].get(i)
            if ev is None:
                print("[%s] engine missing part %d" % (cid, i)); nbad += 1; continue
            d = abs(pv - ev)
            worst = max(worst, d); ncmp += 1
            if d > 1e-7:
                nbad += 1; nbad_chart += 1
                if nbad <= 20:
                    print("[%s] part %2d %-28s py=%.9f eng=%.9f err=%.2e"
                          % (cid, i, g_names[i], pv, ev, d))
        print("[%s] day=%d valid=%d bad=%d" % (cid, e["day"], nvalid, nbad_chart))
    print("TOTAL: compared=%d  worst_err=%.3e deg  bad=%d" % (ncmp, worst, nbad))
    return 0 if nbad == 0 else 1

if __name__ == "__main__":
    # also load names for messages
    g_names = []
    for line in open(os.path.join(ROOT, "astroproject", "src", "core",
                                  "arabic_parts_data.inc"), encoding="utf-8"):
        m = re.match(r'\s*\{\s*L"(.*)",\s*L"(.*)"\s*\},', line)
        if m:
            g_names.append(m.group(2))
    sys.exit(main())
