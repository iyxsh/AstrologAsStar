// astrolog32_unit_progress — P2/A8 次限推进链单测（no gtest）。
//
// 覆盖两条：
//   1) delta=0 恒等：推进目标 == 本命日期时刻 → 推进盘机器文本必须与本命
//      @0203 逐字节一致（金样级验证整条推进触发链：fProgressUS/is.JDp/
//      rProgDay → ProcessInput 推进公式，任何 JD/时区语义漂移都会打破恒等）。
//   2) 前向推进（1958→2026）：Sun 黄经变化落在合理区间（推进确已生效，
//      且非闰余/时区造成的小跳变）。
#include <cstdio>
#include <cstring>
#include <string>
#include "../../include/astrolog_lib.h"

static int g_fail = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); g_fail = 1; } \
} while (0)

static ChartInput mkci(int yea, int mon, int day, const char* tim,
    const char* dst, const char* zon, const char* lon, const char* lat)
{
    ChartInput c = { 0 };
    c.yea = yea; c.mon = mon; c.day = day;
    c.tim = RParseSz(tim, pmTim);
    c.dst = RParseSz(dst, pmDst);
    c.zon = RParseSz(zon, pmZon);
    c.lon = RParseSz(lon, pmLon);
    c.lat = RParseSz(lat, pmLat);
    wcsncpy(c.nam, L"bj-1958", 255);
    wcsncpy(c.loc, L"Beijing", 255);
    return c;
}

/* 从 @0203 行取对象黄经（deg）。行如 "/YF Sun 12 Can 19.xxx, ..."。 */
static double lon_of(const std::wstring& text, const wchar_t* obj)
{
    static const wchar_t* sign0[12] = { L"Ari", L"Tau", L"Gem", L"Can", L"Leo",
        L"Vir", L"Lib", L"Sco", L"Sag", L"Cap", L"Aqu", L"Pis" };
    size_t p = text.find(obj);
    if (p == std::wstring::npos) return -1e9;
    /* 找行首 */
    size_t ls = text.rfind(L'\n', p);
    ls = (ls == std::wstring::npos) ? 0 : ls + 1;
    size_t le = text.find(L'\n', ls);
    std::wstring line = text.substr(ls, (le == std::wstring::npos) ? std::wstring::npos : le - ls);
    /* 结构: /YF Sun <deg> <sign> <frac>, ... */
    int deg = 0; wchar_t sg[8] = { 0 }; double frac = 0.0;
    if (swscanf(line.c_str(), L"/YF %*ls %d %7ls %lf", &deg, sg, &frac) != 3)
        return -1e9;
    for (int i = 0; i < 12; i++)
        if (wcscmp(sg, sign0[i]) == 0)
            return (double)(i * 30 + deg) + frac / 60.0;
    return -1e9;
}

static double circ(double a, double b)   /* a-b 最短环距（度） */
{
    double d = a - b;
    while (d > 180.0) d -= 360.0;
    while (d < -180.0) d += 360.0;
    return d;
}

int main(void)
{
    SetSilent(true);
    initEnv();

    ChartInput nat = mkci(1958, 7, 4, "12:01", "0", "-8", "-116:23", "39:54");

    /* 1) delta=0 恒等：推进到本命自身 == 本命机器文本（9 位逐字节） */
    std::wstring natalText = GetChartMachineText(nat);
    std::wstring prog0 = GetProgressedMachineText(nat, 7, 4, 1958, "12:01",
        nat.dst, nat.zon);
    CHECK(prog0 == natalText);
    if (prog0 != natalText) {
        fprintf(stderr, "  delta=0 mismatch: natal len=%d prog len=%d\n",
            (int)natalText.size(), (int)prog0.size());
    }

    /* 2) 前向推进 1958-07-04 → 2026-09-05：Sun 变化应在合理区间 */
    std::wstring prog = GetProgressedMachineText(nat, 9, 5, 2026, "12:01",
        nat.dst, nat.zon);
    double sNat = lon_of(natalText, L"/YF Sun ");
    double sPrg = lon_of(prog, L"/YF Sun ");
    double mNat = lon_of(natalText, L"/YF Moo ");
    double mPrg = lon_of(prog, L"/YF Moo ");
    CHECK(sNat > -1e8 && sPrg > -1e8 && mNat > -1e8 && mPrg > -1e8);
    double dSun = circ(sPrg, sNat), dMoo = circ(mPrg, mNat);
    /* 次限推进 68 年 → 太阳约 +65..70°；月亮 68d×13.2°/d≈898°→环距约 178°。 */
    CHECK(dSun > 50.0 && dSun < 90.0);
    CHECK(fabs(dMoo) > 100.0 && fabs(dMoo) < 260.0);  /* 月亮大幅推进（环距 ~168°） */
    fprintf(stderr, "  deltaSun=%.4f deg  deltaMoon(circ)=%.4f deg\n", dSun, dMoo);

    if (g_fail) {
        fprintf(stderr, "FAIL unit_progress\n");
        return 1;
    }
    fprintf(stderr, "PASS unit_progress: delta=0 identity (golden-grade) + forward 1958->2026 sanity\n");
    return 0;
}
