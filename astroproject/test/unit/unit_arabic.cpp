// astrolog32_unit_arabic — A5 阿拉伯点引擎单测（no gtest）。
//
// 每盘：initEnv + GetChartMachineText(ChartInput) 触发完整 CastChart（cp0/isDayBirth
// 就绪）→ ComputeArabicParts(rPart[177]) 断言：
//   1) 福点 apFor=0 == CastChart 内联 cp0.longitude[oFor]（已与 golden 9 位对齐）
//      —— 同时比对该盘 golden 数值锚点（bj 盘 226.637503635117°）；
//   2) 精神点 apSpi=1 == Asc ± (Sun−Moon)（日夜翻转语义同福点族）；
//   3) 全部已算点 ∈ [0,360)；
// 通过后向 stdout 打印可机读结果供 test/verify_arabic.py 二次 oracle 交叉对拍：
//     DAY <0|1>              （isDayBirth，供 F 族日夜翻转复算）
//     P <idx> <lon.12f>      （有效点；被屏蔽/递归未及 → 不打印）
#include <cmath>
#include <cstdio>
#include <cwchar>
#include "../../include/astrolog_lib.h"
#include "../../include/models/arabic_parts.h"
#include "../../include/models/settings.h"
#include "../../include/models/chart_data.h"
#include "../../include/utils/utils.h"

extern US us;
extern CP cp0;
extern bool isDayBirth;

static int g_fail = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); g_fail = 1; } \
} while (0)

static double mod360(double x) { x = fmod(x, 360.0); return x < 0.0 ? x + 360.0 : x; }

/* 与 CLI 完全同路径解析（-qb M D Y T dst zon lon lat，西经为正/东经为负），
 * 保证 CastChart 的时区/经纬度状态与 golden/CLI 一致（含 isDayBirth）。 */
static void fill_toks(ChartInput& c, int yea, int mon, int day,
                      const char* tim, const char* dst, const char* zon,
                      const char* lon, const char* lat)
{
    c.yea = yea; c.mon = mon; c.day = day;
    c.tim = RParseSz(tim, pmTim);
    c.dst = RParseSz(dst, pmDst);
    c.zon = RParseSz(zon, pmZon);
    c.lon = RParseSz(lon, pmLon);
    c.lat = RParseSz(lat, pmLat);
    c.alt = 0.0;
}

static void run_one(ChartInput& ci)
{
    GetChartMachineText(ci);                     /* 完整 CastChart，cp0/isDayBirth 就绪 */

    double rPart[cPart];
    ComputeArabicParts(rPart);

    printf("DAY %d\n", isDayBirth ? 1 : 0);      /* 供 python 二次 oracle 复算 F 族日夜翻转 */

    /* 1) 福点：通用引擎 == CastChart 内联（内联已 golden 9 位零差） */
    CHECK(fabs(rPart[apFor] - cp0.longitude[oFor]) < 1e-9);
    /*    bj-1958 盘 golden 锚点：For 16 Sco 38.250218107 = 226.6375036351167° */
    if (ci.yea == 1958 && ci.mon == 7 && ci.day == 4)
        CHECK(fabs(rPart[apFor] - 226.6375036351167) < 1e-7);

    /* 2) 精神点：Asc ± (Sun−Moon)，日夜翻转与福点族同条件 */
    {
        double s = isDayBirth ? 1.0 : -1.0;      /* F 族：夜生取负 rCur=Sun−Moon */
        double expSpirit = mod360(cp0.longitude[oAsc] + s * (cp0.longitude[oSun] - cp0.longitude[oMoo]));
        CHECK(fabs(rPart[apSpi] - expSpirit) < 1e-9);
    }

    /* 3) 已算点范围 + 机读输出（供 python 二次 oracle） */
    int n = 0;
    for (int i = 0; i < cPart; i++) {
        if (rPart[i] >= 0.0) {
            CHECK(rPart[i] < 360.0);
            printf("P %d %.12f\n", i, rPart[i]);
            n++;
        }
    }
    fprintf(stderr, "parts computed: %d/177 (chart %d-%d-%d day=%d)\n",
            n, ci.yea, ci.mon, ci.day, isDayBirth ? 1 : 0);
}

int main(void)
{
    SetSilent(true);
    initEnv();
    ChartInput ci = { 0 };
    fill_toks(ci, 1958, 7, 4, "12:01", "0", "8", "-116:23", "39:54");
    wcsncpy(ci.nam, L"bj-1958", 255);
    wcsncpy(ci.loc, L"Beijing", 255);
    run_one(ci);

    fill_toks(ci, 1969, 7, 20, "20:17", "1", "-7", "118:15", "34:03");
    wcsncpy(ci.nam, L"la-1969", 255);
    wcsncpy(ci.loc, L"Los Angeles", 255);
    run_one(ci);

    if (g_fail) {
        fprintf(stderr, "FAIL unit_arabic\n");
        return 1;
    }
    fprintf(stderr, "PASS unit_arabic: fortune/spirit == inline, range ok\n");
    return 0;
}
