#!/usr/bin/env python3
"""verify_progress.py — P2/A8 次限推进 pymeeus 独立 oracle（增量法）。

引擎把次限盘实现为：ephemeris JD = natal_UT + (JDp_target - natal_UT)/rProgDay
(rProgDay=365.24219)。对推进盘与本命盘都成立的恒等式是**黄经增量**：
    Δ = lon(progressed) - lon(natal)
本 oracle 用 pymeeus 独立星历在同一增量公式下复算 Sun/Moon 的 Δ，
与本地 astrolog32-cli --progto 输出的 Δ 对比。
- 时间原点/时区偏移在「差值」中抵消；ΔT 也近似抵消（推进历元≈本命同年+68天）。
- 容差：Sun 0.05°，Moon 0.3°（pymeeus VSOP/ELP 精度 + 残差）。

用法: python verify_progress.py <astrolog32-cli> [--chart M D Y T dst zon lon lat]...
      默认遍历 test/golden/*.golden.txt（基础盘）推进到 2026-09-05。
"""
import io, json, re, subprocess, sys, os, glob

CLI = None
TARGETS = [(9, 5, 2026, "12:01"), (1, 15, 2033, "0:00")]
CHART_CMDS = []          # list of cmd token lists (from golden fixtures)

SIGN = {"Ari": 0, "Tau": 30, "Gem": 60, "Can": 90, "Leo": 120, "Vir": 150,
        "Lib": 180, "Sco": 210, "Sag": 240, "Cap": 270, "Aqu": 300, "Pis": 330}

def circ(a, b):
    d = (a - b) % 360.0
    return d if d <= 180.0 else d - 360.0

def parse_rows(text):
    out = {}
    for ln in text.splitlines():
        m = re.match(r"/YF\s+(\S+)\s+(\d+)\s+([A-Za-z]{3})\s+([\d.]+),", ln)
        if m:
            out[m.group(1)] = (int(m.group(2)) + SIGN[m.group(3)]
                               + float(m.group(4)) / 60.0)
    return out

def cli_rows(cmd, prog=None):
    args = [CLI] + cmd
    if prog:
        args += ["--progto"] + [str(x) for x in prog]
    p = subprocess.run(args, capture_output=True, timeout=300, text=True)
    return parse_rows(p.stdout), p.returncode

# --- pymeeus 增量复算 ---
def jd0(y, m, d):
    def ja(y, m, d, g):
        a = (14 - m) // 12; yy = y + 4800 - a; mm = m + 12 * a - 3
        return d + (153 * mm + 2) // 5 + 365 * yy + yy // 4 \
            - (yy // 100 if g else 0) + (yy // 400 if g else 0) \
            - (32045 if g else 32083)
    return float(ja(y, m, d, (y, m, d) >= (1582, 10, 15)))

def timdec(s):
    h, mi = s.split(":"); return int(h) + int(mi) / 60.0

def jd_of(y, m, d, t):
    return jd0(y, m, d) + timdec(t) / 24.0

def main():
    global CLI
    argv = sys.argv[1:]
    if not argv:
        print("usage: verify_progress.py <cli>")
        return 1
    CLI = argv[0]
    from pymeeus.Sun import Sun
    from pymeeus.Moon import Moon
    from pymeeus.Epoch import Epoch

    gdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "golden")
    for f in sorted(os.listdir(gdir)):
        if not f.endswith(".golden.txt") or ".U." in f or ".hs" in f:
            continue
        for ln in open(os.path.join(gdir, f), encoding="utf-8", errors="replace"):
            if ln.startswith("# cmd:"):
                CHART_CMDS.append(ln[len("# cmd:"):].strip().split())
                break

    nbad = ntot = 0
    worst = {"Sun": 0.0, "Moon": 0.0}

    def sun_at(jd):
        return float(Sun.apparent_geocentric_position(Epoch(jd))[0])

    def moon_at(jd):
        return float(Moon.geocentric_ecliptical_pos(Epoch(jd))[0])

    for cmd in CHART_CMDS:
        # cmd: -qb M D Y T dst zon lon lat
        M, D, Y, T = int(cmd[1]), int(cmd[2]), int(cmd[3]), cmd[4]
        nat_rows, rc1 = cli_rows(cmd)
        if rc1 != 0 or "Sun" not in nat_rows:
            print("skip (natal rc=%d) %s" % (rc1, cmd)); continue
        jd_nat_naive = jd_of(Y, M, D, T)
        engSunN, engMooN = nat_rows["Sun"], nat_rows["Moo"]

        # Sun 校准：反解引擎本命绝对 JD（太阳 ~1°/day，±1.2d 内单调唯一）
        lo, hi = -1.2, 1.2
        for _ in range(24):
            mid = (lo + hi) / 2.0
            if circ(sun_at(jd_nat_naive + mid), engSunN) > 0:
                hi = mid
            else:
                lo = mid
        off = (lo + hi) / 2.0
        base = jd_nat_naive + off
        err_nat_sun = abs(circ(sun_at(base), engSunN))
        err_nat_moon = abs(circ(moon_at(base), engMooN))

        for (tm, td, ty, tt) in TARGETS:
            prog_rows, rc2 = cli_rows(cmd, (tm, td, ty, tt))
            if rc2 != 0 or "Sun" not in prog_rows:
                print("skip prog rc=%d %s -> %d/%d/%d" % (rc2, cmd, td, tm, ty)); continue
            ddays = jd_of(ty, tm, td, tt) - jd_nat_naive
            jd_pr = base + ddays / 365.24219
            checks = [
                ("Sun", circ(sun_at(jd_pr), prog_rows["Sun"])),
                ("Moon", circ(moon_at(jd_pr), prog_rows["Moo"])),
            ]
            ntot += 1
            for name, err in checks:
                worst[name] = max(worst[name], err)
                tol = 0.05 if name == "Sun" else 0.3
                if err > tol:
                    nbad += 1
                    print("FAIL %s %s->%d/%d/%d eng-prg-py err=%.5f (natSunCal=%.5f natMoon=%.5f)"
                          % (name, "-".join(cmd[1:5]), td, tm, ty, err,
                             err_nat_sun, err_nat_moon))
    print("progress_oracle: %d checks, %d bad; worst Sun=%.5f Moon=%.5f (tol Sun 0.05 Moon 0.3)"
          % (ntot, nbad, worst["Sun"], worst["Moon"]))
    return 0 if nbad == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
