#include "../../include/utils/utils.h"
#include "../../include/models/settings.h"
#include "../../include/models/chart_data.h"
#include "../../include/utils/TransU.h"
#include "../../include/data/signs.h"
#include "../../include/core/planet.h"
#include "../../include/core/synastry.h"
#include "../../include/core/aspects.h"
#include <math.h>

#include <string.h>
extern US us;
extern WI wi;
extern IS is;
extern CI ciMain;
extern bool IsPDsChartWithoutTable;

const byte ignore7[rrMax] = {0, 1, 1, 0, 1};  // Restrictions for rulership types.

const char* get_weekday_name(int day) {
	static const char* weekdays[] = {
		"Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday", "Sunday"
	};
	return (day >= 1 && day <= 7) ? weekdays[day - 1] : "";
}

// Converts a float number to deg min sec
void decToDeg(double num, int *d, int *m, int *s)
{
	num = fabs(num);
	int d1 = int(num);
	double part = (num - d1) * 60;
	int m1 = int(part);
	*s = int((part - m1) * 60);
	*d = int(num);
	*m = int(part);
}
/* Given a month, day, and year, convert it into a single Julian day value, */
/* i.e. the number of days passed since a fixed reference date.             */
long MdyToJulian(int mon, int day, int yea) {
	int fGreg = true;
	if (yea < yeaJ2G || (yea == yeaJ2G && (mon < monJ2G || (mon == monJ2G && day < 15))))
		fGreg = false;
	//	long t = floor(swe_julday(yea, mon, day, 12.0, fGreg) + 0.5);
	//	long t1 = floor(swe_julday(yea, mon, day, 12.0, true) + 0.5);
	double testswe = swe_julday(yea, mon, day, 12.0, fGreg) + 0.5;
	return (long)floor(testswe);
}

/* Like above but return a fractional Julian time given the extra info. */
double MdytszToJulian(int mon, int day, int yea, double tim, double dst, double zon)
{
	return (double)MdyToJulian(mon, day, yea) + (DegMin2DecDeg(tim) + DegMin2DecDeg(zon) - DegMin2DecDeg(dst)) / 24.0;
}

/* Take a Julian day value, and convert it back into the corresponding */
/* month, day, and year.                                               */
void JulianToMdy(double JD, int *mon, int *day, int *yea)
{
	double tim;
	swe_revjul(JD, JD >= 2299171.0 /* October 15, 1582 */, yea, mon, day, &tim);
}
/* Return the day of the week (Sunday is 0) of the specified given date. */
int DayOfWeek(int month, int day, int year)
{
	int d;

	d = (int)((MdyToJulian(month, day, year) + 1) % 7);
	return d < 0 ? d + 7 : d;
}

/* Given a zodiac degree, transform it into its Navamsa position, where   */
/* each sign is divided into ninths, which determines the number of signs */
/* after a base element sign to use. Degrees within signs are unaffected. */
double Navamsa(double deg)
{
	int sign, sign2;
	double unit;

	sign = Z2Sign(deg);
	unit = deg - Sign2Z(sign);
	sign2 = Mod12(((sign - 1 & 3) ^ (2 * (sign - 1 & 1))) * 3 + (int)(unit * 0.3) + 1);
	return Sign2Z(sign2) + unit;
}

/* Determine the number of days in a particular month. The year is needed, */
/* too, because we have to check for leap years in the case of February.   */
int DayInMonth(int month, int year)
{
	int d;

	if (month == mSep || month == mApr || month == mJun || month == mNov)
		d = 30;
	else if (month != mFeb)
		d = 31;
	else
	{
		d = 28;
		if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0 || year <= yeaJ2G))
			d++;
	}
	return d;
}


// Given an x and y coordinate, return the angle formed by a line from the
// origin to this coordinate. This is just converting from rectangular to
// polar coordinates, however this doesn't involve the radius here.
double RAngle(double x, double y)
{
  double a;

  if (x != 0.0) 
  {
    if (y != 0.0)
      a = RAtn(y/x);
    else
      a = x < 0.0 ? rPi : 0.0;
  } 
  else
    a = y < 0.0 ? -rPiHalf : rPiHalf;
  if (a < 0.0)
    a += rPi;
  if (y < 0.0)
    a += rPi;
  return a;
}



/* Determine the sign of a number: -1 if value negative, +1 if value */
/* positive, and 0 if it's zero.                                     */
double RSgn(double r)
{
	return r == 0.0 ? 0.0 : RSgn2(r);
}

/* Given an x and y coordinate, return the angle formed by a line from the */
/* origin to this coordinate. This is just converting from rectangular to  */
/* polar coordinates; however, we don't determine the radius here.         */
double Angle(double x, double y)
{
	double a;

	if (x != 0.0)
	{
		if (y != 0.0)
			a = atan(y / x);
		else
			a = x < 0.0 ? rPi : 0.0;
	}
	else
		a = y < 0.0 ? -rPiHalf : rPiHalf;
	if (a < 0.0)
		a += rPi;
	if (y < 0.0)
		a += rPi;
	return a;
}

/* Modulus function for floating point values, where we bring the given */
/* parameter to within the range of 0 to 360.                           */
double Mod(double d)
{
	if (d >= 360.0)			/* In most cases, our value is only slightly */
		d -= 360.0;			/* out of range, so we can test for it and   */
	else if (d < 0.0)		/* avoid the more complicated arithmetic.    */
		d += 360.0;

	if (d >= 0 && d < 360.0)
		return d;
	return (d - floor(d / 360.0) * 360.0);
}

/* Another modulus function, this time for the range of 0 to 2 Pi. */
double ModRad(double r)
{
	while (r >= rPi2)			/* We assume our value is only slightly out of       */
		r -= rPi2;				/* range, so test and never do any complicated math. */
	while (r < 0.0)
		r += rPi2;
	return r;
}

/* Integer division - like the "/" operator but always rounds result down. */
long Dvd(long x, long y)
{
	long z;

	if (y == 0)
		return x;
	z = x / y;
	if (((x >= 0) == (y >= 0)) || x - z * y == 0)
		return z;
	return z - 1;
}

/* A similar modulus function: convert an integer to value from 1..12. */
int Mod12(int i)
{
	while (i > 12)
		i -= 12;
	while (i < 1)
		i += 12;
	return i;
}

/* Convert an inputed fractional degrees/minutes value to a true decimal   */
/* degree quantity. For example, the user enters the decimal value "10.30" */
/* to mean 10 degrees and 30 minutes; this will return 10.5, i.e. 10       */
/* degrees and 30 minutes expressed as a floating point degree value.      */
double DegMin2DecDeg(double d)
{
	return RSgn(d) * (floor(fabs(d)) + RFract(fabs(d)) * 100.0 / 60.0);
}

/* This is the inverse of the above function. Given a true decimal value */
/* for a zodiac degree, adjust it so the degrees are in the integer part */
/* and the minute expressed as hundredths, e.g. 10.5 degrees -> 10.30    */
double DecDeg2DegMin(double d)
{
	return RSgn(d) * (floor(fabs(d)) + RFract(fabs(d)) * 60.0 / 100.0);
}

/* Return the shortest distance between two degrees in the zodiac. This is  */
/* normally their difference, but we have to check if near the Aries point. */
double MinDistance(double deg1, double deg2)
{
	double i;

	i = fabs(deg1 - deg2);
	return i < 180.0 ? i : 360.0 - i;
}

/* This is just like the above routine, except the min distance value  */
/* returned will either be positive or negative based on whether the   */
/* second value is ahead or behind the first one in a circular zodiac. */
double MinDifference(double deg1, double deg2)
{
	double i;

	i = deg2 - deg1;
	if (fabs(i) < 180.0)
		return i;
	return RSgn(i) * (fabs(i) - 360.0);
}

/* Return the degree of the midpoint between two zodiac positions, making */
/* sure we return the true midpoint closest to the positions in question. */
double Midpoint(double deg1, double deg2)
{
	double mid;

	mid = (deg1 + deg2) / 2.0;
	return MinDistance(deg1, mid) < 90.0 ? mid : Mod(mid + 180.0);
}

/* Given a zodiac degree, transform it into its Decan sign, where each	  */
/* sign is trisected into the three signs of its element. For example,	  */
/* 1 Aries -> 3 Aries, 10 Leo -> 0 Sagittarius, 25 Sagittarius -> 15 Leo. */
// 给定一个黄道度数，将其转换为德肯星座，每个星座都被三等分为其元素的三个星座。例如，1个白羊座->3个白羊座，10个狮子座->0个射手座，25个射手座->15个狮子座
/*
天秤座德坎1出生于9月23日至10月2日
天秤座德坎2出生于10月3日至12日
天秤座德坎3出生于10月13日至22日
*/
double Decan(double deg)
{
	int sign;
	double unit;

	sign = Z2Sign(deg);
	unit = deg - Sign2Z(sign);
	sign = Mod12(sign + 4 * ((int)floor(unit / 10.0)));
	unit = (unit - floor(unit / 10.0) * 10.0) * 3.0;
	return Sign2Z(sign) + unit;
}

/* Do a coordinate transformation: Given a longitude and latitude value,    */
/* return the new longitude and latitude values that the same location      */
/* would have, were the equator tilted by a specified number of degrees.    */
/* In other words, do a pole shift! This is used to convert among ecliptic, */
/* equatorial, and local coordinates, each of which have zero declination   */
/* in different planes. In other words, take into account the Earth's axis. */
void CoorXform(double* azi, double* alt, double tilt)
{
	double x, y, a1, l1;
	double sinalt, cosalt, sinazi, sintilt, costilt;
	sinalt = sin(*alt);
	cosalt = cos(*alt);
	sinazi = sin(*azi);
	sintilt = sin(tilt);
	costilt = cos(tilt);
	x = cosalt * sinazi * costilt;
	y = sinalt * sintilt;
	x -= y;
	a1 = cosalt;
	y = cosalt * cos(*azi);
	l1 = Angle(y, x);
	a1 = a1 * sinazi * sintilt + sinalt * costilt;
	a1 = asin(a1);
	*azi = l1;
	*alt = a1;
}
// Do a coordinate transformation: Given a longitude and latitude value,
// return the new longitude and latitude values that the same location would
// have, were the equator tilted by a specified number of degrees. In other
// words, do a pole shift! This is used to convert among ecliptic, equatorial,
// and local coordinates, each of which have zero declination in different
// planes. In other words, take into account the Earth's axis.
//进行坐标变换：给定经度和纬度值，
//如果赤道倾斜了指定的度数，则返回相同位置的新经度和纬度值。换句话说，做极移！这用于在黄道、赤道和赤道之间转换，
//和局部坐标，每个坐标在不同的平面上具有零赤纬。换句话说，要考虑到地球的轴心。
void CoorXform2(double *azi, double *alt, double tilt)
{
  double x, y, a1, l1;
  double sinalt, cosalt, sinazi, sintilt, costilt;

  *azi = RFromD(*azi); 
  *alt = RFromD(*alt); 
  tilt = RFromD(tilt);
  sinalt = RSin(*alt); 
  cosalt = RCos(*alt); 
  sinazi = RSin(*azi);
  sintilt = RSin(tilt); 
  costilt = RCos(tilt);

  x = cosalt * sinazi * costilt - sinalt * sintilt;
  y = cosalt * RCos(*azi);
  l1 = RAngle(y, x);
  a1 = cosalt * sinazi * sintilt + sinalt * costilt;
  a1 = RAsin(a1);
  *azi = DFromR(l1); 
  *alt = DFromR(a1);
}

double  SIDESIZE = 220;			/* Size of wheel chart information sidebar.    */
#define DEFAULTY    740
#define DEFAULTX    DEFAULTY+SIDESIZE-43		/* Default window size */
GS gs = {
	false, false, false, true, true, false, true, false, false,
	true, true, false, true, false, false, false,
	static_cast<int>(DEFAULTX), static_cast<int>(DEFAULTY), static_cast<int>(ANIMATION_FACTOR_HOURS), 200, 0, 0, 0, 0.0,
	BITMAPMODE, 1, 14.0, 11.0, NULL, oCore, 1111,
	0 // ANSI_CHARSET
};
int FSphereLocal(double azi, double alt, const CIRC *pcr, int *xp, int *yp)
{
  if (gs.fEcliptic)
  {
    azi = Mod(azi - rDegQuad); 
	negV(alt);

    CoorXform2(&azi, &alt, is.latMC - rDegQuad);
    azi = Mod(is.lonMC - azi + rDegQuad);
    CoorXform2(&azi, &alt,-Rad2Deg(is.rObliquity));

    azi = rDegMax - Untropical(azi);
	negV(alt);
  }
  azi = Mod(rDegQuad*3 - (azi + gs.rRot));
  if (gs.rTilt != 0.0)
    CoorXform2(&azi, &alt, gs.rTilt);
  *xp = pcr->xc + (int)((double)pcr->xr * RCosD(azi) * RCosD(alt) - rRound);
  *yp = pcr->yc + (int)((double)pcr->yr * RSinD(alt) - rRound);
  return azi >= rDegHalf;
}

int FSphereEarth(double azi, double alt, const CIRC *pcr, int *xp, int *yp)
{
  azi = Mod(-azi);
  CoorXform2(&azi, &alt, rDegQuad - ciMain.lat);
  return FSphereLocal(azi + rDegQuad, -alt, pcr, xp, yp);
}

int FSpherePrime(double azi, double alt, const CIRC *pcr, int *xp, int *yp)
{
  CoorXform2(&azi, &alt, rDegQuad);
  return FSphereLocal(azi + rDegQuad, alt, pcr, xp, yp);
}

int FSphereMeridian(double azi, double alt, const CIRC *pcr, int *xp, int *yp)
{
  azi = Mod(azi + rDegQuad);
  CoorXform2(&azi, &alt, rDegQuad);
  return FSphereLocal(azi, alt, pcr, xp, yp);
}

double rAspOrb[cAspect + 1] = { 0, 7.0, 7.0, 7.0, 7.0, 6.0, 3.0, 3.0, 1.5, 1.5, 1.0, 1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
double StarOrb = 1.2;
double rObjAdd[cLastMoving + 1] = { 0.0,
	1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};
double CoeffPar = 6.0;
double rObjOrb[cLastMoving + 1] = { 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0
};
double CoeffComp = 2.5;
/* Given an aspect and two objects making that aspect with each other,   */
/* return the maximum orb allowed for such an aspect. Normally this only */
/* depends on the aspect itself, but some objects require narrow orbs,   */
/* and some allow wider orbs, so check for these cases.                  */
double GetOrb(int obj1, int obj2, int asp)
{
	double orb, r;

	orb = rAspOrb[asp];
	r = obj1 > cLastMoving ? StarOrb : rObjOrb[obj1];
	orb = Min(orb, r);
	r = obj2 > cLastMoving ? StarOrb : rObjOrb[obj2];
	orb = Min(orb, r);
	if (obj1 <= cLastMoving && obj2 <= cLastMoving)
	{
		orb += rObjAdd[obj1];
		orb += rObjAdd[obj2];

		if (us.fParallel)
			orb = orb / CoeffPar;
		else if (us.nRel <= rcDual || us.fTransitInf || us.fPrimDirs || IsPDsChartWithoutTable)
			orb = orb / CoeffComp;
	}
	return orb;
}
double rObjOrbA[85] = {
	360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 2.0, 2.0, 360.0, 360.0, 2.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
  2.0, 2.0, 2.0, 2.0, 2.0,
  2.0
};
double GetOrbA(int obj1, int obj2, int asp)
{
	double orb, r;

	orb = rAspOrb[asp];
	r = obj1 > cLastMoving ? StarOrb : rObjOrbA[obj1];
	orb = Min(orb, r);
	r = obj2 > cLastMoving ? StarOrb : rObjOrbA[obj2];
	orb = Min(orb, r);
	if (obj1 <= cLastMoving && obj2 <= cLastMoving)
	{
		orb += rObjAdd[obj1];
		orb += rObjAdd[obj2];

		if (us.fParallel)
			orb = orb / CoeffPar;
		else if (us.nRel <= rcDual || us.fTransitInf || us.fPrimDirs || IsPDsChartWithoutTable)
			orb = orb / CoeffComp;
	}
	return orb;
}

double SphDistance(double lon1, double lat1, double lon2, double lat2)
{
  double dLon, r;

  dLon = RAbs(lon1 - lon2);
  r = RAcosD(RSinD(lat1)*RSinD(lat2) + RCosD(lat1)*RCosD(lat2)*RCosD(dLon));
  return r;
}

void ClearB(pbyte pb, int cb)
{
  while (cb-- > 0)
    *pb++ = 0;
}

/* Convert rectangular to polar coordinates. */
void RecToPol(double X, double Y, double* A, double* R) {
	if (Y == 0.0)
		Y = rSmall;
	*R = sqrt(X * X + Y * Y);
	*A = Angle(X, Y);
}
