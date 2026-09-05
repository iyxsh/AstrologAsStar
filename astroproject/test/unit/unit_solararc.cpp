// astrolog32_unit_solararc — P2/A8-2 太阳/月亮弧方向盘单测（no gtest）。
//
// 验证（引擎内部构造恒等，无需独立星历）：
//   1) mode=2 太阳弧的 Sun == rcProgress 次限 Sun（构造：全体 += (推进Sun−本命Sun)）；
//   2) mode=4 月亮弧的 Moon == 次限 Moon；
//   3) mode=2 均匀移位：任意两对象在方向盘中的相对差 == 本命中的相对差；
//   4) delta=0（目标==本命）时 mode2/mode4 应≈本命（弧差≈0）。
// 移植自 golden CastChart（solarArc 预计算 ~20647 + 移位 ~20914），本地此前缺失。
#include <cstdio>
#include <cstring>
#include <map>
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
    wcsncpy(c.loc, L"bj", 255);
    return c;
}

static const wchar_t* sg[12] = { L"Ari", L"Tau", L"Gem", L"Can", L"Leo",
    L"Vir", L"Lib", L"Sco", L"Sag", L"Cap", L"Aqu", L"Pis" };

static double lon_of(const std::wstring& text, const wchar_t* obj)
{
    size_t p = text.find(obj);
    if (p == std::wstring::npos) return -1e9;
    size_t ls = text.rfind(L'\n', p);
    ls = (ls == std::wstring::npos) ? 0 : ls + 1;
    size_t le = text.find(L'\n', ls);
    std::wstring line = text.substr(ls, (le == std::wstring::npos) ? std::wstring::npos : le - ls);
    int deg = 0; wchar_t s[8] = { 0 }; double fr = 0.0;
    if (swscanf(line.c_str(), L"/YF %*ls %d %7ls %lf", &deg, s, &fr) != 3)
        return -1e9;
    for (int i = 0; i < 12; i++)
        if (wcscmp(s, sg[i]) == 0)
            return (double)(i * 30 + deg) + fr / 60.0;
    return -1e9;
}

static double circd(double a, double b)
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
    ChartInput nat = mkci(1958, 7, 4, "12:01", "0", "8", "-116:23", "39:54");

    std::wstring natText = GetChartMachineText(nat);
    std::wstring prog = GetProgressedMachineText(nat, 9, 5, 2026, "12:01", nat.dst, nat.zon);
    std::wstring sArc = GetSolarArcMachineText(nat, 9, 5, 2026, "12:01", 2, nat.dst, nat.zon);
    std::wstring mArc = GetSolarArcMachineText(nat, 9, 5, 2026, "12:01", 4, nat.dst, nat.zon);

    double pSun = lon_of(prog, L"/YF Sun ");
    double sSun = lon_of(sArc, L"/YF Sun ");       /* 太阳弧 Sun */
    double mSun = lon_of(mArc, L"/YF Sun ");       /* 月亮弧 Sun */
    double pMoo = lon_of(prog, L"/YF Moo ");
    double sMoo = lon_of(sArc, L"/YF Moo ");
    double mMoo = lon_of(mArc, L"/YF Moo ");
    double nSun = lon_of(natText, L"/YF Sun ");
    double nMoo = lon_of(natText, L"/YF Moo ");

    CHECK(pSun > -1e8 && sSun > -1e8 && mMoo > -1e8 && nSun > -1e8);

    /* 1) 太阳弧 Sun == 次限 Sun（构造恒等） */
    double e1 = circd(sSun, pSun);
    CHECK(e1 < 1e-6);
    fprintf(stderr, "  solarSun-progSun err=%.3e deg\n", e1);
    /* 2) 月亮弧 Moon == 次限 Moon */
    double e2 = circd(mMoo, pMoo);
    CHECK(e2 < 1e-6);
    fprintf(stderr, "  lunarMoon-progMoon err=%.3e deg\n", e2);
    /* 3) 太阳弧均匀移位：Mar-Sun 相对差不变 */
    double nMar = lon_of(natText, L"/YF Mar ");
    double sMar = lon_of(sArc, L"/YF Mar ");
    CHECK(nMar > -1e8 && sMar > -1e8);
    double e3 = circd(circd(sMar, sSun), circd(nMar, nSun));
    CHECK(e3 < 1e-6);
    fprintf(stderr, "  uniform-shift err=%.3e deg\n", e3);
    /* 4) delta=0：太阳弧/月亮弧目标==本命 → 弧差≈0，文本≈本命 */
    std::wstring s0 = GetSolarArcMachineText(nat, 7, 4, 1958, "12:01", 2, nat.dst, nat.zon);
    std::wstring m0 = GetSolarArcMachineText(nat, 7, 4, 1958, "12:01", 4, nat.dst, nat.zon);
    double e4 = circd(lon_of(s0, L"/YF Sun "), nSun);
    double e5 = circd(lon_of(m0, L"/YF Moo "), nMoo);
    CHECK(e4 < 1e-6 && e5 < 1e-6);
    fprintf(stderr, "  delta0 solarSun err=%.3e  lunarMoon err=%.3e deg\n", e4, e5);

    if (g_fail) {
        fprintf(stderr, "FAIL unit_solararc\n");
        return 1;
    }
    fprintf(stderr, "PASS unit_solararc: solar-arc Sun==progressed Sun, uniform shift, delta0 identity\n");
    return 0;
}
