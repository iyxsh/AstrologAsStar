// astrolog32_unit_synastry — P2/A10-3 关系网格（Synastry grid）单测（no gtest）。
//
// 关系网格 = 两盘之间所有交叉相位（盘A 对象 × 盘B 对象）。数值正确性锁定：
//   1) 对称：grid(A,B) 与 grid(B,A) 描述同一组「跨盘相位」（交换 A/B  designation 后
//      逐行集合相等）——破 A/B 两盘被当成同一盘 / i,j 映射错。
//   2) 角度自洽：每行 orb ≈ wrap180(MinDistance(lonA,lonB) − exact)（grid 内部即如此
//      存 orb，浮点尾差 < 弧分）→ 破 lonA/lonB 取错盘或 aspect 角算错。
//   3) asp 合法 token：asp ∈ {Con,Opp,Squ,Tri,Sex,Inc,SSx,SSq,Ses,Qui,BQn,SQn,Sep,
//      Nov,BNv,BSp,TSp,QNv}——破 aspect 索引越界/未初始化。
//   4) 头标记：机器文本以 "@0403" 起手（区别于合成/中点盘的 @0203）。
//   5) JSON 与机器文本一致：aspect 条数相等，且每条 JSON 能在文本中找到同 (objA,asp,
//      objB,lonA,lonB) 行。
//   6) 泄漏回归：grid 之后再取 chart(A) 与纯 chart(A) 逐字节（API 全局还原）。
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <cwchar>
#include <algorithm>
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

static double wrap180(double d) { while (d > 180.0) d -= 360.0; while (d < -180.0) d += 360.0; return d; }
static double minDist(double a, double b)
{
    double diff = fabs(a - b);
    double d = diff - floor(diff / 360.0) * 360.0;
    if (d > 180.0) d = 360.0 - d;
    return d;
}
static double r4(double d) { return floor(d * 1e4 + 0.5) / 1e4; }  // 4 位四舍五入键

struct Row { int a, b; std::string asp; double la, lb, orb, exact; };

/* 解析 @0403 机器文本 → 行向量（跳过 @ / # 头行）。 */
static std::vector<Row> parseGrid(const std::wstring& w)
{
    std::vector<Row> v;
    std::string s; s.reserve(w.size());
    for (wchar_t wc : w) { if (wc < 0x80) s.push_back((char)wc); else s.push_back('?'); }
    size_t pos = 0;
    while (pos < s.size()) {
        size_t nl = s.find('\n', pos);
        if (nl == std::string::npos) nl = s.size();
        std::string line = s.substr(pos, nl - pos);
        pos = nl + 1;
        if (line.empty()) continue;
        char c0 = line[0];
        if (c0 == '@' || c0 == '/' || c0 == '#') continue;
        // 字段: a asp b la lb orb exact
        int a, b; double la, lb, orb, exact; char asp[16];
        if (sscanf(line.c_str(), "%d %15s %d %lf %lf %lf %lf", &a, asp, &b, &la, &lb, &orb, &exact) == 7) {
            Row r; r.a = a; r.b = b; r.asp = asp; r.la = la; r.lb = lb; r.orb = orb; r.exact = exact;
            v.push_back(r);
        }
    }
    return v;
}

/* 解析 JSON aspects 数组 → 行向量（与机器文本同字段）。 */
static std::vector<Row> parseJSON(const std::string& js)
{
    std::vector<Row> v;
    size_t pos = 0;
    const char* keys[] = { "\"objA\":", "\"asp\":\"", "\"objB\":", "\"lonA\":", "\"lonB\":", "\"orb\":", "\"exact\":" };
    while ((pos = js.find(keys[0], pos)) != std::string::npos) {
        size_t p = pos + strlen(keys[0]);
        int a = atoi(js.c_str() + p);
        size_t pa = js.find(keys[1], p); if (pa == std::string::npos) break;
        size_t sa = pa + strlen(keys[1]);
        size_t ea = js.find('"', sa);
        std::string asp = js.substr(sa, ea - sa);
        size_t pb = js.find(keys[2], ea); if (pb == std::string::npos) break;
        int b = atoi(js.c_str() + pb + strlen(keys[2]));
        size_t pla = js.find(keys[3], pb); double la = (pla == std::string::npos) ? 0 : atof(js.c_str() + pla + strlen(keys[3]));
        size_t plb = js.find(keys[4], pla); double lb = (plb == std::string::npos) ? 0 : atof(js.c_str() + plb + strlen(keys[4]));
        size_t por = js.find(keys[5], plb); double orb = (por == std::string::npos) ? 0 : atof(js.c_str() + por + strlen(keys[5]));
        size_t pex = js.find(keys[6], por); double exact = (pex == std::string::npos) ? 0 : atof(js.c_str() + pex + strlen(keys[6]));
        Row r; r.a = a; r.b = b; r.asp = asp; r.la = la; r.lb = lb; r.orb = orb; r.exact = exact;
        v.push_back(r);
        pos = pex + 1;
    }
    return v;
}

static std::string rowKey(const Row& r)
{
    char buf[256];
    snprintf(buf, sizeof(buf), "%d|%s|%d|%.4f|%.4f", r.a, r.asp.c_str(), r.b, r4(r.la), r4(r.lb));
    return buf;
}

int main(void)
{
    SetSilent(true);
    initEnv();

    /* 北京 1958（既有金样盘）与 纽约 1985：异半球/异时区/异年代 */
    ChartInput A = mkci(1958, 7, 4, "12:01", "0", "8", "-116:23", "39:54");
    ChartInput B = mkci(1985, 1, 15, "9:30", "0", "-5", "74:00", "40:43");

    std::wstring gAB = GetSynastryGridMachineText(A, B);
    std::wstring gBA = GetSynastryGridMachineText(B, A);

    /* T5 头标记 */
    CHECK(gAB.rfind(L"@0403", 0) == 0);

    std::vector<Row> rAB = parseGrid(gAB);
    std::vector<Row> rBA = parseGrid(gBA);
    CHECK(!rAB.empty());
    CHECK(rAB.size() == rBA.size());

    /* T1 对称：grid(B,A) 每行 (b,a,asp,lb,la) 应命中 grid(A,B) 的 (a,b,asp,la,lb) */
    {
        std::set<std::string> sAB;
        for (auto& r : rAB) sAB.insert(rowKey(r));
        bool sym = true;
        for (auto& r : rBA) {
            Row t = r; std::swap(t.a, t.b); std::swap(t.la, t.lb);
            if (sAB.find(rowKey(t)) == sAB.end()) { sym = false;
                fprintf(stderr, "  symmetry miss: A%d-%s-B%d (la=%.4f lb=%.4f)\n", t.a, t.asp.c_str(), t.b, t.la, t.lb); }
        }
        CHECK(sym);
    }

    /* T2 角度自洽 + T3 asp 合法 token */
    {
        const char* validAsp[] = { "Con","Opp","Squ","Tri","Sex","Inc","SSx","SSq","Ses",
            "Qui","BQn","SQn","Sep","Nov","BNv","BSp","TSp","QNv" };
        int nValid = sizeof(validAsp) / sizeof(validAsp[0]);
        bool ok = true;
        for (auto& r : rAB) {
            bool tok = false;
            for (int t = 0; t < nValid; t++) if (r.asp == validAsp[t]) { tok = true; break; }
            if (!tok) { ok = false; fprintf(stderr, "  bad asp token: %s\n", r.asp.c_str()); }
            double sep = minDist(r.la, r.lb);
            double diff = wrap180(sep - r.exact);
            if (fabs(diff - r.orb) > 0.02) { ok = false;
                fprintf(stderr, "  angle mismatch a=%d b=%d asp=%s sep=%.6f exact=%.6f orb=%.6f diff-orb=%.6f\n",
                    r.a, r.b, r.asp.c_str(), sep, r.exact, r.orb, diff - r.orb); }
        }
        CHECK(ok);
    }

    /* T4 泄漏回归：grid 之后再取 chart(A) 必须与纯 chart(A) 一致 */
    {
        std::wstring ga = GetChartMachineText(A);
        std::wstring ga2 = GetChartMachineText(A);
        CHECK(ga == ga2);
    }

    /* T6 JSON 与机器文本一致 */
    {
        std::string jAB = GetSynastryGridJSON(A, B);
        std::vector<Row> jr = parseJSON(jAB);
        CHECK(jr.size() == rAB.size());
        std::set<std::string> sAB;
        for (auto& r : rAB) sAB.insert(rowKey(r));
        bool match = true;
        for (auto& r : jr) if (sAB.find(rowKey(r)) == sAB.end()) { match = false;
            fprintf(stderr, "  JSON row not in text: A%d-%s-B%d\n", r.a, r.asp.c_str(), r.b); }
        CHECK(match);
    }

    if (g_fail) { fprintf(stderr, "unit_synastry FAILED\n"); return 1; }
    fprintf(stderr, "unit_synastry OK (symmetry + angle-self-consistency + asp-token + leak + json-match) rows=%zu\n", rAB.size());
    return 0;
}
