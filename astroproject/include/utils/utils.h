#ifndef UTILS_H
#define UTILS_H

#if defined(_WIN32)
	#include "windows.h"
	#include "../../swe/swisseph/swedll.h"
    #define STR_CMPI _stricmp
	#define MEM_CCPY _memccpy
	#define STR_DUP _strdup
	#define X_MEMSET x_memset_Intel
#else
	#include "../../swe/swisseph/swephexp.h"
	#include "../../swe/swisseph/sweph.h"
	#include "../../swe/swisseph/swephlib.h"
	#include "../../swe/swisseph/swehouse.h"
	#include "../../swe/swisseph/sweodef.h"

	#include <string.h>
	#define STR_CMPI strcasecmp
	#define MEM_CCPY memccpy
	#define STR_DUP strdup
	#define X_MEMSET x_memset_GCC_CLANG
#endif

typedef unsigned char byte;
typedef unsigned char *pbyte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned int uint;
typedef int KI;

extern const byte ignore7[];

#define cLastMoving 42
#define cStar 75
#define starLo 43
#define starHi (starLo + cStar - 1)

#define yeaJ2G     1582
#define monJ2G     mOct
#define chNull     L'\0'
#define chRet      L'R'

// 数学常量
#define rPi        3.14159265358979323846
#define rPi2       (rPi*2.0)
#define rDegHalf   180.0
#define rDegQuad   90.0
#define rDegRad    (rDegHalf/rPi)
#define rDeg2Rad   (3.14159265358979323846 / 180.0)
#define rRad2Deg   (180.0 / 3.14159265358979323846)
#define Deg2Rad(r) ((r)/rDegRad)
#define Rad2Deg(r) ((r)*rDegRad)

// object comparison
#define FNorm(obj)    FBetween(obj, 0, cLastMoving)
#define FCusp(obj)    FBetween(obj, cuspLo, cuspHi)
#define FAngle(obj)   (obj == oAsc || obj == oNad  || obj == oDes  || obj == oMC)
#define FMinor(obj)   (FCusp(obj) && (obj - oAsc) % 3 != 0)
#define FStar(obj)    FBetween(obj, starLo, starHi)
#define FObject(obj)  ((obj) <= oVesta || (obj) >= uranLo)
#define FThing(obj)   ((obj) <= cThing || (obj) >= uranLo)

#define negV(V) V = -(V)

#define Sq(n) ((n)*(n))
#define RSqr(r) sqrt(r)
#define RLength3(x, y, z) RSqr(Sq(x) + Sq(y) + Sq(z))
#define PtLen(pt) RLength3(pt.x, pt.y, pt.z)
#define PtZero(pt) pt.x = pt.y = pt.z = 0.0;
#define PtSet(pt, a, b, c) pt.x = a; pt.y = b; pt.z = c;
#define PtMul(pt, r) pt.x *= (r); pt.y *= (r); pt.z *= (r);
#define PtDiv(pt, r) pt.x /= (r); pt.y /= (r); pt.z /= (r);
#define PtAdd2(pt, pt2) pt.x += pt2.x; pt.y += pt2.y; pt.z += pt2.z;
#define PtSub2(pt, pt2) pt.x -= pt2.x; pt.y -= pt2.y; pt.z -= pt2.z;
#define PtNeg2(pt, pt2) pt.x = -pt2.x; pt.y = -pt2.y; pt.z = -pt2.z;
#define PtVec(pt, pt1, pt2) pt = pt2; PtSub2(pt, pt1);
#define PtDot(pt1, pt2) (pt1.x*pt2.x + pt1.y*pt2.y + pt1.z*pt2.z)

#define JulianDayFromTime(t) ((t)*36525.0+2415020.0)
#define IoeFromObj(obj) \
  (obj < oMoo ? 0 : (obj <= cLastPlanet ? obj - 2 : obj - uranLo + cLastPlanet - 1))
#define Tropical(deg) (deg - is.rSid + us.rSiderealCorrection)


#define Untropical(deg) ((deg) + is.rSid - us.rZodiacOffset)
#define Max(v1, v2) ((v1) > (v2) ? (v1) : (v2))
#define Min(v1, v2) ((v1) < (v2) ? (v1) : (v2))
#define RSgn2(r) ((r) < 0.0 ? -1.0 : 1.0)
#define RFract(r) ((r) - floor(r))
#define ChCap(ch) ((ch) >= 'a' && (ch) <= 'z' ? (ch) - 'a' + 'A' : (ch))
#define FCapCh(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define FNumCh(ch) ((ch) >= '0' && (ch) <= '9')
#define Ratio(v1, v2, v3) ((v1) + ((v2) - (v1)) * (v3))
#define Sign2Z(s) ((double)(((s)-1)*30))
#define NFloor(r) ((int)floor(r))

#define rPi        3.14159265358979323846
#define rPi2       (rPi*2.0)
#define rPiHalf    (rPi/2.0)
#define rDegMax    360.0
#define rDegHalf   180.0
#define rDegQuad   90.0
#define rDegRad    (rDegHalf/rPi)
#define rEpoch2000 (-24.736467)
#define rAUToKm    149597870.7
#define rLYToAU    63241.07708427
#define rPCToAU    206264.8062471
#define rDayInYear 365.24219
#define rEarthDist 149.59787
#define rJD2000    2451545.0
#define rSmall     (1.7453E-09)
#define rLarge     10000.0
#define rRound     0.5

#define RSgn2(r) ((r) < 0.0 ? -1.0 : 1.0)
#define FBetween(v, v1, v2) ((v) >= (v1) && (v) <= (v2))
#define Ratio(v1, v2, v3) ((v1) + ((v2) - (v1)) * (v3))
#define ZFromS(s) ((double)(((s)-1)*30))
#define SFromZ(r) (((int)(r))/30+1)
#define RFromD(r) ((r)/rDegRad)
#define DFromR(r) ((r)*rDegRad)
#define GFromO(o) ((rDegMax - (o))/10.0)
#define RSin(r) sin(r)
#define RCos(r) cos(r)
#define RTan(r) tan(r)
#define RAtn(r) atan(r)
#define RAsin(r) asin(r)
#define RAcos(r) acos(r)
#define RTanD(r) RTan(RFromD(r))
#define RAbs(r) fabs(r)
#define RAsinD(r) DFromR(RAsin(r))
#define RAngleD(x, y) DFromR(RAngle(x, y))
#define RAtnD(r) DFromR(RAtn(r))
#define FNearR(r1, r2) (RAbs((r1) - (r2)) < 0.001)
#define RSinD(r) sin(Deg2Rad(r))
#define RCosD(r) cos(Deg2Rad(r))
#define Z2Sign(r) (((int)(r))/30+1)

#define FValidMon(mon) FBetween(mon, 1, 12)
#define FValidDay(day, mon, yea) ((day) >= 1 && (day) <= DayInMonth(mon, yea))
#define FValidYea(yea) FBetween(yea, -20000, 20000)
#define FValidTim(tim) ((tim) > -2.0 && (tim) < 24.0 && \
  RFract(fabs(tim)) < 0.60)
#define FValidDst(dst) FValidZon(dst)
#define FValidZon(zon) FBetween(zon, -24.0, 24.0)
#define FValidLon(lon) FBetween(lon, -180.0, 180.0)
#define FValidLat(lat) FBetween(lat, -90.0, 90.0)
#define FValidAspect(asp) FBetween(asp, 0, cAspect)
#define FValidSystem(n) FBetween(n, 0, NUMBER_OF_HOUSE_SYSTEMS - 1)
#define FValidDivision(n) FBetween(n, 1, 2880)
#define FValidOffset(r) FBetween(r, -360.0, 360.0)
#define FValidCenter(obj) \
  (FBetween(obj, oEar, uranHi) && FObject(obj) && (obj) != oMoo)
#define FValidHarmonic(n) FBetween(n, 0, 30000)
#define FValidAstrograph(n) (n > 0 && 160%n == 0)
#define FValidPart(n) FBetween(n, 1, cPart)
#define FValidBioday(n) FBetween(n, 1, 199)
#define FValidScreen(n) FBetween(n, 20, 200)
#define FValidMacro(n) FBetween(n, 1, NUMBER_OF_MACROS)
#define FValidTextrows(n) ((n) == 25 || (n) == 43 || (n) == 50)
#define FValidGlyphs(n) FBetween(n, 0, 2223)
#define FValidGrid(n) FBetween(n, 1, cObj)
#define FValidGraphx(x) (FBetween(x, BITMAPX1, BITMAPX) || (x) == 0)
#define FValidGraphy(y) (FBetween(y, BITMAPY1, BITMAPY) || (y) == 0)
#define FValidRotation(n) FBetween(n, 0, 360-1)

#define cRay       7

extern byte ignore1[];
#define FIgnore(i)  (ignore1[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)
#define FIgnore2(i) (ignore2[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)
#define FIgnore3(i) (ignore3[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)
// Return the minimum great circle distance between two sets of spherical
// coordinates. This is like MinDistance() but takes latitude into account.
#define RAcosD(r) DFromR(RAcos(r))

/* For graphics, this char affects how bitmaps are written. 'N' is written
like with the 'bitmap' program, 'C' is compacted somewhat (files have
less spaces), and 'V' is compacted even more. 'A' means write as rectangular
Ascii text file. 'B' means write as Windows bitmap (.bmp) file. */
#define BITMAPMODE 'B'	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Translate to chart pixel coordinates, that indicate where to draw on a
// chart sphere, for the -XX switch chart. Inputs may be local horizon
// altitude and azimuth coordinates, local horizon prime vertical, local
// horizon meridian, zodiac position and latitude, or Earth coordinates.
typedef struct _CIRC {
  int xc;  // Center coordinates of circle
  int yc;
  int xr;  // Radius of circle
  int yr;
} CIRC;

typedef enum
{
	ANIMATION_FACTOR_SECONDS = 1,
	ANIMATION_FACTOR_MINUTES,
	ANIMATION_FACTOR_HOURS,
	ANIMATION_FACTOR_DAYS,
	ANIMATION_FACTOR_MONTHS,
	ANIMATION_FACTOR_YEARS,
	ANIMATION_FACTOR_DECADES,
	ANIMATION_FACTOR_CENTURIES,
	ANIMATION_FACTOR_MILLENNIA,
	ANIMATION_FACTOR_HERE_AND_NOW,
} animation_factor_t;

// 函数声明
double DegMin2DecDeg(double d);
double MinDifference(double deg1, double deg2);
double Mod(double angle);
// 新增的函数声明
double DecDeg2DegMin(double d);
double MinDistance(double deg1, double deg2);
double Midpoint(double deg1, double deg2);
double ModRad(double angle);
double RAngle(double y, double x);
double RSgn(double value);
int DayInMonth(int month, int year);
void decToDeg(double num, int *d, int *m, int *s);
int DayOfWeek(int month, int day, int year);
const char* get_weekday_name(int day);

// 补充缺失的函数声明
long MdyToJulian(int mon, int day, int yea);
double MdytszToJulian(int mon, int day, int yea, double tim, double dst, double zon);
wchar_t *SzAltitude(double deg);
wchar_t *SzDate(int mon, int day, int yea, int nFormat);
char *SzTime(int hr, int min, int sec);
wchar_t *SzTimeW(int hr, int min, int sec);
wchar_t *SzTime2(int hr, int min, int sec);
char *SzTim(double tim);
wchar_t *SzTimW(double tim);
double Navamsa(double deg);
char *SzZone(double zon);
wchar_t *SzZoneW(double zon);
wchar_t *SzLocation2(double lon, double lat);
double Angle(double x, double y);
long Dvd(long x, long y);
int Mod12(int i);
double Decan(double deg);
void CoorXform(double* azi, double* alt, double tilt);
void CoorXform2(double *azi, double *alt, double tilt);
int FSphereLocal(double azi, double alt, const CIRC *pcr, int *xp, int *yp);
int FSphereEarth(double azi, double alt, const CIRC *pcr, int *xp, int *yp);
int FSpherePrime(double azi, double alt, const CIRC *pcr, int *xp, int *yp);
int FSphereMeridian(double azi, double alt, const CIRC *pcr, int *xp, int *yp);
void JulianToMdy(double JD, int *mon, int *day, int *yea);
double GetOrb(int obj1, int obj2, int asp);
double GetOrbA(int obj1, int obj2, int asp);
double SphDistance(double lon1, double lat1, double lon2, double lat2);
void ClearB(pbyte pb, int cb);
void RecToPol(double X, double Y, double* A, double* R);
#endif // UTILS_H