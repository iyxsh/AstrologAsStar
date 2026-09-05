// astrolog32_unit_eclipse — P2/A15 日月食表单测（no gtest）。
//
// 食表 = 本命年份内全部日食 / 月食（golden PrintSolarEclipse/PrintLunarEclipse 同语义：
// Jan 1 UT 起循环 when_glob，越年即止；时刻按盘时区转本地时）。数值正确性锁定：
//   1) 头标记：日食 @0503 / 月食 @0504（区别于 @0203 盘、@0403 关系网格）。
//   2) 条数与升序：日食 2~5 条、月食 2~3 条，且 jd_ut 严格升序（破窗口/越年逻辑）。
//   3) 极大时刻极值自洽：日食极大处日月经度距角为**局部极小**、月食为**局部极大**
//      （对比 ±0.05d）——破极大时刻取错 / 日月取反。
//   4) 2026 已知食锚定：2 月环食、8 月全食（日食）；3 月全影月食、8 月偏食（月食）。
//   5) type 合法 token；JSON 与文本条数/时刻一致。
//   6) 泄漏回归：取食表后再取 chart(A) 与纯 chart(A) 逐字节。
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <cwchar>
#include "../../include/astrolog_lib.h"
#include "../../swe/swisseph/swephexp.h"

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

static std::string narrow(const std::wstring& w)
{
    std::string s;
    for (size_t i = 0; i < w.size(); i++)
        s += (char)(w[i] < 128 ? w[i] : '?');
    return s;
}

static double wrap180(double d) { while (d > 180.0) d -= 360.0; while (d < -180.0) d += 360.0; return d; }

/* 引擎同路径黄经（与主链一致：无档位时回退 Moshier）。 */
static double BodyLon(int seBody, double jdUt)
{
    double x[6];
    char serr[256];
    double jde = jdUt + swe_deltat(jdUt);
    if (swe_calc(jde, seBody, 0, x, serr) < 0)
        if (swe_calc(jde, seBody, SEFLG_MOSEPH, x, serr) < 0)
            return NAN;
    return x[0];
}

/* 日月经度距角（deg, 0..180）。 */
static double SunMoonSep(double jdUt)
{
    double ls = BodyLon(SE_SUN, jdUt);
    double lm = BodyLon(SE_MOON, jdUt);
    if (isnan(ls) || isnan(lm)) return NAN;
    return fabs(wrap180(ls - lm));
}

struct Row {
    int year, mon, day, hour, minute;
    char type[32];
    double jd, mag, lon, lat;
};

/* 解析 @0503/@0504 机器文本 → 行向量（跳过 @ 头行与 # 注释行）。 */
static std::vector<Row> parseTable(const std::wstring& w)
{
    std::vector<Row> v;
    std::string s = narrow(w);
    size_t pos = 0;
    while (pos < s.size())
    {
        size_t eol = s.find('\n', pos);
        if (eol == std::string::npos) eol = s.size();
        std::string line = s.substr(pos, eol - pos);
        pos = eol + 1;
        if (line.empty() || line[0] == '@' || line[0] == '#') continue;
        Row r;
        memset(&r, 0, sizeof(r));
        if (sscanf(line.c_str(), "%d %d %d %d %d %31s %lf %lf %lf %lf",
            &r.year, &r.mon, &r.day, &r.hour, &r.minute, r.type,
            &r.jd, &r.mag, &r.lon, &r.lat) == 10)
            v.push_back(r);
    }
    return v;
}

static int validType(const char* t)
{
    static const char* toks[] = { "Penumbral", "TotalPenumbral", "Partial",
        "Annular", "AnnularTotal", "Total", "NonCentral" };
    for (int i = 0; i < 7; i++)
        if (strcmp(t, toks[i]) == 0) return 1;
    return 0;
}

static int countIn(const std::string& s, const char* sub)
{
    int n = 0;
    for (size_t p = s.find(sub); p != std::string::npos; p = s.find(sub, p + 1)) n++;
    return n;
}

/* 逐表校验：头标记 + 条数 + 升序 + 极值自洽 + 类型合法。 */
static void checkTable(const std::wstring& txt, const std::string& js,
    const char* marker, int solar, int lo, int hi)
{
    std::string s = narrow(txt);
    std::vector<Row> rows = parseTable(txt);
    size_t i;

    CHECK(s.find(marker) == 0);                       /* 1) 头标记 */
    CHECK((int)rows.size() >= lo && (int)rows.size() <= hi);   /* 2) 条数窗口 */

    for (i = 0; i < rows.size(); i++)
    {
        const Row& r = rows[i];
        CHECK(r.year >= 2025 && r.year <= 2027);
        CHECK(r.mon >= 1 && r.mon <= 12);
        CHECK(r.hour >= 0 && r.hour <= 23 && r.minute >= 0 && r.minute <= 59);
        CHECK(validType(r.type));                     /* 5a) 合法 token */
        CHECK(r.jd > 2400000.0 && r.jd < 2500000.0);
        if (i > 0) CHECK(r.jd > rows[i - 1].jd);      /* 2b) 严格升序 */

        /* 3) 极大时刻 = 日月经度距角的局部极值 */
        double d0 = SunMoonSep(r.jd);
        double dm = SunMoonSep(r.jd - 0.05);
        double dp = SunMoonSep(r.jd + 0.05);
        CHECK(!isnan(d0) && !isnan(dm) && !isnan(dp));
        if (solar)
        {
            CHECK(d0 <= dm + 1e-6 && d0 <= dp + 1e-6);   /* 距角极小 */
            CHECK(d0 < 2.0);
        }
        else
        {
            double e0 = fabs(d0 - 180.0), em = fabs(dm - 180.0), ep = fabs(dp - 180.0);
            CHECK(e0 <= em + 1e-6 && e0 <= ep + 1e-6);   /* 距角极大（近冲） */
            CHECK(e0 < 2.0);
        }
    }

    /* 5b) JSON 与文本一致：事件条数相等 */
    int nJson = countIn(js, "\"jd_ut\"");
    CHECK(nJson == (int)rows.size());
    CHECK(js.find(solar ? "\"eclipse\":\"solar\"" : "\"eclipse\":\"lunar\"")
        != std::string::npos);
}

int main()
{
    initEnv();
    SetSilent(true);

    /* 北京 2026 → 食表窗口 = 2026 全年。
     * ★ 本仓 zon/lon 为「西正」约定（UT = local + zon − dst，见 utils.cpp
     *   MdytszToJulian；实测：北京 1958-07-04 12:01 传 zon=-8 → Sun 101.68°
     *   ≈ 地方正午真值 101.71°，传 +8 → 102.32° ≈ UT 20:01）→ 东八区须传 **-8**。 */
    ChartInput c = mkci(2026, 6, 1, "12:00", "0", "-8", "-116:23", "39:54");
    std::wstring base = GetChartMachineText(c);

    std::wstring st = GetSolarEclipseTableText(c);
    std::wstring lt = GetLunarEclipseTableText(c);
    std::string  sj = GetSolarEclipseTableJSON(c);
    std::string  lj = GetLunarEclipseTableJSON(c);

    printf("--- 2026 solar (Beijing +8) ---\n%s", narrow(st).c_str());
    printf("--- 2026 lunar (Beijing +8) ---\n%s", narrow(lt).c_str());

    checkTable(st, sj, "@0503", 1, 2, 5);
    checkTable(lt, lj, "@0504", 0, 2, 3);

    /* 4) 2026 已知食锚定（UTC 真值 → 北京本地 = UT+8）：
     *    日食 2/17 12:11 环食 → 本地 2/17 20:11；8/12 17:46 全食 → 本地 8/13 01:46。
     *    月食 3/3 11:33 全影 → 本地 3/3 19:33；8/28 04:13 偏食 → 本地 8/28 12:13。 */
    std::vector<Row> sol = parseTable(st);
    std::vector<Row> lun = parseTable(lt);
    int febAnnular = 0, augTotal = 0, marTotalL = 0, augPartialL = 0, febLocal20 = 0;
    for (size_t i = 0; i < sol.size(); i++)
    {
        if (sol[i].mon == 2 && sol[i].day == 17 && strcmp(sol[i].type, "Annular") == 0)
        {
            febAnnular = 1;
            if (sol[i].hour == 20 && sol[i].minute >= 10 && sol[i].minute <= 12)
                febLocal20 = 1;      /* 本地时 = UT + 8（西正 zon 约定） */
        }
        if (sol[i].mon == 8 && sol[i].day == 13 && strcmp(sol[i].type, "Total") == 0)
            augTotal = 1;
    }
    for (size_t i = 0; i < lun.size(); i++)
    {
        if (lun[i].mon == 3 && lun[i].day == 3 && strcmp(lun[i].type, "Total") == 0)
            marTotalL = 1;
        if (lun[i].mon == 8 && lun[i].day == 28 && strcmp(lun[i].type, "Partial") == 0)
            augPartialL = 1;
    }
    CHECK(febAnnular);
    CHECK(febLocal20);
    CHECK(augTotal);
    CHECK(marTotalL);
    CHECK(augPartialL);

    /* 6) 泄漏回归 */
    std::wstring after = GetChartMachineText(c);
    CHECK(after == base);

    if (g_fail) { fprintf(stderr, "unit_eclipse FAILED\n"); return 1; }
    printf("unit_eclipse OK (marker + count + ascending + extremum oracle + 2026 known eclipses + leak)\n");
    return 0;
}
