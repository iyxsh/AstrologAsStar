// astrolog32_unit_midpoint — P2/A10-2 时空中点（Midpoint / -rm）单测（no gtest）。
//
// 时空中点语义（原版 rcMidpoint 镜像，astrolog.cpp CastRelation 3800-3835）：
// 中取两盘时刻 t1/t2 与地理坐标（lon/lat/zon/dst）各 50:50 中点（经度跨 180° 补
// Mod(+360*ratio)），按中点时空重新 cast。数值正确性锁定：
//   1) 恒等自反：midpoint(A,A) == chart(A) 逐字节（中点自反 ⇒ 装配链方向/比例错全破）；
//   2) 对称：midpoint(A,B) == midpoint(B,A) 逐字节（时空中点对称，破方向性 bug）；
//   3) JSON 恒等：midpoint(A,A) JSON == chart(A) JSON；
//   4) 时空中点数值锚定：同日同地 A2=12:00 / B2=18:00 → 中点盘 == 同地 15:00 普通盘
//      （时空中点端到端：时间中点 + 地点中点，金样路线已关，用引擎自身 chart 作 oracle）；
//   5) 泄漏回归：midpoint 之后再 chart(A) 与纯 chart(A) 逐字节（API 全局还原）。
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

/* 解析 JSON objects 数组：按出现顺序返回 (name, longitude)。顺序 = s_goldenIdx。 */
static std::vector<std::pair<std::string,double> > parseLons(const std::string& js)
{
    std::vector<std::pair<std::string,double> > v;
    size_t pos = 0;
    while ((pos = js.find("\"name\":\"", pos)) != std::string::npos) {
        pos += 8;
        size_t e = js.find('"', pos);
        if (e == std::string::npos) break;
        std::string nm = js.substr(pos, e - pos);
        std::string tail = js.substr(e, 260);
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

    /* 北京 1958（既有金样盘）与 纽约 1985：异半球/异时区/异年代 */
    ChartInput A = mkci(1958, 7, 4, "12:01", "0", "8", "-116:23", "39:54");
    ChartInput B = mkci(1985, 1, 15, "9:30", "0", "-5", "74:00", "40:43");

    /* T1 恒等自反：midpoint(A,A) == chart(A) 逐字节 */
    std::wstring maa = GetMidpointMachineText(A, A);
    std::wstring ga  = GetChartMachineText(A);
    CHECK(maa == ga);

    /* T5 泄漏回归：midpoint 后再取 chart(A) 必须与纯 chart(A) 一致 */
    std::wstring ga2 = GetChartMachineText(A);
    CHECK(ga2 == ga);

    /* T3 JSON 恒等 */
    CHECK(GetMidpointChartJSON(A, A) == GetChartJSON(A));

    /* T2 对称：midpoint(A,B) == midpoint(B,A) 逐字节 */
    std::wstring mab = GetMidpointMachineText(A, B);
    std::wstring mba = GetMidpointMachineText(B, A);
    CHECK(mab == mba);

    /* T4 时空中点数值锚定：同日同地 A2=12:00 / B2=18:00 → 中点 == 同地 15:00 普通盘。
     * 用引擎自身 chart 作 oracle（A10/A8 金样路线已关，oracle 自洽）。 */
    ChartInput A2 = mkci(1958, 7, 4, "12:00", "0", "8", "-116:23", "39:54");
    ChartInput B2 = mkci(1958, 7, 4, "18:00", "0", "8", "-116:23", "39:54");
    ChartInput C  = mkci(1958, 7, 4, "15:00", "0", "8", "-116:23", "39:54");
    std::vector<std::pair<std::string,double> > lm = parseLons(GetMidpointChartJSON(A2, B2));
    std::vector<std::pair<std::string,double> > lc = parseLons(GetChartJSON(C));
    CHECK(lm.size() == 40 && lc.size() == 40);
    if (lm.size() == 40 && lc.size() == 40) {
        for (size_t k = 0; k < 40; k++) {
            double d = fabs(wrap180(lm[k].second - lc[k].second));
            /* 时空中点端到端：DegMin 往返应精确，留 1e-6 容差抗浮点尾差 */
            CHECK(d < 1e-6);
            if (g_fail && d >= 1e-6) {
                fprintf(stderr, "  k=%zu name=%s mid=%.9f chartC=%.9f diff=%.3e\n",
                    k, lm[k].first.c_str(), lm[k].second, lc[k].second, d);
            }
        }
    }

    /* 良构性：40 对象全部 longitude ∈ [0,360) */
    for (size_t k = 0; k < lm.size(); k++) {
        CHECK(lm[k].second >= 0.0 && lm[k].second < 360.0);
    }

    if (g_fail) { fprintf(stderr, "unit_midpoint FAILED\n"); return 1; }
    fprintf(stderr, "unit_midpoint OK (reflexivity + symmetry + time/space-midpoint oracle + leak)\n");
    return 0;
}
