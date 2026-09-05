#!/usr/bin/env python3
"""verify_grands.py — A7① 真实盘格局计数对拍（CI，无 golden 依赖）。

对 test/golden/*.grands.json（golden -g0 文本 listing 计数，离线生成），
用本地 astrolog32-cli <cmd> --grands 复算格局记录，断言各类型计数一致。

用法: python verify_grands.py <astrolog32-cli> [golden-dir]
"""
import io, json, re, subprocess, sys, collections, os, glob

CLI = sys.argv[1] if len(sys.argv) > 1 else r"E:/data/gitCode/AstrologAsStar/astroproject/bin/windows/Debug/astrolog32-cli.exe"
GOLD = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(os.path.abspath(__file__)), "golden")

GR = re.compile(r"^GR (.*) (\d+) (\d+) (\d+) (\d+)\s*$")  # 名可含空格，末4 token=对象

def local_counts(cmd_tokens):
    p = subprocess.run([CLI] + cmd_tokens + ["--grands"],
                       capture_output=True, timeout=300)
    cnt = collections.Counter()
    for raw in p.stdout.decode("utf-8", "replace").splitlines():
        m = GR.match(raw.rstrip("\r"))
        if m:
            cnt[m.group(1).strip()] += 1
    return cnt, p.returncode

def main():
    files = sorted(glob.glob(os.path.join(GOLD, "*.grands.json")))
    if not files:
        print("no *.grands.json fixtures under", GOLD)
        return 1
    nbad = ntot = 0
    for f in files:
        js = json.load(io.open(f, encoding="utf-8"))
        cmd = js["cmd"].split()
        want = js["counts"]
        got, rc = local_counts(cmd)
        ntot += 1
        if rc != 0:
            print("FAIL %-18s cli rc=%d" % (js["chart"], rc)); nbad += 1; continue
        keys = sorted(set(want) | set(got))
        diffs = [k for k in keys if want.get(k, 0) != got.get(k, 0)]
        if diffs:
            nbad += 1
            print("FAIL %-18s want=%s" % (js["chart"], {k: want.get(k, 0) for k in keys}))
            print("     %-18s got =%s  diff=%s" % ("", {k: got.get(k, 0) for k in keys}, diffs))
        else:
            print("OK   %-18s configs=%d" % (js["chart"], sum(want.values())))
    print("grands_oracle: %d/%d charts matched golden -g0 counts" % (ntot - nbad, ntot))
    return 0 if nbad == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
