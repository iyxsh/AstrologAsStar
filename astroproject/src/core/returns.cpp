// P2/A9-0 太阳返照时刻解算（纯天文定义；见 returns.h 注释）。
#include "../../include/core/returns.h"
#include "../../include/core/ephemeris.h"   /* SetEphemerisPath（主链同款路径） */
#include "../../swe/swisseph/swephexp.h"
#include <math.h>

static void EnsureSwe(void)
{
	static int done = 0;
	if (!done) { SetEphemerisPath(); done = 1; }
}

static double BodyEclLon(int seBody, double jdUt)
{
	double x[6];
	char serr[AS_MAXCH];
	double jde = jdUt + swe_deltat(jdUt);
	EnsureSwe();
	/* 与主链 CalculatePlanetSE（eepp=-1 默认）一致：不设 eph flag → swe 自动
	 * 回退（无 .se1 时 Moshier）；失败再显式 Moshier。 */
	int flag = SEFLG_SPEED;
	if (swe_calc(jde, seBody, flag, x, serr) < 0)
	{
		flag = SEFLG_MOSEPH | SEFLG_SPEED;
		if (swe_calc(jde, seBody, flag, x, serr) < 0)
			return NAN;
	}
	return x[0];
}

static double SunLonAt(double jdUt)
{
	return BodyEclLon(SE_SUN, jdUt);
}

double MoonEclipticLon(double jdUt)
{
	return BodyEclLon(SE_MOON, jdUt);
}

double SunEclipticLon(double jdUt)
{
	return SunLonAt(jdUt);
}

double SolarReturnJulian(double natalSunLonDeg, int targetYear)
{
	double lon0, jd0, jd, delta, targetDelta, lo, hi, mlo, mhi;
	int d, kTarget;

	if (!(natalSunLonDeg >= 0.0 && natalSunLonDeg < 360.0) || targetYear < 1)
		return NAN;

	jd0 = swe_julday(targetYear, 1, 1, 0.0, SE_GREG_CAL);   /* 1 月 1 日 00:00 UT */
	lon0 = SunLonAt(jd0);
	if (isnan(lon0))
		return NAN;

	/* SunLonAt 返回归一化 [0,360)。年内 Sun 单调（日增 ~1°，360 处回卷一次）。
	 * 若年初未达 target：找 raw 自下方首次穿越 target（平滑段，可二分）。
	 * 若年初已过 target：先等回卷（raw 骤降），再找 raw 再次穿越 target。 */
	if (lon0 > natalSunLonDeg + 1e-9)
	{
		/* 等回卷日 */
		double prev = lon0;
		for (d = 1; d <= 370; d++)
		{
			double cur = SunLonAt(jd0 + d);
			if (isnan(cur)) return NAN;
			if (cur < prev - 180.0)   /* 回卷 359→0 */
				break;
			prev = cur;
		}
		if (d > 370) return NAN;
		lo = jd0 + d;
	}
	else if (lon0 >= natalSunLonDeg - 1e-9)
	{
		return jd0;   /* 年初恰为返照（≤1e-9） */
	}
	else
	{
		lo = jd0;
	}

	/* 找 raw 穿越 natalSunLonDeg 的首日（自 lo 起，lo 处 raw < target） */
	if (SunLonAt(lo) >= natalSunLonDeg - 1e-9)
		return lo;   /* 回卷当日已含目标（极小 target 情形） */
	hi = lo;
	for (long dd = (long)(lo - jd0); dd <= 370; dd++)
	{
		hi = jd0 + (double)dd;
		if (SunLonAt(hi) >= natalSunLonDeg)
			break;
	}
	if (hi == lo)
		return NAN;
	/* lo 处 raw < target，hi 处 >= target：二分求解 */
	for (int it = 0; it < 60; it++)
	{
		jd = (lo + hi) / 2.0;
		if (SunLonAt(jd) >= natalSunLonDeg)
			hi = jd;
		else
			lo = jd;
	}
	return (lo + hi) / 2.0;
}

/* 目标日历月内的月亮返照（raw 穿越 natal；0.25d 扫描 + 二分）。
 * 月亮黄经 mod 360 单调增（27.3d/圈），月内可穿越 1~2 次。 */
int LunarReturnJulians(double natalMoonLonDeg, int year, int month,
	double* outJd, int max)
{
	double jdA, jdB, raw, prevRaw, u, uPrev, prevJd, jd, limit, lo, hi;
	int y2, m2, M, count = 0, greg;

	if (!(natalMoonLonDeg >= 0.0 && natalMoonLonDeg < 360.0) || year < 1
		|| month < 1 || month > 12 || max < 1)
		return 0;

	greg = (year > 1582) || (year == 1582 && month >= 10) ? SE_GREG_CAL : SE_JUL_CAL;
	jdA = swe_julday(year, month, 1, 0.0, greg);
	y2 = year; m2 = month + 1;
	if (m2 == 13) { m2 = 1; y2++; }
	jdB = swe_julday(y2, m2, 1, 0.0, greg);

	prevRaw = BodyEclLon(SE_MOON, jdA);
	if (isnan(prevRaw)) return 0;
	u = prevRaw;                       /* unwrapped 起点 */
	M = (int)floor((u - natalMoonLonDeg) / 360.0);
	prevJd = jdA;
	uPrev = u;

	for (double j = 0.25; j <= (jdB - jdA) + 1e-9; j += 0.25)
	{
		jd = jdA + j;
		if (jd > jdB) jd = jdB;
		raw = BodyEclLon(SE_MOON, jd);
		if (isnan(raw)) { prevJd = jd; continue; }
		double dd = raw - prevRaw;
		if (dd > 180.0) dd -= 360.0;
		if (dd < -180.0) dd += 360.0;
		u = uPrev + dd;
		prevRaw = raw;

		/* 穿越 target+360*(M+1) 的次数 */
		while (u >= natalMoonLonDeg + 360.0 * (M + 1) && count < max)
		{
			limit = natalMoonLonDeg + 360.0 * (M + 1);
			/* 括号 [prevJd, jd]（≤0.25d，raw 连续单调穿越 natal） */
			lo = prevJd; hi = jd;
			for (int it = 0; it < 60; it++)
			{
				double mid = (lo + hi) / 2.0;
				if (BodyEclLon(SE_MOON, mid) >= natalMoonLonDeg)
					hi = mid;
				else
					lo = mid;
			}
			outJd[count++] = (lo + hi) / 2.0;
			M++;
			(void)limit;
		}
		prevJd = jd;
		uPrev = u;
	}
	return count;
}
