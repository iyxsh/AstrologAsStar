// astrolog32_unit_transit — P2/A8-1 行运盘单测（no gtest）。
//
// rcTransit 单盘语义：目标时刻普通盘 cast（ignore 换 ignore2 只影响宫位可见
// 性，不影响 cp0 位置）。断言 GetTransitMachineText(target) 与
// GetChartMachineText(target) 逐字节一致 → 行运数值正确性由既有 64 金样直接
// 继承（目标时刻普通盘已与 golden @0203 对齐）。
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
    wcsncpy(c.nam, L"t", 255);
    wcsncpy(c.loc, L"t", 255);
    return c;
}

int main(void)
{
    SetSilent(true);
    initEnv();

    /* 三种目标时刻盘（覆盖半球/时区/高纬）：行运文本必须与普通盘逐字节一致 */
    ChartInput tgts[3];
    tgts[0] = mkci(1958, 7, 4, "12:01", "0", "-8", "-116:23", "39:54"); /* bj 本命 */
    tgts[1] = mkci(2026, 9, 5, "12:01", "0", "-8", "-116:23", "39:54");  /* 未来同址 */
    tgts[2] = mkci(2020, 12, 21, "12:00", "0", "-1", "-15:00", "78:00"); /* 78N 高纬 */

    for (int k = 0; k < 3; k++) {
        std::wstring a = GetChartMachineText(tgts[k]);
        std::wstring b = GetTransitMachineText(tgts[k]);
        CHECK(a == b);
        if (a != b)
            fprintf(stderr, "  transit mismatch case %d: len %d vs %d\n", k,
                (int)a.size(), (int)b.size());
    }
    /* sanity：2026 行运盘 Sun 应显著不同于 1958 本命 Sun（前进） */
    {
        ChartInput nat = tgts[0], fut = tgts[1];
        /* 简单取 "Sun" 行号 —— 只断言两盘文本确实不同即可（前进必有差异） */
        std::wstring n = GetChartMachineText(nat);
        std::wstring f = GetTransitMachineText(fut);
        CHECK(n != f);
    }

    if (g_fail) {
        fprintf(stderr, "FAIL unit_transit\n");
        return 1;
    }
    fprintf(stderr, "PASS unit_transit: rcTransit single-chart == target chart (golden inherited)\n");
    return 0;
}
