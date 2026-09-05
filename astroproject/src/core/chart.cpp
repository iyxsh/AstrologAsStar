#include "../../include/core/chart.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/astrolog.h"
#include "../../include/utils/utils.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/core/ephemeris.h"
#include "../../include/core/fixed_stars.h"
#include "../../include/data/signs.h"

extern CI ciCore;
extern CP cp0, cp1, cp2, cp3, cp4, cpPDs, cpBak;
extern int eepp;
extern bool isDayBirth;
extern int hRevers;
extern int cSign;
extern double Longit;
extern double Latit;

int rgObjEso1[90] = {sSag,
	sLeo, sVir, sAri, sGem, sSco, sAqu, sCap, sLib, sCan, sPis,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	sAri, sTau, sGem, sCan, sLeo, sVir, sLib, sSco, sSag, sCap, sAqu, sPis,
	sTau, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int rgObjEso2[90] = {0,
	0, 0, 0, 0, 0, 0, 0, 0, sLeo, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	sVir, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int rgObjHie1[90] = {sGem,
	sLeo, sAqu, sSco, sCap, sSag, sVir, sLib, sAri, sCan, sPis,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	sAri, sTau, sGem, sCan, sLeo, sVir, sLib, sSco, sSag, sCap, sAqu, sPis,
	sTau, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int rgObjHie2[90] = {0,
	0, 0, 0, 0, 0, 0, 0, sLeo, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	sAqu, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int rgObjRay[90] = {3,
	2, 4, 4, 5, 6, 2, 3, 7, 6, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 4, 2, 3, 1, 2, 3, 4, 5, 7, 5, 6,
	1, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 3, 2, 1, 4, 7, 6, 5,    // 3214765: Seven circuit Labyrinth sequence!
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int rgSignRay2[12+1][cRay+1];

double rise_set[2];
int computeRiseSet()
{
	char serr[AS_MAXCH];
	double epheflag = SEFLG_SWIEPH;

	if (eepp == 0)
		epheflag = SEFLG_JPLEPH;
	else if (eepp == 1)
		epheflag = SEFLG_SWIEPH;
	else if (eepp == 2)
		epheflag = SEFLG_MOSEPH;


	int gregflag;
	int year = ciCore.yea;
	int month = ciCore.mon;
	int day = ciCore.day;
	double geo_longitude = -DegMin2DecDeg(ciCore.lon);
	double geo_latitude = DegMin2DecDeg(ciCore.lat);
	double geo_altitude = ciCore.alt;

	if ((long)ciCore.yea * 10000L + (long)ciCore.mon * 100L + (long)ciCore.day < 15821015L)
		gregflag = false;
	else
		gregflag = true;

	double datm[2] = { 1013.25,15.0 };

	double geopos[3] = { geo_longitude,geo_latitude,geo_altitude };

	double trise, tset;

	double tjd = swe_julday(year, month, day, 0, gregflag);
	double dt = geo_longitude / 360.0;
	tjd = tjd - dt;

	// 天文日出被定义为太阳圆盘的顶部出现在地平线上的时间。天文日落被定义为太阳圆盘的顶部消失在地平线以下的时刻。
	// 默认情况下，函数 swe_rise_trans() 遵循天文日出和日落的定义。此外，天文年鉴和报纸也根据这一定义出版天文日出和日落。
	// 印度占星和印度历法对日出和日落有不同的定义。当太阳圆盘的中心正好在地平线上时，他们认为是太阳升起或落下的时候。此外，印度占星的方法忽略大气折射。
	// 此外，印度占星使用地心位置而不是站心位置，忽略太阳的黄道纬度。
	// 为了计算正确的印度占星的上升和下降时间, 标志 SE_BIT_NO_REFRACTION 和 SE_BIT_DISC_CENTER 必须添加到参数 rsmi 中.
	swe_rise_trans(tjd, SE_SUN, (char*)"", epheflag, SE_CALC_RISE | SE_BIT_HINDU_RISING, geopos, datm[0], datm[1], &trise, serr);// 2435033.7361664693
	swe_rise_trans(tjd, SE_SUN, (char*)"", epheflag, SE_CALC_SET | SE_BIT_DISC_CENTER | SE_BIT_NO_REFRACTION, geopos, datm[0], datm[1], &tset, serr);// 2435034.1846803487

	rise_set[0] = trise - ciCore.zon / 24.0;
	rise_set[1] = tset - ciCore.zon / 24.0;

	double d_hour = DegMin2DecDeg(ciCore.tim) - ciCore.dst;

	int	jy, jm, jd;
	int	jy2, jm2, jd2;
	double  jh, jh2;

	int rh, rm, rs;
	swe_revjul(rise_set[0], rise_set[0] >= 2299171.0, &jy, &jm, &jd, &jh);
	decToDeg(jh, &rh, &rm, &rs);  // sunrise time

	int rh2, rm2, rs2;
	swe_revjul(rise_set[1], rise_set[1] >= 2299171.0, &jy2, &jm2, &jd2, &jh2);
	decToDeg(jh2, &rh2, &rm2, &rs2);  // sunset time

	if (d_hour >= jh && d_hour < jh2)  // 2430572.7202830324,2430573.3014239906
	{
		isDayBirth = true;
	}
	else
	{
		isDayBirth = false;
	}

	ciCore.sunset = rise_set[1];
	ciCore.sunrise = rise_set[0];

	// refresh menu
	//EnableMenuItem(wi.hmenu, cmdFirdaria3, !isDayBirth);
	//EnableMenuItem(wi.hmenu, cmdFirdaria1, isDayBirth);
	//EnableMenuItem(wi.hmenu, cmdFirdaria2, isDayBirth);

	return 0;
}


/* This is a subprocedure of CastChart(). Once we have the chart parameters, */
/* calculate a few important things related to the date, i.e. the Greenwich  */
/* time, the Julian day and fractional part of the day, the offset to the    */
/* sidereal, and a couple of other things.                                   */
double ProcessInput(bool fDate)
{
	double Off;
	double x[6], jd, eph_time, ayanamsa;
	char serr[AS_MAXCH];

	int gregflag = SE_GREG_CAL;
	ciCore.tim = RSgn(ciCore.tim) * floor(fabs(ciCore.tim)) + RFract(fabs(ciCore.tim)) * 100.0 / 60.0 + DegMin2DecDeg(ciCore.zon) - DegMin2DecDeg(ciCore.dst);
	ciCore.lon = DegMin2DecDeg(ciCore.lon);
	ciCore.lat = Min(ciCore.lat, 89.9999);		/* Make sure the chart isn't being cast */
	ciCore.lat = Max(ciCore.lat, -89.9999);		/* on the precise north or south pole.  */
	double tmp = DegMin2DecDeg(ciCore.lat);
	ciCore.lat = Deg2Rad(tmp);

	/* if parameter 'fDate' isn't set, then we can assume that the true time */
	/* has already been determined (as in a -rm switch time midpoint chart). */

	if (fDate)
	{
		is.JD = (double)MdyToJulian(ciCore.mon, ciCore.day, ciCore.yea);
		if (!us.fProgressUS || us.fSolarArc)
		{
			is.T = (is.JD + ciCore.tim / 24.0 - 2415020.5) / 36525.0;
		}
		else
		{
			/* Determine actual time that a progressed chart is to be cast for. */
			is.T = ((is.JD + ciCore.tim / 24.0 + (is.JDp - (is.JD + ciCore.tim / 24.0)) / us.rProgDay) - 2415020.5) / 36525.0;
		}
	}

	jd = JulianDayFromTime(is.T);

	if ((long)ciCore.yea * 10000L + (long)ciCore.mon * 100L + (long)ciCore.day < 15821015L)
		gregflag = false;
	else
		gregflag = true;

	//jut = ciCorejhour + jmin / 60.0 + jsec / 3600.0;

	if (us.fSidereal)
		swe_set_sid_mode(us.nSiderealMode, 0, 0);  // for swe_get_ayanamsa()

	eph_time = jd + swe_deltat(jd);
	ayanamsa = swe_get_ayanamsa(eph_time);

	if (ayanamsa > 180.0)
	{
		// swe_get_ayanamsa flips Ayanamsha from 0 to 360 degrees around year 200, 
		// Astrolog doesn't
		ayanamsa -= 360.0;
	}

	// Swiss Ephemeris returns a positive value for ayanamsa
	Off = -ayanamsa;

	// Compute angle that the ecliptic is inclined to the Celestial Equator 计算黄道向天赤道倾斜的角度 eph_time=2435033.5142473509
	swe_calc(eph_time, SE_ECL_NUT, 1, x, serr);


	if (us.fSidereal)
	{
		is.rObliquity = Deg2Rad(x[1]);	// mean obliquity (no nutation)    23.445160778774497
	}
	else
	{
		is.rObliquity = Deg2Rad(x[0]);	// true obliquity with nutation
	}

	us.Nutation = x[2];

	is.rSid = us.fSidereal ? Off + us.rSiderealCorrection : 0.0;
	return Off;
}

double force[NUMBER_OBJECTS];
double CastChart(bool fDate)
{
	CI ci;
	double Off = 0.0, j;
	double  ep1 = 0.0;
	int i;
	is.rSid = 0.0;
	computeRiseSet();
	ci = ciCore;
	Longit = DegMin2DecDeg(ciCore.lon);
	Latit = DegMin2DecDeg(ciCore.lat);
	if (ciCore.mon == -1)
	{
		is.MC = cp0.longitude[oMC];
		is.Asc = cp0.longitude[oAsc];
	}
	else
	{
		Off = ProcessInput(fDate);
		SwissHouse(is.T, ciCore.lon, ciCore.lat * rDegRad, us.nHouseSystem, &is.Asc, &is.MC, &is.RAa, &is.Vtx, &ep1, &is.OB, &Off);
		hRevers = 0;
		if (!ignore1[oMoo] || !ignore1[oNoNode] || !ignore1[oSoNode] || !ignore1[oFor])
		{
			cp0.vel_longitude[oNoNode] = -1.0;
		}
		SetEphemerisPath();
		ComputeWithSwissEphemeris(is.T);
		/* A4: 恒星（若 -U 启用，接入 ComputeStars 填充 cp0.longitude[starLo..starHi]）
		 * 依赖 is.rObliquity/is.rSid/Longit/Latit/oMC 此时均已就绪。
		 * SD 参数与原版 golden 逐字对齐：ComputeStars(us.fSidereal ? 0.0 : -Off)，
		 * 其中 Off = -ayanamsa（tropical 模式下 SD=+ayanamsa 抵消 rEpoch2000 常量并
		 * 施加 J2000→盘面日期的剩余岁差）。曾误传 0.0 导致恒星黄经整体偏移 -24.16°。 */
		if (us.fAllStar)
		{
			ComputeStars(us.fSidereal ? 0.0 : -Off);
		}
		// 南交点
		cp0.longitude[oSoNode] = Mod(cp0.longitude[oNoNode] + 180.0);
		cp0.vel_longitude[oSoNode] = cp0.vel_longitude[oNoNode];

		j = cp0.longitude[oMoo] - cp0.longitude[oSun];

		/* Day/night Part-of-Fortune inversion (matches the original engine:
		 * in the default mode us.nArabicNight==0 "invert night", night
		 * births use j = Sun-Moon (i.e. Fortune = Asc+Sun-Moon). isDayBirth
		 * is set by computeRiseSet() at the top of CastChart(). */
		if (us.nArabicNight < 0) /* Invert always */
		{
			if (isDayBirth)
				negV(j);
		}
		if (us.nArabicNight == 0) /* Invert night */
		{
			if (!isDayBirth)
				negV(j);
		}
		// 福点
		j = fabs(j) < 90.0 ? j : j - RSgn(j) * 360.0;
		cp0.longitude[oFor] = Mod(j + is.Asc);
		cp0.longitude[oVtx] = is.Vtx;
		cp0.longitude[oEP] = ep1; // ep1 105.81259194743023
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			cp0.longitude[cuspLo + i - 1] = cp0.cusp_pos[i];

		if (!us.fHouseAngle)
		{
			cp0.longitude[oAsc] = is.Asc;
			cp0.longitude[oMC] = is.MC;
			cp0.longitude[oDes] = Mod(is.Asc + 180.0);
			cp0.longitude[oNad] = Mod(is.MC + 180.0);
		}
	}
	for (i = 0; i <= cObj; i++)
	{
		if (force[i] != 0.0)
		{
			cp0.longitude[i] = force[i] - 360.0;
			cp0.latitude[i] = cp0.vel_longitude[i] = 0.0;
		}
	}
	ComputeInHouses();			/* Figure out what house everything falls in. */
	ciCore = ci;
	return is.T;
}

char ruler1[cLastMoving + 1] = { 0, 5,  4, 3, 2, 1,  9,10,11, 12, 8, 12, 2, 6, 7, 8, 11, 5, 8, 12, 7, 1, 1,  2, 3, 4, 5,  6, 7, 8, 9, 10, 11, 12, 7,  8, 5,10, 9,  6, 1, 6, 9 };
char ruler2[cLastMoving + 1] = { 0, 0,  0, 6, 7, 8, 12,11, 0, 0,  0, 0,  0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0 };
char exalt[cLastMoving + 1] = { 0, 1,  2, 11,12,10, 4, 7, 8, 9,  6, 4,  4, 10,5, 11,6, 12, 12,9, 12, 10,5,  6, 7, 8, 9, 10, 11,12,1, 2,  3,  4,  3,  6, 1, 9, 11, 8, 5, 3, 12 };
/* Given a cp0.longitude and sign, determine whether: The cp0.longitude rules the sign, */
/* the cp0.longitude has its fall in the sign, the cp0.longitude exalts in the sign, or */
/* is debilitated in the sign; and return an appropriate character.       */
wchar_t Dignify(int obj, int sign,bool chs)
{
	// sRFED = "RFed"      R=擢升   F擢升  e陷    d落  //入庙  同入庙  失势  同失势  耀升   落陷
	// 
	if (obj > cLastMoving)
		return L' ';
	if (ruler1[obj] == sign || ruler2[obj] == sign) {
		if (chs)
			return L'庙';
		else
			return L'R';
	}
	if (ruler1[obj] == Mod12(sign + 6) || ruler2[obj] == Mod12(sign + 6)) {
		if (chs)
			return L'旺';
		else {
			return L'F';
		}
	}
	if (exalt[obj] == sign) {
		if (chs)
			return L'陷';
		else {
			return L'e';
		}
	}
	if (exalt[obj] == Mod12(sign + 6)) {
		if (chs)
			return L'落';
		else {
			return L'd';
		}
	}

	if (!chs)
		return L'-';
	else
		return L'—';
}
char *DignifyA(int obj, int sign)
{
  static char szDignify[7];
  int sign2 = Mod12(sign+6), ray, ich;

  sprintf(szDignify, "-_____");
  if (obj > oNorm)
    goto LExit;

  // Check standard rulerships.
  if (!ignore7[rrStd]){
    if (ruler1[obj] == sign || ruler2[obj] == sign)
      szDignify[rrStd+1] = 'R';
    else if (ruler1[obj] == sign2 || ruler2[obj] == sign2)
      szDignify[rrStd+1] = 'd';
  }
  if (!ignore7[rrExa]){
    if (exalt[obj] == sign)
      szDignify[rrExa+1] = 'X';
    else if (exalt[obj] == sign2)
      szDignify[rrExa+1] = 'f';
  }

  // Check esoteric rulerships.
  if (!ignore7[rrEso]){
    if (rgObjEso1[obj] == sign || rgObjEso2[obj] == sign)
      szDignify[rrEso+1] = 'S';
    else if (rgObjEso1[obj] == sign2 || rgObjEso2[obj] == sign2)
      szDignify[rrEso+1] = 's';
  }
  if (!ignore7[rrHie]){
    if (rgObjHie1[obj] == sign || rgObjHie2[obj] == sign)
      szDignify[rrHie+1] = 'H';
    else if (rgObjHie1[obj] == sign2 || rgObjHie2[obj] == sign2)
      szDignify[rrHie+1] = 'h';
  }
  if (!ignore7[rrRay]){
    ray = rgObjRay[obj];
    if (ray > 0){
      if (rgSignRay2[sign][ray] > 0)
        szDignify[rrRay+1] = 'Y';
      else if (rgSignRay2[sign2][ray] > 0)
        szDignify[rrRay+1] = 'z';
    }
  }

LExit:
  // Put "most significant" rulership state present in the first character.
  // Order: Standard rulership, exaltation, esoteric, Hierarchical, Ray.
  for (ich = 1; ich <= rrMax; ich += (ich == 1 ? 3 :
    (ich == 4 ? -2 : (ich == 3 ? 2 : 1))))
  {
    if (szDignify[ich] != '_')
	{
      szDignify[0] = szDignify[ich];
      break;
    }
  }
  return szDignify;
}