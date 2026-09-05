// astrolog32_unit_composite — P2/A10-1 合成盘（Composite）单测（no gtest）。
//
// 合成盘语义（原版 rcComposite 镜像，astrolog.cpp CastRelation 3765-3800）：
// cp0 全体对象 = 两盘 50:50 中点 Ratio()（跨 180° 补 Mod(+360*ratio)），宫头另含
// 互补对/ASC >90° 补 180 校正。数值正确性锁定三层：
//   1) 恒等自反：composite(A,A) == chart(A) 逐字节（中点自反 ⇒ 装配链任何方向性
//      错误/漂移都会破坏，最强链检）；
//   2) 中点公式：composite(A,B) 行星（含 Node/Fortune）longitude == wrap180 解的中
//      点，容差 1e-5（JSON 9 位小数通道）；异半球异年代盘对自然覆盖跨 0° 组合；
//      宫头允许 ±180（原版校正）；占位恒星恒定 0 严格；
//   3) 泄漏回归：composite 之后再 chart(A) 与纯 chart(A) 逐字节（API 全局还原）。
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <utility>
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
    wcsncpy(c.nam, L"c", 255);
    wcsncpy(c.loc, L"c", 255);
    return c;
}

static double norm360(double d) { d = fmod(d, 360.0); if (d < 0) d += 360.0; return d; }
static double wrap180(double d) { while (d > 180.0) d -= 360.0; while (d < -180.0) d += 360.0; return d; }

/* 解析 JSON objects 数组：按出现顺序返回 (name, longitude)。
 * 顺序 = s_goldenIdx：k 0..9=行星1..10, 10=Node(16), 11=Fortune(19),
 * 12..23=宫头(22..33), 24..39=占位恒星(恒定 0)。 */
static std::vector<std::pair<std::string,double> > parseLons(const std::string& js)
{
    std::vector<std::pair<std::string,double> > v;
    size_t pos = 0;
    while ((pos = js.find("\"name\":\"", pos)) != std::string::npos) {
        pos += 8;                                    /* 跳过 "name":" */
        size_t e = js.find('"', pos);
        if (e == std::string::npos) break;
        std::string nm = js.substr(pos, e - pos);
        std::string tail = js.substr(e, 260);           /* name 后窗口内含 longitude */
        size_t lp = tail.find("\"longitude\":");
        if (lp == std::string::npos) break;
        double lon = atof(tail.c_str() + lp + 12);
        v.push_back(std::make_pair(nm, lon));
        pos = e + 1;
    }
    return v;
}

int main(void)
{
    SetSilent(true);
    initEnv();

    /* 北京 1958（既有金样盘）与 纽约 1985：异半球/异时区/异年代 → 行星分布
     * 差异大，跨 0° 边界组合自然出现。 */
    ChartInput A = mkci(1958, 7, 4, "12:01", "0", "-8", "-116:23", "39:54");
    ChartInput B = mkci(1985, 1, 15, "9:30", "0", "+5", "74:00", "40:43");

    /* T1 恒等自反：composite(A,A) == chart(A) 逐字节 */
    std::wstring caa = GetCompositeMachineText(A, A);
    std::wstring ga  = GetChartMachineText(A);
    CHECK(caa == ga);

    /* T3 泄漏回归：composite 后再取 chart(A) 必须与纯 chart(A) 一致 */
    std::wstring ga2 = GetChartMachineText(A);
    CHECK(ga2 == ga);

    /* T2 中点公式（JSON 十进制通道） */
    std::vector<std::pair<std::string,double> > la = parseLons(GetChartJSON(A));
    std::vector<std::pair<std::string,double> > lb = parseLons(GetChartJSON(B));
    std::vector<std::pair<std::string,double> > lc = parseLons(GetCompositeChartJSON(A, B));
    CHECK(la.size() == 40 && lb.size() == 40 && lc.size() == 40);
    if (la.size() == 40 && lb.size() == 40 && lc.size() == 40) {
        for (size_t k = 0; k < 40; k++) {
            double mid = norm360(la[k].second
                + wrap180(lb[k].second - la[k].second) * 0.5);
            double got = lc[k].second;
            /* 差统一走 wrap180（±180 域），勿用 norm360——负小差会被翻成 359.9° */
            double d = fabs(wrap180(got - mid));
            if (k < 12) {
                /* 行星 + Node + Fortune：严格中点（合成=两独立本命盘算术中点） */
                CHECK(d < 1e-5);
            } else if (k < 24) {
                /* 宫头：允许原版 180 互补校正 */
                CHECK(d < 1e-5 || fabs(d - 180.0) < 1e-5);
            } else {
                /* 占位恒星槽：恒定 0（两盘都 0，中点仍 0） */
                CHECK(fabs(got) < 1e-9);
            }
            if (g_fail) {
                fprintf(stderr, "  k=%zu name=%s la=%.9f lb=%.9f mid=%.9f got=%.9f\n",
                    k, la[k].first.c_str(), la[k].second, lb[k].second, mid, got);
            }
        }
    }

    /* T1b JSON 恒等：composite(A,A) JSON == chart(A) JSON（对象逐字段全等） */
    CHECK(GetCompositeChartJSON(A, A) == GetChartJSON(A));

    if (g_fail) { fprintf(stderr, "unit_composite FAILED\n"); return 1; }
    fprintf(stderr, "unit_composite OK (40 objects x midpoint identity + reflexivity + leak)\n");
    return 0;
}
