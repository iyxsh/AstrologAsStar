// P2/A15 日月食表（对齐 golden PrintSolarEclipse/PrintLunarEclipse；见 eclipses.h 注释）。
#include "../../include/core/eclipses.h"
#include "../../include/core/ephemeris.h"   /* SetEphemerisPath（主链同款路径） */
#include "../../swe/swisseph/swephexp.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static void EnsureSwe(void)
{
	static int done = 0;
	if (!done) { SetEphemerisPath(); done = 1; }
}

/* 星历档位：与主链一致优先真实星历，缺 .se1 时回退 Moshier（食函数两档均支持）。 */
static int EclIf(void)
{
	static int cached = -2;
	if (cached == -2)
	{
		double x[6];
		char serr[AS_MAXCH];
		double jd = 2451545.0;
		EnsureSwe();
		cached = (swe_calc(jd + swe_deltat(jd), SE_SUN, SEFLG_SWIEPH, x, serr) >= 0)
			? SEFLG_SWIEPH : SEFLG_MOSEPH;
	}
	return cached;
}

const char* EclipseTypeName(int type)
{
	switch (type)
	{
	case eclPenumbral:      return "Penumbral";
	case eclTotalPenumbral: return "TotalPenumbral";
	case eclPartial:        return "Partial";
	case eclAnnular:        return "Annular";
	case eclAnnularTotal:   return "AnnularTotal";
	case eclTotal:          return "Total";
	case eclNonCentral:     return "NonCentral";
	default:                return "None";
	}
}

/* SE_ECL_* 位标志 → 食型。ANNULAR_TOTAL(hybrid) 先判：swe 对全环食会同时置
 * ANNULAR|TOTAL|ANNULAR_TOTAL；其余按 golden if 链优先级（全 > 环 > 偏 > 非中心）。 */
static int MapSolarType(int eclflag)
{
	if (eclflag & SE_ECL_ANNULAR_TOTAL) return eclAnnularTotal;
	if (eclflag & SE_ECL_TOTAL)         return eclTotal;
	if (eclflag & SE_ECL_ANNULAR)       return eclAnnular;
	if (eclflag & SE_ECL_PARTIAL)       return eclPartial;
	if (eclflag & SE_ECL_NONCENTRAL)    return eclNonCentral;
	return eclNone;
}

static int MapLunarType(int eclflag)
{
	if (eclflag & SE_ECL_TOTAL)      return eclTotal;
	if (eclflag & SE_ECL_PARTIAL)    return eclPartial;
	if (eclflag & SE_ECL_PENUMBRAL)  return eclPenumbral;
	return eclNone;
}

/* UT JD → 盘时区本地时刻（golden call_solar_eclipse 同款：先向最近分钟取整，
 * 再 swe_utc_time_zone 按时区平移）。 */
static void LocalFromUt(double jdUt, double zon, int* py, int* pmo, int* pd,
	int* ph, int* pmi)
{
	int y, mo, d, h, mi, oy, omo, od, oh, omi;
	double ut, sec, osec;

	swe_revjul(jdUt, jdUt >= 2299171.0 ? SE_GREG_CAL : SE_JUL_CAL, &y, &mo, &d, &ut);
	ut += 0.5 / 3600.0;
	h  = (int)ut;
	mi = (int)fmod(ut * 60.0, 60.0);
	sec = fmod(ut * 3600.0, 60.0);
	oy = y; omo = mo; od = d; oh = h; omi = mi; osec = sec;
	swe_utc_time_zone(y, mo, d, h, mi, sec, zon, &oy, &omo, &od, &oh, &omi, &osec);
	*py = oy; *pmo = omo; *pd = od; *ph = oh; *pmi = omi;
}

static int GregFlagFor(int yea)
{
	return ((long)yea * 10000L + 101L < 15821015L) ? SE_JUL_CAL : SE_GREG_CAL;
}

std::vector<EclipseEvent> SolarEclipseTable(const ChartInput& chart)
{
	std::vector<EclipseEvent> out;
	char serr[AS_MAXCH];
	double tret[30], attr[30], geopos[10];
	double jd0, t;
	int i, ifl;

	if (chart.yea < 1)
		return out;
	EnsureSwe();
	ifl = EclIf();
	jd0 = swe_julday(chart.yea, 1, 1, 0.0, GregFlagFor(chart.yea));
	t = jd0;
	for (i = 0; i < 10; i++)
	{
		int y, mo, d, eclflag, type;
		double tmax, ut;
		EclipseEvent e;

		memset(tret, 0, sizeof(tret));
		memset(attr, 0, sizeof(attr));
		eclflag = swe_sol_eclipse_when_glob(t, ifl, SE_ECL_ALLTYPES_SOLAR, tret, 0, serr);
		if (eclflag == ERR)
			break;
		tmax = tret[0];
		if (!(tmax > 0.0))
			break;
		swe_revjul(tmax, tmax >= 2299171.0 ? SE_GREG_CAL : SE_JUL_CAL, &y, &mo, &d, &ut);
		if (y > chart.yea)            /* 越年即止（golden 同） */
			break;

		memset(&e, 0, sizeof(e));
		e.jdUt = tmax;
		e.type = MapSolarType(eclflag);
		LocalFromUt(tmax, chart.zon, &e.year, &e.mon, &e.day, &e.hour, &e.minute);
		/* 极大点地理坐标 + 食分（attr[0]） */
		geopos[0] = 0.0; geopos[1] = 0.0; geopos[2] = 0.0;
		if (swe_sol_eclipse_where(tmax, ifl, geopos, attr, serr) >= 0)
		{
			e.lon = geopos[0];
			e.lat = geopos[1];
			e.mag = attr[0];
		}
		out.push_back(e);
		t = tmax + 0.5;               /* 显式前移（同类食间隔 ≥29d） */
	}
	return out;
}

std::vector<EclipseEvent> LunarEclipseTable(const ChartInput& chart)
{
	std::vector<EclipseEvent> out;
	char serr[AS_MAXCH];
	double tret[30], attr[30];
	double jd0, t;
	int i, ifl;

	if (chart.yea < 1)
		return out;
	EnsureSwe();
	ifl = EclIf();
	jd0 = swe_julday(chart.yea, 1, 1, 0.0, GregFlagFor(chart.yea));
	t = jd0;
	for (i = 0; i < 10; i++)
	{
		int y, mo, d, eclflag;
		double tmax, ut;
		EclipseEvent e;

		memset(tret, 0, sizeof(tret));
		memset(attr, 0, sizeof(attr));
		eclflag = swe_lun_eclipse_when(t, ifl, SE_ECL_ALLTYPES_LUNAR, tret, 0, serr);
		if (eclflag == ERR)
			break;
		tmax = tret[0];
		if (!(tmax > 0.0))
			break;
		swe_revjul(tmax, tmax >= 2299171.0 ? SE_GREG_CAL : SE_JUL_CAL, &y, &mo, &d, &ut);
		if (y > chart.yea)
			break;

		memset(&e, 0, sizeof(e));
		e.jdUt = tmax;
		e.type = MapLunarType(eclflag);
		LocalFromUt(tmax, chart.zon, &e.year, &e.mon, &e.day, &e.hour, &e.minute);
		/* 月食无极大点；attr[0] = umbral magnitude（geopos 传 NULL 走地心） */
		if (swe_lun_eclipse_how(tmax, ifl, NULL, attr, serr) >= 0)
			e.mag = attr[0];
		out.push_back(e);
		t = tmax + 0.5;
	}
	return out;
}

/* ---------- 机器文本 / JSON 写行器（@0503 日食表 / @0504 月食表） ---------- */

/* 纯 ASCII 追加（本机文本全为 ASCII；回避各家 wprintf 对 %hs/%s 的语义差异）。 */
static void AppendAscii(std::wstring& s, const char* p)
{
	for (; p && *p; ++p)
		s += (wchar_t)(unsigned char)*p;
}

static std::wstring BuildEclipseText(const char* marker, const char* kind,
	const std::vector<EclipseEvent>& v)
{
	char line[256];
	std::wstring s;
	size_t i;

	AppendAscii(s, marker);
	s += L'\n';
	snprintf(line, sizeof(line), "# %s count=%d\n", kind, (int)v.size());
	AppendAscii(s, line);
	for (i = 0; i < v.size(); i++)
	{
		const EclipseEvent& e = v[i];
		snprintf(line, sizeof(line), "%d %d %d %d %d %s %.5f %.4f %.4f %.4f\n",
			e.year, e.mon, e.day, e.hour, e.minute, EclipseTypeName(e.type),
			e.jdUt, e.mag, e.lon, e.lat);
		AppendAscii(s, line);
	}
	return s;
}

static std::string BuildEclipseJSON(const char* kind,
	const std::vector<EclipseEvent>& v)
{
	char buf[320];
	std::string s;
	size_t i;

	s += "{\"eclipse\":\"";
	s += kind;
	s += "\",\"count\":";
	snprintf(buf, sizeof(buf), "%d", (int)v.size());
	s += buf;
	s += ",\"events\":[";
	for (i = 0; i < v.size(); i++)
	{
		const EclipseEvent& e = v[i];
		if (i) s += ",";
		snprintf(buf, sizeof(buf),
			"{\"year\":%d,\"mon\":%d,\"day\":%d,\"hour\":%d,\"min\":%d,"
			"\"type\":\"%s\",\"jd_ut\":%.5f,\"mag\":%.4f,\"lon\":%.4f,\"lat\":%.4f}",
			e.year, e.mon, e.day, e.hour, e.minute, EclipseTypeName(e.type),
			e.jdUt, e.mag, e.lon, e.lat);
		s += buf;
	}
	s += "]}";
	return s;
}

ASTROLOG32_API std::wstring GetSolarEclipseTableText(const ChartInput& chart)
{
	return BuildEclipseText("@0503", "solar", SolarEclipseTable(chart));
}

ASTROLOG32_API std::wstring GetLunarEclipseTableText(const ChartInput& chart)
{
	return BuildEclipseText("@0504", "lunar", LunarEclipseTable(chart));
}

ASTROLOG32_API std::string GetSolarEclipseTableJSON(const ChartInput& chart)
{
	return BuildEclipseJSON("solar", SolarEclipseTable(chart));
}

ASTROLOG32_API std::string GetLunarEclipseTableJSON(const ChartInput& chart)
{
	return BuildEclipseJSON("lunar", LunarEclipseTable(chart));
}
