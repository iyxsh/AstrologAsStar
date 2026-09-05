// astrolog32_unit_grands — A7 格局识别 API（DetectGrands）单测（no gtest）。
//
// 用合成 aspect grid + 位置构造 8 类格局，断言 DetectGrands 能检出：
//   Stellium / Grand Trine / Kite / T-Square / Yod / Grand Cross / Cradle /
//   Mystic Rectangle（条件镜像 astrolog.cpp DisplayGrands，逐行对齐）。
#include <cstdio>
#include <cstring>
#include "../../include/core/aspects.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"

extern US us;
extern CP cp0;

static int g_fail = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); g_fail = 1; } \
} while (0)

static void setpair(GridInfo& g, int a, int b, int asp)
{
    g.n[a][b] = g.n[b][a] = asp;
}

/* 返回 buf 中是否存在 (ac, a, b, c, d) 记录（d 传 -1 表示任意） */
static int has(const int buf[][5], int n, int ac, int a, int b, int c, int d)
{
    for (int t = 0; t < n; t++)
        if (buf[t][0] == ac && buf[t][1] == a && buf[t][2] == b &&
            buf[t][3] == c && (d < 0 || buf[t][4] == d))
            return 1;
    return 0;
}

int main(void)
{
    GridInfo g;
    int buf[8192][5];

#define ZERO() do { memset(&g, 0, sizeof(g)); } while (0)

    /* --- 1. Stellium：1/2/3 三者合相 --- */
    ZERO();
    setpair(g, 1, 2, aCon); setpair(g, 1, 3, aCon); setpair(g, 2, 3, aCon);
    {
        int n = DetectGrands(&g, buf, 8192);
        CHECK(n >= 1 && has(buf, n, acS, 1, 2, 3, -1));
    }

    /* --- 2. Grand Trine + Kite：1/2/3 三合 + 4 与 1、2 六分 --- */
    ZERO();
    setpair(g, 1, 2, aTri); setpair(g, 1, 3, aTri); setpair(g, 2, 3, aTri);
    setpair(g, 1, 4, aSex); setpair(g, 2, 4, aSex);
    {
        int n = DetectGrands(&g, buf, 8192);
        CHECK(n >= 1 && has(buf, n, acGT, 1, 2, 3, -1));
        CHECK(n >= 1 && has(buf, n, acK, 1, 2, 3, 4));
    }

    /* --- 3. T-Square：5 冲 6，7 与两者四分（i 为顶点） --- */
    ZERO();
    setpair(g, 5, 6, aOpp); setpair(g, 5, 7, aSqu); setpair(g, 6, 7, aSqu);
    {
        int n = DetectGrands(&g, buf, 8192);
        CHECK(n >= 1);   /* acTS(7,5,6) 或 (5,6,7) 依循环序 */
        int found = 0;
        for (int t = 0; t < n; t++)
            if (buf[t][0] == acTS) found = 1;
        CHECK(found);
    }

    /* --- 4. Yod：8/9 六分，10 与两者梅花相 --- */
    ZERO();
    setpair(g, 8, 9, aSex); setpair(g, 8, 10, aInc); setpair(g, 9, 10, aInc);
    {
        int n = DetectGrands(&g, buf, 8192);
        int found = 0;
        for (int t = 0; t < n; t++)
            if (buf[t][0] == acY) found = 1;
        CHECK(found);
    }

    /* --- 5. Grand Cross：1(0°) 2(90°) 3(180°) 4(270°) 全四分链 --- */
    ZERO();
    for (int i = 1; i <= 118; i++) cp0.longitude[i] = 0.0;
    cp0.longitude[1] = 0.0; cp0.longitude[2] = 90.0;
    cp0.longitude[3] = 180.0; cp0.longitude[4] = 270.0;
    setpair(g, 1, 2, aSqu); setpair(g, 2, 3, aSqu);
    setpair(g, 3, 4, aSqu); setpair(g, 1, 4, aSqu);
    setpair(g, 1, 3, aOpp); setpair(g, 2, 4, aOpp);
    {
        int n = DetectGrands(&g, buf, 8192);
        CHECK(n >= 1 && has(buf, n, acGC, 1, 2, 3, 4));
    }

    /* --- 6. Cradle：1(0°) 2(60°) 3(120°) 4(180°) 链六分，首尾冲 --- */
    ZERO();
    cp0.longitude[1] = 0.0; cp0.longitude[2] = 60.0;
    cp0.longitude[3] = 120.0; cp0.longitude[4] = 180.0;
    setpair(g, 1, 2, aSex); setpair(g, 2, 3, aSex); setpair(g, 3, 4, aSex);
    setpair(g, 1, 4, aOpp);
    {
        int n = DetectGrands(&g, buf, 8192);
        CHECK(n >= 1 && has(buf, n, acC, 1, 2, 3, 4));
    }

    /* --- 7. Mystic Rectangle：1-2 冲、3-4 冲、1-3 三合、2-4 三合 --- */
    ZERO();
    setpair(g, 1, 2, aOpp); setpair(g, 3, 4, aOpp);
    setpair(g, 1, 3, aTri); setpair(g, 2, 4, aTri);
    {
        int n = DetectGrands(&g, buf, 8192);
        CHECK(n >= 1 && has(buf, n, acMR, 1, 2, 3, 4));
    }

    /* --- 8. 空网格 → 0 --- */
    ZERO();
    {
        int n = DetectGrands(&g, buf, 8192);
        CHECK(n == 0);
    }

    if (g_fail) {
        fprintf(stderr, "FAIL unit_grands\n");
        return 1;
    }
    printf("PASS unit_grands: stellium/grand-trine/kite/t-square/yod/grand-cross/cradle/mystic-rect\n");
    return 0;
}
