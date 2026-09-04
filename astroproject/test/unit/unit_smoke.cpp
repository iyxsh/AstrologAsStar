// astrolog32_unit — P0.4 API smoke tests (no gtest dependency).
//
// Plain C++11 assertions; returns 0 on success, 1 on failure so it can be
// registered with CTest directly. Covers:
//   1. RParseSz input parsing (time / longitude / zone / dst)
//   2. GetChartMachineText() structural smoke against the @0203 format
//   3. GetChartJSON() shape (40 objects, Fortune included)
//   4. Input validation — degenerate dates must not crash
//   5. UTF-8 round-trip of a Chinese chart name (same path the CLI uses)
//
// Numeric golden alignment itself is enforced by test/verify_cli.py
// (registered as the "golden_diff" CTest) — this file only guards the API.
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cwchar>
#include <string>
#include "../../include/astrolog_lib.h"

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        return 1; \
    } \
} while (0)

static int count_substr(const std::wstring& s, const wchar_t* sub)
{
    int n = 0;
    size_t pos = 0, len = wcslen(sub);
    while ((pos = s.find(sub, pos)) != std::wstring::npos) { n++; pos += len; }
    return n;
}

static std::string w2u(const std::wstring& w)   /* CLI-style UTF-8 encoder */
{
    std::string out;
    for (wchar_t wc : w) {
        if (wc < 0x80) out.push_back((char)wc);
        else if (wc < 0x800) {
            out.push_back((char)(0xC0 | (wc >> 6)));
            out.push_back((char)(0x80 | (wc & 0x3F)));
        } else {
            out.push_back((char)(0xE0 | (wc >> 12)));
            out.push_back((char)(0x80 | ((wc >> 6) & 0x3F)));
            out.push_back((char)(0x80 | (wc & 0x3F)));
        }
    }
    return out;
}

static ChartInput make_chart(const char* tim, const char* dst, const char* zon,
                             const char* lon, const char* lat,
                             int mon, int day, int yea,
                             const wchar_t* nam = L"", const wchar_t* loc = L"")
{
    ChartInput ci = {0};
    ci.mon = mon; ci.day = day; ci.yea = yea;
    ci.tim = RParseSz(tim, pmTim);
    ci.dst = RParseSz(dst, pmDst);
    ci.zon = RParseSz(zon, pmZon);
    ci.lon = RParseSz(lon, pmLon);
    ci.lat = RParseSz(lat, pmLat);
    ci.alt = 0.0;
    wcsncpy(ci.nam, nam, 255);
    wcsncpy(ci.loc, loc, 255);
    return ci;
}

int main()
{
    SetSilent(true);
    initEnv();

    /* ---- 1. RParseSz parsing ---- */
    {
        double t = RParseSz("12:01", pmTim);
        CHECK(fabs(t - 12.01) < 1e-6);          /* DegMin-style storage */
        double lon = RParseSz("-116:23", pmLon);
        CHECK(fabs(lon - (-116.23)) < 1e-6);
        double zon = RParseSz("8", pmZon);
        CHECK(fabs(zon - 8.0) < 1e-9);
        double dst = RParseSz("1", pmDst);
        CHECK(fabs(dst - 1.0) < 1e-9);
        double lat = RParseSz("39:54", pmLat);
        CHECK(fabs(lat - 39.54) < 1e-6);
    }

    /* ---- 2. @0203 machine text structure (bj-1958 input) ---- */
    ChartInput ci = make_chart("12:01", "0", "8", "-116:23", "39:54",
                               7, 4, 1958);
    std::wstring txt = GetChartMachineText(ci);
    CHECK(txt.find(L"@0203  ; Astrolog32 chart positions.\n") == 0);
    CHECK(count_substr(txt, L"/YF ") == 40);          /* exact object set */
    CHECK(txt.find(L"/YF Sun 12 Can 19.057349120,") != std::wstring::npos);
    CHECK(txt.find(L"/YF For 16 Sco 38.2502181") != std::wstring::npos);
    CHECK(txt.find(L"360.000000000") != std::wstring::npos);  /* cusp/For speed sentinel */
    CHECK(txt.find(L"/YF  45") != std::wstring::npos);        /* placeholder rows */
    CHECK(txt.find(L"/YF 117  0 Ari  0.000000000") != std::wstring::npos);
    CHECK(txt.find(L"999.000000000") != std::wstring::npos);

    /* ---- 3. JSON shape ---- */
    std::string js = GetChartJSON(ci);
    CHECK(js.find("{\"app\":\"Astrolog32\"") == 0);
    int names = 0;
    for (size_t p = 0; (p = js.find("\"name\"", p)) != std::string::npos; p += 6)
        names++;
    CHECK(names == 40);
    CHECK(js.find("\"houses\":{") != std::string::npos);

    /* ---- 4. Input validation (degenerate input must not crash) ---- */
    ChartInput bad = make_chart("25:99", "0", "8", "-999:99", "95:00",
                                2, 30, 2023);          /* month 2 has no day 30 */
    std::wstring btxt = GetChartMachineText(bad);
    CHECK(!btxt.empty());
    CHECK(btxt.compare(0, 5, L"@0203") == 0);

    /* ---- 5. UTF-8 round-trip of Chinese name/location ---- */
    ChartInput cn = make_chart("12:01", "0", "8", "-121:28", "31:14",
                               8, 8, 1988, L"上海", L"上海");
    std::wstring ctxt = GetChartMachineText(cn);
    CHECK(ctxt.find(L"/zi \"上海\" \"上海\"") != std::wstring::npos);
    std::string u8 = w2u(ctxt);
    /* "上海" = E4B88A E6B5B7 in UTF-8 */
    CHECK(u8.find("\xe4\xb8\x8a\xe6\xb5\xb7") != std::string::npos);

    puts("unit_smoke: ALL PASS");
    return 0;
}
