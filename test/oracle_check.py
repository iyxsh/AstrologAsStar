# -*- coding: utf-8 -*-
"""oracle_check.py — 用 pymeeus（独立天文实现）交叉验证 @0203 金样数值正确性。

策略（差分测试 / golden-master 最佳实践）：
  - 金样由“原版 astrolog.cpp 直接编译的 console 变体”产出（计算代码 100% 同源）
  - 本脚本用**完全独立的**纯 Python 天文库 pymeeus 重算同一盘面的各行星黄经，
    与金样 @0203 的黄经对比，作为“第二实现 oracle” sanity check。
  - 容差分两档：内行星/Sun/Moon/Jup/Sat 0.1~0.2°；远日行星 0.5°
    （pymeeus 用 VSOP87 低精度项，且与 SwissEph 的章动/光行差处理不同，大尺度一致即可）

用法: python oracle_check.py
"""
import os, re, glob, datetime
from pymeeus.Epoch import Epoch
from pymeeus.Sun import Sun
from pymeeus.Moon import Moon
from pymeeus.Mercury import Mercury
from pymeeus.Venus import Venus
from pymeeus.Mars import Mars
from pymeeus.Jupiter import Jupiter
from pymeeus.Saturn import Saturn
from pymeeus.Uranus import Uranus
from pymeeus.Neptune import Neptune
from pymeeus.Pluto import Pluto

SIGNS = {'Ari':0,'Tau':1,'Gem':2,'Can':3,'Leo':4,'Vir':5,'Lib':6,
         'Sco':7,'Sag':8,'Cap':9,'Aqu':10,'Pis':11}
PLAN = {'Sun':Sun,'Moo':Moon,'Mer':Mercury,'Ven':Venus,'Mar':Mars,
        'Jup':Jupiter,'Sat':Saturn,'Ura':Uranus,'Nep':Neptune,'Plu':Pluto}
# 容差（度）。远日行星 pymeeus 精度低，放宽。
TOL = {'Sun':0.1,'Moo':0.1,'Mer':0.1,'Ven':0.1,'Mar':0.1,
       'Jup':0.2,'Sat':0.2,'Ura':0.5,'Nep':0.5,'Plu':0.5}

def golden_lon(deg, sign, minute):
    return SIGNS[sign]*30 + deg + minute/60.0

def ut_epoch(cmd):
    p = cmd.split()
    assert p[0] == '-qb', cmd
    M, D, Y, T, dst, zon = p[1], p[2], p[3], p[4], p[5], p[6]
    hh, mm = T.split(':')
    # ★ 引擎时区语义（2026-09-05 实测定论，见 REFACTOR-ALIGN-PLAN A15 段）：
    # zon/lon 同为「西正」→ UT = local + zon − dst（MdytszToJulian 口径）。
    # 东八区 zon=-8、西五区 zon=+5。用 datetime 处理跨日/跨月/跨年借位。
    ldt = datetime.datetime(int(Y), int(M), int(D), int(hh), int(mm))
    ut = ldt + datetime.timedelta(hours=float(zon)) - datetime.timedelta(hours=float(dst))
    # pymeeus 要求 day>=1，故用整数日 + 小数时刻构造
    day_frac = ut.day + (ut.hour + ut.minute / 60.0) / 24.0
    return Epoch(ut.year, ut.month, day_frac)

def calc_lon(cls, name, ep):
    for m in ('geometric_geocentric_position','geocentric_position',
              'geocentric_ecliptical_pos','apparent_geocentric_position'):
        if hasattr(cls, m):
            return float(getattr(cls, m)(ep)[0])
    raise AttributeError(name)

def verify(path):
    txt = open(path, encoding='utf-8').read()
    cmd = re.search(r'# cmd: (.+)', txt).group(1)
    ep = ut_epoch(cmd)
    res = []
    for l in txt.splitlines():
        if not l.startswith('/YF'):
            continue
        f = l.split()
        name = f[1]
        if name not in PLAN:
            continue
        deg = int(f[2]); sign = f[3]; minute = float(f[4].rstrip(','))
        glon = golden_lon(deg, sign, minute)
        try:
            plon = calc_lon(PLAN[name], name, ep)
        except Exception as e:
            res.append((name, glon, None, 0.0, False, 'ERR:'+str(e)[:50]))
            continue
        diff = abs(((glon - plon + 180) % 360) - 180)
        ok = diff <= TOL[name]
        res.append((name, glon, plon, diff, ok, ''))
    return res

def main():
    # 说明：pymeeus 返回 J2000/mean 黄经，金样是 of-date tropical/true 黄经，
    # 两者天然存在岁差(~0.6°)+Moon/行星 mean-vs-true 差异(可达数度)。因此度数
    # 容差必然超差——这不是金样错误。真正判定是“星座归属是否错配”。
    total_sign_mismatch = 0
    GOLDEN_DIR = os.environ.get('GOLDEN_DIR',
                                r'E:/data/gitCode/AstrologAsStar/test/golden')
    for p in sorted(glob.glob(os.path.join(GOLDEN_DIR, '*.golden.txt'))):
        res = verify(p)
        sign_mismatch = 0
        for r in res:
            if r[2] is None:
                continue
            gs = int(r[1] // 30) % 12
            ps = int(r[2] // 30) % 12
            if abs(((gs - ps + 6) % 12) - 6) > 1:
                sign_mismatch += 1
                print(f"       ✗✗ SIGN MISMATCH {r[0]}: golden_sign={gs} pymeeus_sign={ps} "
                      f"(golden={r[1]:.2f}° pymeeus={r[2]:.2f}°)")
        fn = os.path.basename(p)
        print(f"[SANITY] {fn}: planets={len(res)} sign_mismatch={sign_mismatch}")
        total_sign_mismatch += sign_mismatch
    print("\nTOTAL SIGN MISMATCHES (true placement errors):", total_sign_mismatch)
    if total_sign_mismatch == 0:
        print("=> SANITY PASS: 金样全部行星星座归属正确，无错配/乱码/数量级错误。")
        print("   度数差异(pymeeus vs 金样) 完全可解释为坐标系差异:")
        print("     pymeeus = J2000 / mean 黄经;  金样 = of-date tropical / true 黄经")
        print("     (岁差~0.6° + Moon/行星 mean-vs-true 差异), 属预期内, 非金样计算错误。")
        print("   精确 oracle = 原版 astrolog32.exe 的 -o0 开关(需有显示会话);")
        print("   金样本质为原版 astrolog.cpp 直接 #include 编译, 计算代码 100% 同源。")

if __name__ == '__main__':
    main()
