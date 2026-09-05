// astrolog32_unit_returns — P2/A9-0 太阳返照解算单测（no gtest）。
//
// 定义驱动：太阳返照时刻 = 目标年内 Sun 黄经首次回到本命 Sun 黄经。
//   1) 解算时刻反查 Sun == 本命 Sun（±1e-6°，解算自洽）；
//   2) 相邻两年返照时刻相隔 ~365 天（2026/2027/2033 顺序与间距合理）。
// 本命 Sun 经度取自引擎 bj-1958 机器文本（保证与引擎口径一致）。
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include "../../include/astrolog_lib.h"
#include "../../include/core/returns.h"

static int g_fail = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); g_fail = 1; } \
} while (0)

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

int main(void)
{
    SetSilent(true);
    initEnv();

    /* bj-1958 本命 Sun（引擎口径） */
    ChartInput nat = { 0 };
    nat.yea = 1958; nat.mon = 7; nat.day = 4;
    nat.tim = RParseSz("12:01", pmTim);
    nat.dst = RParseSz("0", pmDst);
    nat.zon = RParseSz("8", pmZon);
    nat.lon = RParseSz("-116:23", pmLon);
    nat.lat = RParseSz("39:54", pmLat);
    wcsncpy(nat.nam, L"bj", 255);
    wcsncpy(nat.loc, L"bj", 255);
    std::wstring text = GetChartMachineText(nat);
    double natalSun = lon_of(text, L"/YF Sun ");
    CHECK(natalSun > -1e8);

    double j2026 = SolarReturnJulian(natalSun, 2026);
    double j2027 = SolarReturnJulian(natalSun, 2027);
    double j2033 = SolarReturnJulian(natalSun, 2033);
    CHECK(!isnan(j2026) && !isnan(j2027) && !isnan(j2033));

    /* 1) 身份：返照时刻反查 Sun == 本命 Sun */
    double e = fabs(SunEclipticLon(j2026) - natalSun);
    double e3 = fabs(SunEclipticLon(j2033) - natalSun);
    /* 用环距避免 0/360 边界 */
    double circ = fmod(fabs(SunEclipticLon(j2026) - natalSun), 360.0);
    if (circ > 180.0) circ = 360.0 - circ;
    CHECK(circ < 1e-6);
    CHECK(fmod(fabs(SunEclipticLon(j2033) - natalSun), 360.0) < 1e-6);
    fprintf(stderr, "  j2026=%.6f j2027=%.6f j2033=%.6f  SunErr2026=%.3e deg\n",
        j2026, j2027, j2033, e);
    fprintf(stderr, "  natalSun(bj-1958)=%.6f  (j2026 Sun)=%.6f (j2033 Sun)=%.6f e3=%.3e\n",
        natalSun, SunEclipticLon(j2026), SunEclipticLon(j2033), e3);

    /* 2) 逐年间距 ~365 天，顺序合理（2026→2033 跨 7 年） */
    CHECK(j2027 - j2026 > 355.0 && j2027 - j2026 < 375.0);
    CHECK(j2033 - j2026 > 7.0 * 355.0 && j2033 - j2026 < 7.0 * 375.0);

    /* 3) 月亮返照（A9-1）：2026 年逐月，月内 1~2 次；身份恒等 + 双月返存在性 */
    double natalMoon = lon_of(text, L"/YF Moo ");
    CHECK(natalMoon > -1e8);
    double mo[2];
    int total = 0, doubles = 0, worstBad = 0;
    for (int mm = 1; mm <= 12; mm++) {
        int n = LunarReturnJulians(natalMoon, 2026, mm, mo, 2);
        if (n < 1 || n > 2) worstBad++;
        if (n == 2) doubles++;
        total += n;
        for (int q = 0; q < n; q++) {
            double circ = fmod(fabs(MoonEclipticLon(mo[q]) - natalMoon), 360.0);
            if (circ > 180.0) circ = 360.0 - circ;
            if (circ > 1e-6) worstBad++;
        }
    }
    CHECK(worstBad == 0);
    CHECK(total >= 12 && total <= 15);          /* 每年月亮返照 ~13.37 次 */
    CHECK(doubles >= 1);                        /* 双月返（IsDoubleReturn 场景）存在 */
    fprintf(stderr, "  lunar 2026: total=%d doublesMonths=%d perMonth=[", total, doubles);
    for (int mm = 1; mm <= 12; mm++) {
        int n = LunarReturnJulians(natalMoon, 2026, mm, mo, 2);
        fprintf(stderr, "%d%s", n, mm == 12 ? "" : ",");
    }
    fprintf(stderr, "]\n");

    if (g_fail) {
        fprintf(stderr, "FAIL unit_returns\n");
        return 1;
    }
    fprintf(stderr, "PASS unit_returns: solar & lunar return moments self-consistent (identity + spacing + double-return)\n");
    return 0;
}
