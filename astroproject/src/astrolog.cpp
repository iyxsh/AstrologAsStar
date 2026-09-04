/*
char
About 30 deg strip:
https://passion-astrologue.com/regle-30-degres-astuce-interpretation/

** IMPORTANT NOTICE: The graphics database and chart display routines
** used in this program are Copyright (C) 1991-1998 by Walter D. Pullen
** (Astara@msn.com, http://www.magitech.com/~cruiser1/astrolog.htm).
** Permission is granted to freely use and distribute these routines
** provided one doesn't sell, restrict, or profit from them in any way.
** Modification is allowed provided these notices remain with any
** altered or edited versions of the program.
**
** The main planetary calculation routines used in this program have
** been Copyrighted and the core of this program is basieadercally a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for personal use but not to sell them or profit from
** them in any way.
**
** The PostScript code within the core graphics routines are programmed
** and Copyright (C) 1992-1993 by Brian D. Willoughbyopenfile
** (brianw@sounds.wa.com). Conditions are identical to those above.
**
** The extended accurate ephemeris databases and formulas are from the
** calculation routines in the library SWISS EPHEMERIS and are
** programmed and copyright 1998 by Astrodienst AG. The use of that
** source code is subject to the Swiss Ephemeris Public License,
** available at http://www.astro.ch/swisseph.
**
** This copyright notice must not be changed or removed
** by any user of this program.
**
** Initial programming 8/28,30, 9/10,13,16,20,23, 10/3,6,7, 11/7,10,21/1991.
** X Window graphics initially programmed 10/23-29/1991.
** PostScript graphics initially programmed 11/29-30/1992.
** Modifications from version 5.40 to 5.41 are by Alois Treindl.
** Modifications from version 5.41 to 5.41g are by Valentin Abramov.
** Windows 32-bits version by Antonio Ramos.
**
** Portions of this code are from Astrolog 6.00 and beyond, which use the
** GNU General Public License as published by the Free Software Foundation;
** either version 2 of the License, or (at your option) any later version.
** See the GNU General Public License for more details, at http://www.gnu.org
**
** This software is provided "as is", without warranty of any kind, either
** express or implied. In no event shall the authors or copyright holders be
** liable for any claim, damages or other liability, arising from the use or
** performance of this software.
*/

#include "astrolog.h"

#define _CRT_SECURE_NO_DEPRECATE
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500 
#endif
#define WIN32_LEAN_AND_MEAN 1
#ifdef _MSC_VER
#pragma comment ( linker,"/ALIGN:4096" )
#pragma warning( disable : 4706 4057 4131 4244 4701 4100 4005 4996)
#endif

#define WIN

#define INTERPRET	/* Comment out this #define if you don't want the ability */
					/* to display interpretations of the various chart types. */

#define BIORHYTHM	/* Comment out this #define if you don't want the    */
					/* non-astrological biorhythm charts in the program. */

/*
** DATA CONFIGURATION SECTION: These settings describe particulars of
** your own location and where the program looks for certain info. It is
** recommended that these values be changed appropriately, although the
** program will still run if they are left alone.
*/

#define DEFAULT_LONG 0.0	/* Change these values to the longitude west    */
#define DEFAULT_LAT  0.0	/* and latitude north of your current location. */
/* Use negative values for east/southern areas. */

#define DEFAULT_ZONE 0.0	/* Change this number to the time zone of your */
							/* current location in hours before (west of)  */
							/* GMT. Use negative values for eastern zones. */

/*
** OPTIONAL CONFIGURATION SECTION: Although not necessary, one may like
** to change some of the values below: These constants affect some of
** the default parameters and other such things.
*/

#define DEFAULT_SYSTEM 0	/* Normally, Placidus houses are used (unless the */
/* user specifies otherwise). If you want a       */
/* different default system, change this number   */
/* to a value from 0..9 (values same as in -c).   */

#define DIVISIONS 48	/* Greater numbers means more accuracy but slower  */
						/* calculation, of exact aspect and transit times. */

/* Name of file to look in for default program parameters (which will */
/* override the compile time values here, if the file exists).        */
#define CONFIGURATION_FILEW L"astrolog32.dat"
#define AUTORUN_FILEW L"autorun.dat"

#define WHEELCOLS		20	/* Affects width of each house in wheel display.    */
#define WHEELROWS		11	/* Max no. of objects that can be in a wheel house. */
#define SCREENWIDTH		120	/* Number of columns to print interpretations in.   */
#define MONTHSPACE		3	/* Number of spaces between each calendar column.   */
#define MAXINDAY		450	/* Max number of aspects or transits displayable.   */
#define MAXCROSS		750	/* Max number of latitude crossings displayable.    */
#define CREDITWIDTH		74	/* Number of text columns in the -Hc credit screen. */
#define MAXSWITCHES		32	/* Max number of switch parameters per input line.  */
#define PSGUTTER		9	/* Points of white space on PostScript page edge.   */

/*
** By the time you reach here and the above values are customized as
** desired, Astrolog is ready to be compiled! Be sure to similarly
** change the values in the astrolog.dat file, which will override any
** corresponding compile time values here. Don't change any of the
** values in the section below unless you know what you're doing.
*/

#define BIODAYS      14			/* Days to include in graphic biorhythms.      */

#define chMin1 '\''
#define chSec1 '"'

// Atlas values
#define cchSzAtl 54
#define cchSzZon 13

#define MAX_STRING_NAME_LEN 50
/*
******************************************************************************
** External Include Files.
******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <string>

//#define szTtyCore  "tty"
#define cchSzDef   200
#define cchSzMax   510
/* Array index limits */
#define cZone      69
#define cPart      177
#define cAspConfig 8

// 解析模式枚举
#define pmMon     1
#define pmDay     2
#define pmYea     3
#define pmTim     4
#define pmDst     5
#define pmZon     6
#define pmLon     7
#define pmLat     8
#define pmObject  9
#define pmAspect  10
#define pmHouseSystem 11
#define pmSign    12
#define pmColor   13
#define pmMonEn   14
#define pmSignEn  15
#define pmObjectEn 16
#define pmWeek    18

int CountryID = 1;
int eepp = -1;

char ignoreSO[118];
wchar_t *SzLocation2(double lon, double lat);

bool UsePDsInChart = false;
bool IsPDsChartWithoutTable = false;   // Table

int CurrentRec = 0;
bool isDayBirth;

#define cUran (uranHi - uranLo + 1)	// 9

#define aDir -2
#define aSig -1

typedef struct _GraphicsInternal
{
	int nCurrChart;				/* Current type of chart to create.           */
	bool fMono;					/* Is this a monochrome monitor.              */
	int kiCur;					/* Current color drawing with.                */
	pbyte bm;					/* Pointer to allocated memory.               */
	int cbBmpRow;				/* Horizontal size of bitmap array in memory. */
	wchar_t *szFileOut;			/* Current name of bitmap file (-Xo).         */
	FILE *file;					/* Actual file handle writing graphics to.    */
	int yBand;					/* Vertical offset to current bitmap band.    */
	double rAsc;					/* Degree to be at left edge in wheel charts. */
	bool fFile;					/* Are we making a graphics file.             */
	int nScale;					/* Scale ratio, i.e. percentage / 100.        */
	int nScaleT;				/* Relative scale to draw chart text at.      */
	int nPenWid;				/* Pen width to use when creating metafiles.  */
	int xOffset;				/* Viewport origin.                           */
	int yOffset;
	int xTurtle;				/* Current coordinates of drawing pen.        */
	int yTurtle;
	int xPen;					/* Cached coordinates where last line ended.  */
	int yPen;
	int nScaleText;
	int fDidSphere;      /* Has a chart sphere been drawn once yet?    */
	int nMode;
	bool fBmp;
	int nGridCell;
}
GI;

typedef struct _ElementTable
{
	int coSum;					/* Total objects considered.          */
	int coHemi;					/* Number that can be in hemispheres. */
	int coSign[NUMBER_OF_SIGNS];			/* Number of objects in each sign.    */
	int coHouse[NUMBER_OF_HOUSES];			/* Number of objects in each house.   */
	int coElemMode[4][3];		/* Objects in each elem/mode combo.   */
	int coElem[4];				/* Object in each element.            */
	int coMode[3];				/* Objects in each sign mode.         */
	int coModeH[3];				/* Objects in each house mode.        */
	int coYang;					/* Objects in Fire/Air signs.         */
	int coYin;					/* Objects in Earth/Water signs.      */
	int coLearn;				/* Objects in first six signs.        */
	int coShare;				/* Objects in last six signs.         */
	int coAsc;					/* Objects in Eastern houses.         */
	int coDes;					/* Objects in Western houses.         */
	int coMC;					/* Objects in Southern houses.        */
	int coIC;					/* Objects in Northern houses.        */
}
ET;

#include "./models/version.h"

/*
******************************************************************************
** Global Variables.
******************************************************************************
*/
US us = {
	/* 67Chart types 17*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0,0,0,0,0,0,0,0,0,0,
		/*27 Chart suboptions 43*/
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,0,0,0,0,0,

		/* Table chart types */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0, /*fDebug57*/

		/* Main flags */
		0, 0, 0, 0, 0, 0, 0, 0, 0, true, true,

		/* Main subflags */
		false, 0, 0, 0, 0, 0,

		/* Rare flags */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		/* Value settings */
		0,
		0,
		0,
		DEFAULT_SYSTEM,
		0,
		oEar,
		0,

		0,
		0,
		0,

		1.0,
		0,
		0,
		0,
		DIVISIONS,
		SCREENWIDTH,
		SE_SIDM_FAGAN_BRADLEY,
		0.0,
		DEFAULT_ZONE,
		DEFAULT_LONG,
		DEFAULT_LAT,
		L"",
		true,			// useInternationalAtlas
		1,				//Advanced mode by default
		0, 0,			// ommit (regional) wizard		

		/* Value subsettings */

		4, 5, cPart, 0.0, 365.24219, 1, 1, 24, 0, BIODAYS,0,false,0
};

IS is = {
	false, false, false, false, false, false,
	NULL, NULL, NULL, NULL, 0, 0, 0, 0.0, 0.0, 0.0,
	NULL, 0.0, 0.0, 0.0, 0.0, 0.0
};

// chart info
// month == -1 means "no time or space"

// Longitude and latitude in ciCore may be in degrees or radians, depending on location!
// In CastChart ciCore value is saved and later restored, but in the mean time, in 
// ProcessInput value is changed to radians! Mad!
CI ciCore = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
CI ciMain = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
CI ciTwin = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
CI ciTran = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };

CI ciNatal = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
CI ciNatal2 = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
// planet positions
CP cp0, cp1, cp2, cp3, cp4, cpPDs, cpBak, cpIng, cpSave;
bool IsDoubleReturn = false; // 一个月中有两次月亮返照

double Longit = 0.0;
double Latit = 0.0;

/*
******************************************************************************
** Global Arrays.
******************************************************************************
*/

double spacex[cLastMoving + 1], spacey[cLastMoving + 1];
double spacez[cLastMoving + 1];
int starname[cStar + 1], kObjA[NUMBER_OBJECTS];

/*
******************************************************************************
** Global Tables.
******************************************************************************
*/

const char *szAppName = szAppNameCore;
const wchar_t *szAppNameW = szAppNameCoreW;

const double  rZon[cZone] = {
	10.30, 10.30, 10.30, 10.0, 10.0, 9.30, 9.0, 9.0, 9.0, 9.0, 8.0,
	8.0, 8.0, 8.0, 7.0, 7.0, 7.0, 7.0, 7.0, 6.0, 6.0,
	6.0, 6.0, 6.0, 5.0, 5.0, 5.0, 5.0, 5.0, 4.0, 4.0,
	4.0, 4.0, 4.0, 3.0, 3.0, 3.0, 3.0, 3.0, 2.0, 1.0,
	0.0, 0.0, 0.0, 0.0, -1.0, -2.0, -4.0, -5.0,
	-5.30, -5.30, -5.30, -6.0, -6.30, -7.0, -8.0, -9.0, -9.0, -9.0,
	-9.30, -10.0, -11.0, -11.30, -11.30, -11.30, -12.0, 24.0, 24.0, 24.0
};
#define INTERPRETALT
#ifdef INTERPRETALT
byte NoPrint = 0;
byte InterpretAlt = 0;
#endif

const double rObjDiam[oVesta + 1] = { 12756.0, 1392000.0, 3476.0,
	4878.0, 12102.0, 6786.0, 142984.0, 120536.0, 51118.0, 49528.0, 2300.0,
	320.0, 955.0, 538.0, 226.0, 503.0
};

double PowerPar = 0.25;

int hRevers = 0;
byte PolarMCflip = 0;
byte oscLilith = 0;
bool fSortAspectsByOrbs = 0; // if false, sorting is done by power

GI gi = {
	0, false, -1,
	NULL, 0, NULL, NULL, 0, 0.0, false,
	2, 1, 10, 0, 0, 0, 0, -1, -1
	,1,false
};

WI wi = {
	/*NULL, (HWND)NULL, (HWND)NULL, (HWND)NULL, (HMENU)NULL, (HACCEL)NULL,
	hdcNil, hdcNil,hdcNil, (HWND)NULL, (HPEN)NULL, (HBRUSH)NULL, (HFONT)NULL,*/
	0, 0, 0, 0, 0, 0, 0, -1, -1,
	0, 0, false, true, false, true, false, 1, false,
	false, false, false, false, true, false, 1, 1000, 0, 0, false,
	false, /*fDisableMacroShortcuts*/
	/*NULL,//HFONT h;
	NULL,//HFONT m_hFont;
	NULL,//HFONT m_hFont2;
	*/
	0,//LANGID lid;
	false,//bool chs;
	/*(HFONT)NULL,*/
	0,
	false,
	false
};

#define PrintL() is.szFileScreen? PrintSzW(L"\n"):PrintSzW(L"\n")
#define PrintL2() is.szFileScreen? PrintSzW(L"\n\n"):PrintSzW(L"\n\n")

#define chSig3(A) tSignName[A][0], tSignName[A][1], tSignName[A][2]
#define chSig3C(A) tSignName[A][0], tSignName[A][1], tSignName[A][2]

#define chObj3(A) tObjShortName[A][0], tObjShortName[A][1], tObjShortName[A][2]
#define chObj3C(A) tObjShortName[A][0], tObjShortName[A][1]

#define FSwitchF(f) ((((f) | fOr) & !fAnd) ^ fNot)
#define SwitchF(f) f = FSwitchF(f)

//==================================================================================================

#define JulianDayFromTime(t) ((t)*36525.0+2415020.0)

#define Mon ciMain.mon
#define Day1 ciMain.day
#define Yea ciMain.yea
#define Tim ciMain.tim
#define Zon ciMain.zon
#define Dst ciMain.dst
#define Lon ciMain.lon
#define Lat ciMain.lat

int oNod = 16;

// Return the cp0.longitude or other object that an object orbits, if any.
int ObjOrbit(int obj)
{
  if (FGeo(obj))
    return oEar;
  if (FBetween(obj, oMer, cPlanet) || obj == oEar)
    return oSun;
  if (FCust(obj)){
    return oSun;
  }
  return -1;
}

enum _eclipses {
	etUndefined = -1, // Not checked
	etNone      = 0,  // No eclipse
	etPenumbra  = 1,  // Penumbral eclipse
	etPenumbra2 = 2,  // Total penumbral eclipse
	etPartial   = 3,  // Partial eclipse
	etAnnular   = 4,  // Annular eclipse
	etTotal     = 5,  // Total eclipse
	etMax       = 6,
};

int cSign = 12;
const char* tSuffix[] = {"","st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th", "th", "th" };
const char* tElement[] = { "Fire,","Earth","Air","Water" };
const wchar_t* sTrans = L"trans ";
const wchar_t* sProgr = L"progr";
const char* tAspectAbbrev[] = {"","Con","Opp","Squ","Tri","Sex","Inc","SSx","SSq","Ses","Qui","BQn","SQn","Sep","Nov","BNv","BSp","TSp","QNv"};
const wchar_t* sNatal=L"natal  ";
const wchar_t*  sVernalEquinox = L" (Vernal Equinox)";
const wchar_t* sAutumnalEquinox = L" (Autumnal Equinox)";
const wchar_t* sSummerSolstice = L" (Summer Solstice)";
const wchar_t* sWinterSolstice = L" (Winter Solstice)";
const char* tAspectName[]={"Conjunct","Opposite","Square","Trine","Sextile","Inconjunct","Semisextile","Semisquare","Sesquiquadrate","Quintile","Biquintile","Semiquintile","Septile","Novile","Binovile","Biseptile","Triseptile","Quatronovile"};
const char* szPerson="This person";
const char* tInteract[]={"is %sconnected and fused together with","%sopposes and creates tension with","is %sin conflict with","is %sin harmony with","has %sopportunity for growth, exchange, and harmony in relation with","is %sdifferent from","%sgets new perspectives in relation with","%screates internal friction with","%screates internal agitation with","%screatively relates externally with","%screatively relates internally with"};
const char* tModify[]={"always ","always ","irreconcilably ","always ","much ","completely ","often ","often ","often ","often ","often ","","","","","","","","","","","","somewhat ","somewhat ","somewhat ","somewhat ","some ","somewhat ","sometimes ","sometimes ","sometimes ","sometimes ","sometimes "};
const wchar_t* sTheir=L"their %ls.";
const char* tTherefore[]={"Both parts are prominent in their psyche","Balance is needed","Adaptation is required by both sides","","","They can often relate in a discordant way","","","","","",""};
const char* tSignName[] = { "","Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo", "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces" };
const wchar_t* sNewMoon = L" (New Moon)";
const wchar_t* sFullMoon = L" (Full Moon)";
const wchar_t* sHalfMoon = L" (Half Moon)";
const char* szEclipse[] ={"No", "Penumbral", "Total Penumbral", "Partial", "Annular", "Total"};
const wchar_t* sSolar = L"Solar";
const wchar_t* sLunar = L"Lunar";
const char* tMonth[]={"","January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char* AmPm[] = {"AM", "PM"};
const char* tHouseSystem[] = { "","Placidus", "Koch", "Equal(Asc)", "Campanus", "Meridian", "Regiomontanus", "Porphyry", "Morinus", "Topocentric", "Alcabitius", "Equal(MC)", "Neo-Porphyry", "Whole", "Vedic", "Null", "Shripati" };
const char* tColor[] = {"Color  1", "Color 11", "Color 12", "Color 14", "Color 13", "Color 15", "Color 16", "Color 10", "Color  9", "Color  3", "Color  4", "Color  6", "Color  7", "Color  5", "Color  8", "Color  2"};
const char* sNoAspects = "No Aspects in list!";
const char* sTotalPower = "Total Power: %.2f - Average Power: %.2f\n";
//==================================================================================================

//==================================================================================================

const char* szZon[cZone] = {
	"HST", "HT", "H",
	"CAT",
	"AHS", "HDT", "AHD",
	"YST", "YT", "Y", "YDT",
	"PST", "PT", "P", "PDT", "PWT",
	"MST", "MT", "M", "MDT", "MWT",
	"CST", "CT", "C", "CDT", "CWT",
	"EST", "ET", "E", "EDT", "EWT",
	"AST", "AT", "A", "ADT", "AWT",
	"BST", "BT", "B", "BDT",
	"WAT",
	"GMT", "GT", "G",
	"WET",
	"CET",
	"EET",
	"UZ3",
	"UZ4",
	"IST", "IT", "I",
	"UZ5",
	"NST",
	"SST",
	"CCT",
	"JST", "JT", "J",
	"SAS",
	"GST",
	"UZ1",
	"NZT", "ZT", "Z", "IDL",
	"LMT", "LT", "L"
};

const char* szZonC[cZone] = {
	"夏威夷标准时间", "HT", "H",
	"阿拉斯加中央时间",
	"夏威夷标准时间", "HDT", "AHD",
	"育空标准时间", "YT", "Y", "YDT",
	"太平洋标准时间", "PT", "P", "PDT", "PWT",
	"山地冬季时间", "MT", "M", "MDT", "MWT",
	"中部标准时间", "CT", "C", "CDT", "CWT",
	"东方标准时间", "ET", "E", "EDT", "EWT",
	"大西洋标准时间", "AT", "A", "ADT", "AWT",
	"英国的夏天时间", "BT", "B", "BDT",
	"西非时间",
	"格林威治标准时间", "GT", "G",
	"欧洲西部时间",
	"欧洲中央时间",
	"欧洲东方时间",
	"俄罗斯第三时区",
	"俄罗斯第四时区",
	"印度标准时间", "IT", "I",
	"俄罗斯第五时区",
	"纽芬兰标准时间",
	"萨摩亚标准时间",
	"北京时间",
	"日本标准时间", "JT", "J",
	"加拿大萨斯卡通时间",
	"格鲁吉亚标准时间",
	"俄罗斯第一时区",
	"新西兰时间", "ZT", "Z", "IDL",
	"当地平均时", "LT", "L"
};

//#include "shlobj.h"		// for SHBrowseForFolder
void PrintInDayEvent(int source, int aspect, int dest, int nVoid);

void initElemTable(ET* pet)
{
	pet->coSum = 0;					/* Total objects considered.          */
	pet->coHemi = 0;					/* Number that can be in hemispheres. */
	memset(&pet->coSign, 0, sizeof(pet->coSign));			/* Number of objects in each sign.    */
	memset(&pet->coHouse, 0, sizeof(pet->coHouse));		/* Number of objects in each house.   */
	memset(&pet->coElemMode, 0, sizeof(pet->coElemMode));/* Objects in each elem/mode combo.   */
	memset(&pet->coElem, 0, sizeof(pet->coElem));/* Object in each element.            */
	memset(&pet->coMode, 0, sizeof(pet->coMode));				/* Objects in each sign mode.         */
	memset(&pet->coModeH, 0, sizeof(pet->coModeH));			/* Objects in each house mode.        */
	pet->coYang = 0;					/* Objects in Fire/Air signs.         */
	pet->coYin = 0;					/* Objects in Earth/Water signs.      */
	pet->coLearn = 0;				/* Objects in first six signs.        */
	pet->coShare = 0;				/* Objects in last six signs.         */
	pet->coAsc = 0;					/* Objects in Eastern houses.         */
	pet->coDes = 0;					/* Objects in Western houses.         */
	pet->coMC = 0;					/* Objects in Southern houses.        */
	pet->coIC = 0;					/* Objects in Northern houses.        */
}

/* Fill out tables based on the number of unrestricted planets in signs by	*/
/* element, signs by mode, as well as other values such as the number of	*/
/* objects in yang vs. yin signs, in various house hemispheres (north/south */
/* and east/west), and the number in first six signs vs. second six signs.	*/
/* This is used by the -v chart listing and the sidebar in graphics charts. */
void CreateElemTable(ET *pet) 
{
	int i, s;

	//memset(pet, 0, sizeof(ET));
	initElemTable(pet);
	for (i = 0; i <= cObj; i++)
	{
		if (!FIgnore(i))
		{
			pet->coSum++;
			s = Z2Sign(cp0.longitude[i]);
			pet->coSign[s - 1]++;
			pet->coElemMode[(s - 1) & 3][(s - 1) % 3]++;
			pet->coElem[(s - 1) & 3]++;
			pet->coMode[(s - 1) % 3]++;
			pet->coYang += (s & 1);
			pet->coLearn += (s < sLib);
			if (!FCusp(i))
			{
				pet->coHemi++;
				s = cp0.house_no[i];
				pet->coHouse[s - 1]++;
				pet->coModeH[(s - 1) % 3]++;
				pet->coMC += (s >= sLib);
				pet->coAsc += (s < sCan || s >= sCap);
			}
		}
	}

	pet->coYin = pet->coSum - pet->coYang;
	pet->coShare = pet->coSum - pet->coLearn;
	pet->coDes = pet->coHemi - pet->coAsc;
	pet->coIC = pet->coHemi - pet->coMC;
}

double rOne = 0.999999999;
double rFractal = 0.0;

// Set location parameters in chart info to default location
void SetToHere(CI* ci)
{
	ci->dst = us.dstDef;
	ci->zon = us.zonDef;
	ci->lon = us.lonDef;
	ci->lat = us.latDef;
	ci->alt = us.altDef;
	wcscpy(ci->loc, us.szLocNameDef);
	wcscpy(ci->nam, L"Here And Now");
}

void GetSysCurTime(int *pmonth, int *pday, int *pyear, double *phour,double *pmin,double* psec)
{
#ifdef _WIN32
	SYSTEMTIME	st;
	GetSystemTime(&st);

	*pmonth = st.wMonth;
	*pday = st.wDay;
	*pyear = st.wYear;
	*phour = (double)st.wHour;
	*pmin = (double)st.wMinute;
	*psec = (double)st.wSecond;
#else
	time_t t = time(NULL);
	struct tm tm;
	localtime_r(&t, &tm);
	*pyear = tm.tm_year + 1900;
	*pmonth = tm.tm_mon + 1;
	*pday = tm.tm_mday;
	*phour = (double)tm.tm_hour;
	*pmin = (double)tm.tm_min;
	*psec = (double)tm.tm_sec;
#endif
}


/* Compute the date and time it is right now as the program is running      */
/* using the computer's internal clock. The time return value filled is		*/
/* expressed in the given zone parameter.									*/
//void GetTimeNow(int *pmonth, int *pday, int *pyear, double *ptime,double zon)
void GetTimeNow(int *pmonth, int *pday, int *pyear, double *ptime)
{
	int month,day,year;
	double hour,min,sec;
	GetSysCurTime(&month, &day, &year, &hour, &min, &sec);
	//hour = hour - zon;

	while (hour < 0.0)
	{
		--day;
		hour += 24.0;
	}

	while (hour >= 24.0)
	{
		++day;
		hour -= 24.0;
	}

	if (day < 1)
	{
		if (--month < 1)
		{
			month = 12;
			year--;
		}

		day += DayInMonth(month, year);
	}
	else if (day > DayInMonth(month, year))
	{
		day -= DayInMonth(month, year);

		if (++month > 12)
		{
			month = 1;
			year++;
		}
	}

	*pmonth = month;
	*pday = day;
	*pyear = year;
	*ptime = hour + min / 100.0 + sec / 6000.0;
}

// Set time parameters to now
void SetToNow(CI* ci)
{
	GetTimeNow(&ci->mon, &ci->day, &ci->yea, &ci->tim);
}
// Set location parameters in chart info to default location, and time parameters to now
void SetHereAndNow(CI* ci)
{
	SetToHere(ci);
	SetToNow(ci);
}

void PrintSzW(const wchar_t *sz, ...)
{
	va_list	args;
	wchar_t	buffer[100000];
	wchar_t	szInput[3], *pch;

	va_start(args, sz);
	vswprintf(buffer,sizeof(buffer), sz, args);

	for (pch = buffer; *pch; pch++)
	{
		if (*pch != L'\n')
		{
			is.cchCol++;
			if (us.fClip80 && is.cchCol >= us.nScreenWidth)	/* Clip if need be. */
			{
				//continue;
				is.cchRow++;
				is.cchCol = 1;
			}
		}
		else
		{
			is.cchRow++;
			is.cchCol = 0;
		}

		// PrintSzW(L"áéěíýúůŽŠČŘŘ");
		is.S = stdout;
		if (is.S == stdout)
		{
			if ((*pch > 127 || *pch < 0) /*&& *pch!=-24*/)
			{
				szInput[0] = *pch;
				szInput[1] = chNull;
				szInput[2] = chNull;
				wprintf(L"%ls", szInput);
				//&& *pch!='ô'
				if (
					*pch != 224 && *pch != 242 && *pch != 232 && *pch != 233 && *pch != 238 &&
					*pch != 168 && *pch != 283 && *pch != 244 && *pch != 367 && *pch != 283 &&
					*pch != 237 && *pch != 353 && *pch != 225 && *pch != 253 && *pch != 352 &&
					*pch != 345 && *pch != 382 && *pch != 268 && *pch != 218 && *pch != 328 &&
					*pch != 226 && *pch != 231 && *pch != 226 && *pch != 234 && *pch != 235 &&
					*pch != 239 && *pch != 249 && *pch != 251 && *pch != 252 && *pch != 227 &&
					*pch != 229 && *pch != 229 && *pch != 170 && *pch != 279 && *pch != 281 &&
					*pch != 363 && *pch != 248 && *pch != 363 && *pch != 339 && *pch != 269 &&
					*pch != 230 && *pch != 250 && *pch != 230 && *pch != 381 && *pch != 344 && *pch != 0xb0)
					is.cchCol++;
			}
			else
			{
				if ((byte)* pch >= ' ')
				{
					szInput[0] = *pch;
					szInput[1] = chNull;
					wprintf(L"%ls", szInput);
				}
			}
		}
		else
		{
			putwc(*pch, is.S); //     write to file
		}
		//if (*pch == L'\n' && is.S == stdout && wi.hdcPrint != hdcNil && is.cchRow >= us.nScrollRow)
		//{
		//	/* If writing to the printer, start a new page when appropriate. */
		//	is.cchRow = 0;
		//	EndPage(wi.hdcPrint);
		//	StartPage(wi.hdcPrint);
		//
		//	/* StartPage clobbers all the DC settings for Windows 95, 98, etc,   */
		//	/* however for NT, 2000 and later it doesn't. Here we cater for both */
		//	SetMapMode(wi.hdcPrint, MM_ANISOTROPIC);	/* For SetViewportExtEx */
		//	SetViewportOrgEx(wi.hdcPrint, 0, 0, NULL);
		//	SetViewportExtEx(wi.hdcPrint, wi.xPrint, wi.yPrint, NULL);
		//	SetWindowOrgEx(wi.hdcPrint, 0, 0, NULL);
		//	SetWindowExtEx(wi.hdcPrint, wi.xClient, wi.yClient, NULL);
		//	SetBkMode(wi.hdcPrint, TRANSPARENT);
		//	SelectObject(wi.hdcPrint, wi.hfont);
		//}
	}
	va_end(args);
}


/* Print a string on the screen. A seemingly simple operation, however we */
/* keep track of what column we are printing at after each newline so we  */
/* can automatically clip at the appropriate point, and we keep track of  */
/* the row we are printing at, so we may prompt before screen scrolling.  */
void PrintSz(const char *sz, ...)
{
	if (is.szFileScreen)
	{
		if (strcmp(sz, "\n") == 0)
			PrintSzW(L"\n");
		else if (strcmp(sz, "\n\n") == 0)
			PrintSzW(L"\n");
		else
			PrintSzW(char_to_wchar(sz).c_str());
		return;
	}
	va_list	args;
	unsigned char	buffer[100000];
	char	szInput[3];
	unsigned char *pch;
	memset(buffer, 0, 100000);

	va_start(args, sz);
	vsprintf((char *)buffer, sz, args);

	for (pch = buffer; *pch; pch++)
	{
		if (*pch != '\n')
		{
			is.cchCol++;
			if (us.fClip80 && is.cchCol >= us.nScreenWidth)	/* Clip if need be. */
			{
				continue;
			}
		}
		else
		{
			is.cchRow++;
			is.cchCol = 0;
		}

		if (is.S == stdout)
		{
			if ((*pch > 127 || *pch < 0) /*&& *pch!=-24*/)
			{
				szInput[0] = *pch;
				szInput[1] = *(pch + 1);
				szInput[2] = chNull;
				wprintf(L"%ls", char_to_wchar(szInput).c_str());
				if (1)
					is.cchCol++;

				if (0 && *pch != 224 && *pch != 242 && *pch != 232 && *pch != 233 && *pch != 238 && *pch != 168)
					is.cchCol++;
				pch++;
			}
			else
			{
				if ((byte)* pch >= ' ')
				{
					szInput[0] = *pch;
					szInput[1] = chNull;
					wprintf(L"%ls", char_to_wchar(szInput).c_str());
				}
			}
		}
		else
		{
			putc(*pch, is.S);
		}
	}

	va_end(args);
}

/* Print a single character on the screen. */
void PrintCh(char ch)
{
	wchar_t sz[2];
	sz[0] = ch;
	sz[1] = chNull;				/* Treat char as a string of length one. */
	PrintSzW(sz);				/* Then call above to print the string.  */
}

/* Print a single character on the screen 'n' times */
void PrintTab(char ch, int cch)
{
	int		i;
	wchar_t	sz[2];

	sz[0] = ch;
	sz[1] = chNull;

	for (i = 0; i < cch; i++)
		PrintSzW(sz);
}

wchar_t szZod[12];
wchar_t *SzZodiac(double deg)
{
	int sign, d, m;
	double s;

	switch (us.nDegForm)
	{
	case 0:

		/* Normally, we format the position in degrees/sign/minutes format: */

		deg = Mod(deg + (is.fSeconds ? 0.5 / 60.0 / 60.0 : 0.5 / 60.0));
		sign = (int)deg / 30;
		d = (int)deg - sign * 30;
		m = (int)(RFract(deg) * 60.0);
		if (wi.chs)
		{
			//tSignName 原来位配置文件取 sign + 1 改为数组后 下标从 0 开始，不加 1
			swprintf(szZod, sizeof(szZod) / sizeof(wchar_t), L" %ls|%2d|%02d", char_to_wchar(tSignName[sign+1]).c_str(),d,m);
		}
		else
		{
			wchar_t tstmp[MAX_STRING_NAME_LEN];
			swprintf(tstmp,sizeof(tstmp)/sizeof(wchar_t), L"%ls",char_to_wchar(tSignName[sign+1]).c_str());
			swprintf(szZod, sizeof(szZod) / sizeof(wchar_t), L"%lc%lc%lc|%2d|%02d", tstmp[0], tstmp[1], tstmp[2],d,m);
		}

		if (is.fSeconds)
		{
			s = RFract(deg) * 60.0;
			s = RFract(s) * 60.0;
			if (0)
				swprintf(&szZod[7], sizeof(szZod[7]) / sizeof(wchar_t), L"'%02d\"", (int)s);
			else
				//swprintf(&szZod[8], L"'%02d\"", (int) s);
				swprintf(&szZod[7], sizeof(szZod[7]) / sizeof(wchar_t), L"'%02d\"", (int)s);
		}
		break;

	case 1:
		/* However, if -sh switch in effect, get position in hours/minutes: */

		deg = Mod(deg + (is.fSeconds ? 0.5 / 4.0 / 60.0 : 0.5 / 4.0));
		d = (int)deg / 15;
		m = (int)((deg - (double)d * 15.0) * 4.0);
		swprintf(szZod, sizeof(szZod) / sizeof(wchar_t), L"%2dh,%02dm", d, m);
		if (is.fSeconds)
		{
			s = RFract(deg) * 4.0;
			s = RFract(s) * 60.0;
			//swprintf(&szZod[7], L",%02ds", (int) s);
			if (0)
				swprintf(&szZod[7], sizeof(szZod[7]) / sizeof(wchar_t), L"'%02d\"", (int)s);
			else
				swprintf(&szZod[7], sizeof(szZod[7]) / sizeof(wchar_t), L"'%02d\"", (int)s);
		}
		break;

	default:
		/* Otherwise, if -sd in effect, format position as decimal degrees */

		swprintf(szZod, sizeof(szZod) / sizeof(wchar_t), is.fSeconds ? L"%11.7f" : L"%7.3f", deg);
		break;
	}
	return szZod;
}

/* This is similar to formatting a zodiac degree, but here we return a */
/* string of a (signed) declination value in degrees and minutes.      */
wchar_t szAlt[12];
wchar_t *SzAltitude(double deg)
{
	int d, m, f;
	double s;
	wchar_t DegSign = 176;
	if (us.nDegForm == 2)
	{
		// use decimal degrees
		swprintf(szAlt, sizeof(szAlt) / sizeof(wchar_t), is.fSeconds ? L"%lc%9.6f" : L"%lc%6.3f", deg < 0.0 ? L'-' : L'+', fabs(deg));
	}
	else
	{
		// degrees, minutes, and possibly seconds
		f = deg < 0.0;
		deg = fabs(deg) + (is.fSeconds ? 0.5 / 60.0 / 60.0 : 0.5 / 60.0);
		d = (int)deg;
		m = (int)(RFract(deg) * 60.0);
		//ch = us.fAnsiChar > 1 ? chDeg2 : chDeg1;
		//ch = chDeg1;
		if (!wi.chs)
			swprintf(szAlt, sizeof(szAlt) / sizeof(wchar_t), L"%lc%2d%lc%02d'", f ? L'-' : L'+', d, DegSign, m);
		else
			swprintf(szAlt, sizeof(szAlt) / sizeof(wchar_t), L"%ls%2d%lc%02d'", f ? L" -" : L" +", d, DegSign, m);

		if (is.fSeconds)
		{
			s = RFract(deg) * 60.0;
			s = RFract(s) * 60.0;
			if (0)
				swprintf(&szAlt[7], sizeof(szAlt[7]) / sizeof(wchar_t), L"%02d\"", (int)s);
			else
				swprintf(&szAlt[8], sizeof(szAlt[8]) / sizeof(wchar_t), L"%02d\"", (int)s);
		}
	}
	return szAlt;
}

/* Another string formatter, here we return a date string given a month,    */
/* day, and year. We format with the day or month first based on whether    */
/* the "European" date variable is set or not. The routine also takes a     */
/* parameter to indicate how much the string should be abbreviated, if any. */
wchar_t *SzDate(int mon, int day, int yea, int nFormat)
{
	static wchar_t szDat[20];

	if (wi.chs)
		swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%d.%02d.%02d", yea, mon, day);
	else if (us.fEuroDate)
	{
		switch (nFormat)
		{
		case 2:
			if (wi.chs)
				swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%2d %c%c%c%c%5d", day, tMonth[mon][0], tMonth[mon][1], tMonth[mon][2], tMonth[mon][3], yea);
			else
				swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%2d %c%c%c%5d", day, tMonth[mon][0], tMonth[mon][1], tMonth[mon][2], yea);
			break;
		case 1:
			//swprintf(szDat, L"%d %ls %d", day, Lang(tMonth, mon), yea);
			break;
		case -1:
			swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%2d-%2d-%2d", day, mon, abs(yea) % 100);
			break;
		default:
			swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%02d-%02d-%d", day, mon, yea);
			break;
		}
	}
	else
	{
		switch (nFormat)
		{
		case 3:
			if (wi.chs)
				swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%c%c%c%c %2d, %d", day, tMonth[mon][0], tMonth[mon][1], tMonth[mon][2], tMonth[mon][3], yea);
			else
				swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%c%c%c %2d, %d", day, tMonth[mon][0], tMonth[mon][1], tMonth[mon][2], yea);
			break;
		case 2:
			if (wi.chs)
				swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%c%c%c%c %2d%5d", day, tMonth[mon][0], tMonth[mon][1], tMonth[mon][2], tMonth[mon][3], yea);
			else
				swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%c%c%c %2d%5d", day, tMonth[mon][0], tMonth[mon][1], tMonth[mon][2], yea);
			break;
		case 1:
			//swprintf(szDat, L"%ls %d, %d", Lang(tMonth, mon), day, yea);
			break;
		case -1:
			swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%2d/%2d/%2d", mon, day, abs(yea) % 100);
			break;
		default:
			swprintf(szDat, sizeof(szDat) / sizeof(wchar_t), L"%2d/%2d/%4d", mon, day, yea);
			break;
		}
	}
	return szDat;
}

/* Return a string containing the given time expressed as an hour and */
/* minute quantity. This is formatted in 24 hour or am/pm time based  */
/* on whether the "European" time format flag is set or not.          */
char *SzTime(int hr, int min, int sec)
{
	static char szTim[48];

	while (min >= 60)
	{
		min -= 60;
		hr++;
	}
	while (hr < 0)
		hr += 24;
	while (hr >= 24)
		hr -= 24;

	if (us.fEuroTime)
	{
		if (sec == -1)
			snprintf(szTim, sizeof(szTim), "%2d:%02d", hr, min);
		else
			snprintf(szTim, sizeof(szTim), "%2d:%02d:%02d", hr, min, sec);
	}
	else
	{
		if (sec == -1)
			//snprintf(szTim, sizeof(szTim), "%2d:%02d%ls", Mod12(hr), min, hr < 12 ? L"AM" : L"PM");
			snprintf(szTim, sizeof(szTim), "%2d:%02d%s", Mod12(hr), min, hr < 12 ? AmPm[0] : AmPm[1]);
		else
			snprintf(szTim, sizeof(szTim), "%2d:%02d:%02d%s", Mod12(hr), min, sec, hr < 12 ? AmPm[0] : AmPm[1]);
	}
	return szTim;
}

wchar_t *SzTimeW(int hr, int min, int sec)
{
	static wchar_t szTim[12];

	while (min >= 60)
	{
		min -= 60;
		hr++;
	}
	while (hr < 0)
		hr += 24;
	while (hr >= 24)
		hr -= 24;

	if (us.fEuroTime)
	{
		if (sec == -1)
			swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%2d:%02d", hr, min);
		else
			swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%2d:%02d:%02d", hr, min, sec);
	}
	else
	{
		if (sec == -1)
			swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%2d:%02d%ls", Mod12(hr), min, hr < 12 ? L"AM":L"PM");
		else
			swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%2d:%02d:%02d%ls", Mod12(hr), min, sec, hr < 12 ? L"AM" : L"PM");
	}
	return szTim;
}

wchar_t *SzTime2(int hr, int min, int sec)
{
	static wchar_t szTim[12];

	while (min >= 60)
	{
		min -= 60;
		hr++;
	}
	while (hr < 0)
		hr += 24;
	while (hr >= 24)
		hr -= 24;

	if (us.fEuroTime)
	{
		if (sec == -1)
			swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%d:%02d", hr, min);
		else
			swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%d:%02d:%02d", hr, min, sec);
	}
	else
	{
		if (sec == -1)
			swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%d:%02d%ls", Mod12(hr), min, hr < 12 ? L"AM" : L"PM");
		else
			swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%d:%02d:%02d%ls", Mod12(hr), min, sec, hr < 12 ? L"AM" : L"PM");
	}
	return szTim;
}

/* This just determines the correct hour and minute and calls the above. */
char *SzTim(double tim)
{
	/* Corrected by Michael Rideout on February 15, 2004: */
	int d, m, s;
	int hr = 0, min = 0, sec = 0;
	double rMin;

	hr = NFloor(tim);
	rMin = (tim - hr) * 100.0;
	min = (int)(rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	sec = (int)(60.0 * rFractal + 0.5);
	
	decToDeg(DegMin2DecDeg(tim) + 0.50 / 3600.0, &d, &m, &s);
	return SzTime(d, m, s);
}

wchar_t *SzTimW(double tim)
{
	/* Corrected by Michael Rideout on February 15, 2004: */
	int d, m, s;
	int hr = 0, min = 0, sec = 0;
	double rMin;

	hr = NFloor(tim);
	rMin = (tim - hr) * 100.0;
	min = (int)(rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	sec = (int)(60.0 * rFractal + 0.5);

	decToDeg(DegMin2DecDeg(tim) + 0.50 / 3600.0, &d, &m, &s);
	static wchar_t szTim[12];
	swprintf(szTim, sizeof(szTim) / sizeof(wchar_t), L"%2d:%02d:%02d", d, m, s);
	return szTim;
}


/* Return a string containing the given time zone, given as a real value     */
/* having the hours before GMT in the integer part and minutes fractionally. */
char *SzZone(double zon)
{
	static char szZon[48];
	int mini, seci;
	double rMin;

	rMin = RFract(fabs(zon)) * 100.0;
	mini = (int)(rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	seci = (int)(60.0 * rFractal + 0.5);
	if (seci == 60)
	{
		int h;
		double dlat = DegMin2DecDeg(zon);
		decToDeg(dlat, &h, &mini, &seci);
	}

	snprintf(szZon, sizeof(szZon), "%c%d:%02d:%02d", zon > 0.0 ? '-' : '+', (int)fabs(zon),
		(int)(RFract(fabs(zon)) * 100.0 + 0.5 / 60.0), seci);
	return szZon;
}

wchar_t *SzZoneW(double zon)
{
	static wchar_t szZon[10];
	int mini, seci;
	double rMin;

	rMin = RFract(fabs(zon)) * 100.0;
	mini = (int)(rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	seci = (int)(60.0 * rFractal + 0.5);
	if (seci == 60)
	{
		int h;
		double dlat = DegMin2DecDeg(zon);
		decToDeg(dlat, &h, &mini, &seci);
	}

	swprintf(szZon, sizeof(szZon) / sizeof(wchar_t), L"%c%d:%02d:%02d", zon > 0.0 ? L'-' : L'+', (int)fabs(zon),
		(int)(RFract(fabs(zon)) * 100.0 + 0.5 / 60.0), seci);

	return szZon;
}

wchar_t chDeg3 = 176;//L'°';//°
wchar_t *SzLocation2(double lon, double lat)
{
	static wchar_t szLoc[32];
	int mini, minj, seci, secj;
	double rMin;
	unsigned char ch2, ch3;
	wchar_t ch1 = chDeg3;

	rMin = RFract(fabs(lon)) * 100.0;
	mini = (int)(rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	seci = (int)(60.0 * rFractal + 0.5);
	if (seci == 60)
	{
		int h;
		double dlat = DegMin2DecDeg(lon);
		decToDeg(dlat, &h, &mini, &seci);
	}

	rMin = RFract(fabs(lat)) * 100.0;
	minj = (int)(rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	secj = (int)(60.0 * rFractal + 0.5);
	if (secj == 60)
	{
		int h;
		double dlat = DegMin2DecDeg(lat);
		decToDeg(dlat, &h, &minj, &secj);
	}

	ch2 = chMin1;
	ch3 = chSec1;
	if (us.fAnsiChar)
	{
		int a = floor(fabs(lat));
		swprintf(szLoc, sizeof(szLoc) / sizeof(wchar_t), L"%3d%lc%02d%lc%02d%3d%lc%02d%lc%02d",
			(int)floor(fabs(lon)), lon < 0.0 ? L'E' : L'W', mini, ch2, seci,
			a, lat < 0.0 ? L'S' : L'N', minj, ch2, secj);
	}
	else
	{
		int a = floor(fabs(lat));
		swprintf(szLoc, sizeof(szLoc) / sizeof(wchar_t), L"%3d%lc%02d%lc%02d%lc%lc%3d%lc%02d%lc%02d%lc%lc",
			(int)floor(fabs(lon)), ch1, mini, ch2, seci, ch3,
			lon < 0.0 ? L'E' : L'W', a, ch1, minj, ch2, secj, ch3, lat < 0.0 ? L'S' : L'N');
	}
	return szLoc;
}

#define PrintAltitude(deg) PrintSzW(SzAltitude(deg))

/* Print header info showing time and date of the chart being displayed.   */
/* This is used by ChartListing() and the -l sector chart in PrintChart(). */
void PrintHeader()
{
	wchar_t sz[cchSzDef];
	wchar_t szW[cchSzDef];
	int day, fNam, fLoc;
	CI ciT;

	ciT = ciMain;
	if (us.fProgressUS && !us.nRel)
	{
		ciMain = ciCore;
	}

	fNam = *ciMain.nam > chNull;
	fLoc = *ciMain.loc > chNull;

	if (us.nRel == rcDual)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t),L"%ls",szAppNameW);
		PrintSzW(sz);
	}
	else if (us.nRel == rcSynastry)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", szAppNameW);
		PrintSzW(sz);
	}
	else if (us.nRel == rcProgTran)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls chart ", szAppNameW);
		PrintSzW(sz);

		if (wcscmp(ciMain.nam, L"") == 0)
		{
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls%ls", L"for ", L"Here And Now:\n");
		}
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t),ciMain.nam);
		PrintSzW(sz);
		wprintf(L"\n");
		day = DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea);
		const char* daystr = get_weekday_name(day);
		char day3str[4] = {};
		sprintf(day3str, "%c%c%c", daystr[0], daystr[1],daystr[2]);
		if (wi.chs)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls %ls %ls (%lcT %ls GMT)", char_to_wchar(daystr).c_str(), SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), SzTimW(ciMain.tim), ChDstW(ciMain.dst), SzZoneW(ciMain.zon));
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls %ls %ls (%lcT %ls GMT)", char_to_wchar(day3str).c_str(), SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), SzTimW(ciMain.tim), ChDstW(ciMain.dst), SzZoneW(ciMain.zon));
		PrintSzW(sz);
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%lc%ls%ls%ls\n", fLoc && !fNam ? L'\n' : L' ',
			ciMain.loc, fLoc ? L" " : L"", SzLocation2(ciMain.lon, ciMain.lat));
		PrintSzW(sz);

		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", L"sNatalHouses3");
		PrintSzW(sz);
	}
	else if (us.f12parts2)
	{
		if (CountryID == 1)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Dodekatemoria Chart", szAppNameW);
		else if (CountryID == 4)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls 十二分盘", szAppNameW);
		else if (CountryID == 2)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Thème Dodécatémorie", szAppNameW);
		else if (CountryID == 3)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Carta Dodekatemoria", szAppNameW);
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Dodekatemoria Chart", szAppNameW);

		PrintSzW(sz);
	}
	if (us.fNavamsas2)
	{
		if (CountryID == 1)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Navamsa Chart", szAppNameW);
		else if (CountryID == 4)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls 九分盘", szAppNameW);
		else if (CountryID == 2)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Thème Navamsa", szAppNameW);
		else if (CountryID == 3)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Carta Navamsa", szAppNameW);
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Navamsa Chart", szAppNameW);

		PrintSzW(sz);
	}
	if (us.fAnti) // 映点
	{
		if (CountryID == 1)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Antiscia Chart", szAppNameW);
		else if (CountryID == 4)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls 映点盘", szAppNameW);
		else if (CountryID == 2)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Thème Antisces", szAppNameW);
		else if (CountryID == 3)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Carta Antiscia", szAppNameW);
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Antiscia Chart", szAppNameW);

		PrintSzW(sz);
	}
	if (us.fAnti2) // 反映点
	{
		if (CountryID == 1)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Contra Antiscia  Chart", szAppNameW);
		else if (CountryID == 4)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls 反映点盘", szAppNameW);
		else if (CountryID == 2)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Thème Contre - Antisces", szAppNameW);
		else if (CountryID == 3)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Carta Contra Antiscia", szAppNameW);
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls Contra Antiscia  Chart", szAppNameW);
		PrintSzW(sz);
	}
	else
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\n%ls chart ", szAppNameW);
		PrintSzW(sz);
	}

	if (ciMain.mon == -1)
	{
		PrintSzW(L"No time or space");
		PrintSzW(L"\n");
	}
	else if (us.nRel == rcComposite)
	{
		PrintSzW(L"Composite chart");
		PrintSzW(L"\n");
	}
	else
	{
		if (us.nRel == rcMidpoint)
		{
			PrintSzW(L"Time and space Midpoint");
			PrintSzW(L"\n");
		}
		else
		{
			if (us.nRel != rcProgTran)
			{
				if (wcscmp(ciMain.nam, L"") == 0)
				{
					swprintf(sz,sizeof(sz)/sizeof(wchar_t),L"%ls%ls", L"for ", L"Here And Now:");
					PrintSzW(sz);
					wprintf(L"\n");
				}
				else
				{
					swprintf(szW, sizeof(szW) / sizeof(wchar_t) ,L"%ls%ls", L"for ", ciMain.nam);
					PrintSzW(szW);
					wprintf(L"\n");
				}
			}
		}
		if (us.nRel == rcProgTran)
		{
			day = DayOfWeek(ciTwin.mon, ciTwin.day, ciTwin.yea);
			const char* daystr = get_weekday_name(day);
			char day3str[4] = {};
			sprintf(day3str, "%c%c%c", daystr[0], daystr[1], daystr[2]);
			if (wi.chs)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls %ls %ls (%lcT %ls GMT)", char_to_wchar(daystr).c_str(), SzDate(ciTwin.mon, ciTwin.day, ciTwin.yea, 3), SzTimW(ciTwin.tim), ChDstW(ciTwin.dst), SzZoneW(ciTwin.zon));
			else
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls %ls %ls (%lcT %ls GMT)", char_to_wchar(day3str).c_str(), SzDate(ciTwin.mon, ciTwin.day, ciTwin.yea, 3), SzTimW(ciTwin.tim), ChDstW(ciTwin.dst), SzZoneW(ciTwin.zon));
			PrintSzW(sz);
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%lc%ls%ls%ls\n", fLoc && !fNam ? L'\n' : L' ', ciTwin.loc, fLoc ? L" " : L"", SzLocation2(ciTwin.lon, ciTwin.lat));
			PrintSzW(sz);
			return;
		}
		else
		{
			day = DayOfWeek(ciNatal.mon, ciNatal.day, ciNatal.yea);
			const char* daystr = get_weekday_name(day);
			char day3str[4] = {};
			sprintf(day3str, "%c%c%c", daystr[0], daystr[1], daystr[2]);
			if (wi.chs)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls %ls %ls (%lcT %ls GMT)", char_to_wchar(daystr).c_str(), SzDate(ciNatal.mon, ciNatal.day, ciNatal.yea, 3), SzTimW(ciNatal.tim), ChDstW(ciNatal.dst), SzZoneW(ciNatal.zon));
			else
			{
				//wchar_t *a = Lang(tDay, day);
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls %ls %ls (%lcT %ls GMT)", char_to_wchar(daystr).c_str(), SzDate(ciNatal.mon, ciNatal.day, ciNatal.yea, 3), SzTimW(ciNatal.tim), ChDstW(ciNatal.dst), SzZoneW(ciNatal.zon));
			}
			PrintSzW(sz);
			if (us.nRel == rcMidpoint)
			{
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" %ls\n", SzLocation2(ciMain.lon, ciMain.lat));
				PrintSzW(sz);
			}
			else
			{
				swprintf(szW,
					sizeof(szW)/sizeof(wchar_t),
					L"%ls%ls%ls%ls\n",
					fLoc && !fNam ? L"\n" : L" ",
					ciMain.loc, fLoc ? L" " : L"",
					SzLocation2(ciMain.lon, ciMain.lat));
				//PrintSzW(szW);
				wprintf(L"%ls",szW);
			}
		}
	}
	if (us.fHorizon)
	{
		if (us.fPrimeVert)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Prime Vertical\n");
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Local Horizon\n");
		PrintSzW(sz);
	}

	if (us.fFirdaria)
	{
		if (isDayBirth)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Firdaria for day birth\n");
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Firdaria for night birth\n");
		PrintSzW(sz);
	}
	if (us.fPlanetaryHours)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Planetary Hours");
		PrintSzW(sz);
	}

	if (us.fProgressUS || us.nRel == rcTransit || us.nRel == rcDual
		|| us.nRel == rcSynastry || us.nRel == rcProgress || us.fTransitInf || us.nRel == rcProgTran)
		/*|| us.nRel == rcSynastry || us.nRel == rcProgress || us.fTransitInf)*/
	{
		if (us.fProgressUS && !us.nRel)
		{
			if (us.fSolarArc == 1)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Chart directed (Degree per Year/Month) to\n");
			else
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Chart progressed to:");
		}
		else if (us.fTransitInf)
		{
			if (!is.fProgressIS)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\nTransits for:\n");
			else
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\nProgress for:\n");
		}
		else
		{
			if (us.nRel == rcProgress)
			{
				if (!us.fInterpret && (us.fInfluence || us.fListing))
				{
					if (us.fSolarArc == 1)
						swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Natal houses, planets directed (Degree per Year/Month) to\n");
					else
					{
						if (us.nRel == rcProgress)
							swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\nNatal houses, planets progressed to\n");
						else
							swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\nNatal houses, planets progressed (and transit) to\n");
					}
				}
				else
				{
					swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Progressions for\n");
				}
			}
			else if (us.nRel == rcTransit)
			{
				if (!us.fInterpret && (us.fInfluence || us.fListing))
					swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\nNatal houses, transit planets for:\n");
				else
					swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\nTransits for:\n");
			}
			else if (us.nRel == rcSynastry)
			{
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"planets for ");
			}
			else if (us.nRel == rcDual)
			{
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"\nand ");
			}
			else
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"and\n");
		}
		PrintSzW(sz);
		if (us.fProgressUS && !us.nRel)
			ciMain = ciT;
		else if (us.fTransitInf)
			ciMain = ciTran;
		else
			ciMain = ciTwin;
		if (us.nRel == rcDual || us.nRel == rcSynastry)
		{
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls\n", ciMain.nam);
			PrintSzW(sz);
		}
		day = DayOfWeek(ciMain.mon, ciMain.day, ciMain.yea);
		const char* daystr = get_weekday_name(day);
		char day3str[4] = {};
		sprintf(day3str, "%c%c%c", daystr[0], daystr[1], daystr[2]);
		if (1)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%s %ls %ls (%lcT %ls GMT)", char_to_wchar(daystr).c_str(), SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), SzTimW(ciMain.tim), ChDstW(ciMain.dst), SzZoneW(ciMain.zon));
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls %ls %ls (%lcT %ls GMT)", char_to_wchar(day3str).c_str(), SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), SzTimW(ciMain.tim), ChDstW(ciMain.dst), SzZoneW(ciMain.zon));
		PrintSzW(sz);
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%lc%ls%ls%ls\n", fLoc && !fNam ? L'\n' : L' ', ciMain.loc, fLoc ? L" " : L"", SzLocation2(ciMain.lon, ciMain.lat));
		PrintSzW(sz);
		ciMain = ciT;
	}
}

/* copy as test */
void ChartListing()
{
	ET et;
	wchar_t sz[cchSzDef];
	int obj_left, obj_right, i, k, l;
	double rT;
	byte ignoreT[NUMBER_OBJECTS];
	memcpy(&ignoreT, &ignore1, NUMBER_OBJECTS);//5
	//is.szFileScreen = NULL;
	
	//Undo ...
	CreateElemTable(&et);

	PrintHeader();
	wprintf(L"\n");
	
	if (us.fSeconds)
	{
		wchar_t sz[120];
		if (wi.chs)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Body         Location  Ret. %ls Rul. House Rul.  Velocity\n", us.fEquator ? L"赤纬" : L"黄纬");
		else
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"Body         Location  Ret. %ls Rul. House Rul.  Velocity\n", us.fEquator ? L"Declin. " : L"Latitude");

		PrintSzW(sz);
		wprintf(L"\n");
	}
	else
	{
		wchar_t a1[120];
		wcscpy(a1, char_to_wchar(szHouseSystem[us.nHouseSystem]).c_str());

		if (wi.chs)
		{
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"Body        Locat. Ret. %ls. Rul. House  Rul. Veloc.    %ls Houses.\n", us.fEquator ? L"赤纬" : L"黄纬", char_to_wchar(szHouseSystem[us.nHouseSystem]).c_str());
		}
		else
		{
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"Body        Locat. Ret. %ls. Rul. House  Rul. Veloc.    %ls Houses.\n", us.fEquator ? L"Decl" : L"Lati", char_to_wchar(szHouseSystem[us.nHouseSystem]).c_str());
		}
		PrintSzW(sz);
		wprintf(L"\n");
	}

	PrintL();
	/* Ok, now print out the location of each object. */
	for (obj_right = 1 - us.fSeconds, obj_left = 0; obj_right <= uranHi; obj_right++, obj_left++)
	{
		if (us.fSeconds)
		{
			if (FIgnore(obj_right))
			{
				continue;
			}
		}
		else
		{
			if (obj_right > cuspLo && ((obj_right < cuspHi) || FIgnore(obj_right)))
			{
				continue;
			}

			while (obj_right <= cuspHi && obj_left <= cuspHi && FIgnore(obj_left))
			{
				obj_left++;
			}
		}

		if (obj_right >= cuspLo && obj_right <= cuspHi - 2 && obj_left > cuspHi - 2)	/* Don't print blank */
		{
			continue;											/* lines.      VA.   */
		}

		if (obj_right <= cuspHi - 2 && obj_left > cuspHi - 2)
		{
			if (!wi.chs)
			{
				if (CountryID != 5)
					PrintTab(' ', 52);
				else
					PrintTab(' ', 51);
			}
			else
			{
				PrintTab(' ', 55);
			}
		}
		else
		{
			if (obj_right > cuspHi - 2)
			{
				obj_left = obj_right;
			}
			wchar_t sz[120];
			//AnsiColor(kObjA[obj_left]);

			if (wi.chs)
			{
				//wchar_t szA[120];
				//swprintf(szA, sizeof(szA) / sizeof(wchar_t), "%-10.10ls:", char_to_wchar(tObjName[obj_left]).c_str());
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%-10.10ls:", char_to_wchar(tObjName[obj_left]).c_str());
				PrintSzW(sz);
			}
			else
			{
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%-11.11ls:", char_to_wchar(tObjName[obj_left]).c_str());
				PrintSzW(sz);
			}
			if (IsPDsChartWithoutTable && UsePDsInChart)// pds in chart
				PrintSzW(SzZodiac(cpPDs.longitude[obj_left]));
			else
				PrintSzW(SzZodiac(cp0.longitude[obj_left]));

			if (IsPDsChartWithoutTable && UsePDsInChart)
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" %lc ", cpPDs.vel_longitude[obj_left] >= 0.0 ? L'*' : chRet);
			else
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" %lc ", cp0.vel_longitude[obj_left] >= 0.0 ? L'*' : chRet);
			PrintSzW(sz);


			//if (obj_left <= cThing || obj_left > cuspHi)  oFor
			if (obj_left <= oFor || obj_left > cuspHi)
			{
				if (IsPDsChartWithoutTable && UsePDsInChart)
					PrintAltitude(cpPDs.latitude[obj_left]);
				else
					PrintAltitude(cp0.latitude[obj_left]);
			}
			else
			{
				// 前下划线
				if (wi.chs)
					PrintTab('_', us.fSeconds ? 11 : 8);
				else
					PrintTab('_', us.fSeconds ? 10 : 7);
			}

			wchar_t a1;
			if (IsPDsChartWithoutTable && UsePDsInChart)
				a1 = Dignify(obj_left, Z2Sign(cpPDs.longitude[obj_left]),wi.chs);// 		a1	8212 L'—'	wchar_t
			else
				a1 = Dignify(obj_left, Z2Sign(cp0.longitude[obj_left]),wi.chs);

			if (a1 == L'—')
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" (%lc)", a1);// 星体及它所在的星座
			else
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" (%lc)", a1);

			if (wi.chs)
				PrintSzW(FCusp(obj_left) ? L"    " : sz);
			else
				PrintSzW(FCusp(obj_left) ? L"   " : sz);

			if (FCusp(obj_left) /*&& wi.chs && into*/)
				PrintSzW(L" ");

			if (IsPDsChartWithoutTable && UsePDsInChart)
			{
				k = cpPDs.house_no[obj_left];
				l = Z2Sign(cpPDs.cusp_pos[k]);
			}
			else
			{
				k = cp0.house_no[obj_left];
				l = Z2Sign(cp0.cusp_pos[k]);
			}

			//AnsiColor(kSignA(k));

			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" [%ls%2d] ",char_to_wchar(tSuffix[k]).c_str(), k);
			PrintSzW(sz);

			//AnsiColor(kDefault);

			wchar_t a2[2] = { 0,0 };
			a2[0] = Dignify(obj_left, k,wi.chs);
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"[%ls", a2);

			PrintSzW(FCusp(obj_left) ? L"  " : sz);

			a2[0] = Dignify(obj_left, l,wi.chs);
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%ls] ", a2);
			PrintSzW(FCusp(obj_left) ? L"   " : sz);

			if (obj_left > 19 && FCusp(obj_left))
			{
				if (wi.chs)
					PrintSzW(L"  ");
			}

			if (
				FObject(obj_left) ||
				obj_left == oNoNode || obj_left == oSoNode || obj_left == oLil || obj_left == oFor)
			{
				if (IsPDsChartWithoutTable && UsePDsInChart)
				{
					PrintCh((byte)(cpPDs.vel_longitude[obj_right] < 0.0 ? '-' : '+'));
					rT = Rad2Deg(fabs(cpPDs.vel_longitude[obj_left]));
				}
				else
				{
					PrintCh((byte)(cp0.vel_longitude[obj_right] < 0.0 ? '-' : '+'));
					rT = Rad2Deg(fabs(cp0.vel_longitude[obj_left]));
				}

				if (rT == 360)
					rT = 0;
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), us.fSeconds ? (rT < 10.0 ? L"%9.7f" : L"%9.6f") : (rT < 10.0 ? L"%5.3f" : L"%5.2f"), rT);
				PrintSzW(sz);
			}
			else
			{
				// 后下划线
				PrintTab('_', us.fSeconds ? 10 : 6);
			}
		}
		if (!us.fSeconds)
		{
			/* For some lines, we have to append the house cusp positions. */
			if (obj_right <= NUMBER_OF_SIGNS)
			{
				PrintSzW(L"  -  ");
				//AnsiColor(kSignA(obj_right));
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%ls %2d: ", L"House cusp", obj_right);
				PrintSzW(sz);
				if (IsPDsChartWithoutTable && UsePDsInChart)
					PrintSzW(SzZodiac(cpPDs.cusp_pos[obj_right]));
				else
					PrintSzW(SzZodiac(cp0.cusp_pos[obj_right]));
			}

			/* For some lines, we have to append the element table information. */

			if (obj_right == NUMBER_OF_SIGNS + 2)
			{
				PrintSzW(L"      Car Fix Mut TOT");
			}
			else if (obj_right > NUMBER_OF_SIGNS + 2 && obj_right < NUMBER_OF_SIGNS + 7)
			{
				k = obj_right - (NUMBER_OF_SIGNS + 2) - 1;
				//AnsiColor(kElemA[k]);
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  %c%c%c%3d %3d %3d %3d",
						tElement[k][0], tElement[k][1], tElement[k][2],
						et.coElemMode[k][0], et.coElemMode[k][1], et.coElemMode[k][2], et.coElem[k]);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  %ls%4d %4d %4d %4d",
						char_to_wchar(tElement[k]).c_str(), 
						et.coElemMode[k][0], et.coElemMode[k][1], et.coElemMode[k][2], et.coElem[k]);

				PrintSzW(sz);
				//AnsiColor(kDefault);
			}
			else if (obj_right == NUMBER_OF_SIGNS + 7)
			{
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  TOT %2d %3d %3d %3d", et.coMode[0], et.coMode[1], et.coMode[2], et.coSum);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  共 %3d %4d %4d %4d", et.coMode[0], et.coMode[1], et.coMode[2], et.coSum);
				PrintSzW(sz);
			}
			else if (obj_right == oCore)
			{
				PrintTab(' ', 23);
			}
			else if (obj_right >= uranLo)
			{
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" Uranian #%d", obj_right - uranLo + 1);
				PrintSzW(sz);
			}
			sz[0] = chNull;

			switch (obj_right - NUMBER_OF_SIGNS - 1)
			{
			case 1:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  +:%2d", et.coYang);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" 阳:%2d", et.coYang);
				break;
			case 2:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   -:%2d", et.coYin);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   阴:%2d", et.coYin);
				break;
			case 3:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   M:%2d", et.coMC);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   上:%2d", et.coMC);
				break;
			case 4:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   N:%2d", et.coIC);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   下:%2d", et.coIC);
				break;
			case 5:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   A:%2d", et.coAsc);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   左:%2d", et.coAsc);
				break;
			case 6:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   D:%2d", et.coDes);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   右:%2d", et.coDes);
				break;
			case 7:
			{
				PrintSzW(L"  ");
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"Learn: %d, Share: %d", et.coLearn, et.coShare);
			}
			break;
			}
			PrintSzW(sz);
		}
		else	// print seconds
		{
			PrintSzW(L" Decan: ");
			int isfSeconds = is.fSeconds;
			is.fSeconds = false;
			if (IsPDsChartWithoutTable && UsePDsInChart)
				PrintSzW(SzZodiac(Decan(cpPDs.longitude[obj_right])));
			else
				PrintSzW(SzZodiac(Decan(cp0.longitude[obj_right])));

			is.fSeconds = isfSeconds;
		}
		//PrintL();
		wprintf(L"\n");
	}

	/* Do another loop to print out the stars in their specified order. */
	if (us.nStar)
	{
		for (i = starLo; i <= starHi; i++)
		{
			for (int j = 0; j < starLo; j++)
			{
				if (!ignoreSO[i] && !ignoreSO[j] && abs(cp0.longitude[j] - cp0.longitude[i]) <= 0.5)
				{
					j = cLastMoving + starname[i - cLastMoving];

					if (!wi.chs)
					{
						swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%-11.11s:", char_to_wchar(tObjName[j]).c_str());
						PrintSzW(sz);
					}
					else
					{
						char szA[120];
						sprintf(szA, "%-10.10s:", tObjName[j]);
						PrintSz(szA);
					}
					PrintSzW(SzZodiac(cp0.longitude[j]));
					PrintSzW(L"   ");
					PrintAltitude(cp0.latitude[j]);
					k = cp0.house_no[j];
					//AnsiColor(kSignA(k));

					//AnsiColor(kDefault);
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   %ls %-8.8s  Star #%2d: %5.2f\n",
						us.fSeconds ? L"   " : L"", char_to_wchar(szStarConName[j - cLastMoving]).c_str(), i - cLastMoving, rStarBright[j - cLastMoving]);
					PrintSzW(sz);
					break;

				}
			}
		}
	}
	memcpy(&ignore1, &ignoreT, NUMBER_OBJECTS);
}

/* Convert rectangular to spherical coordinates. */
double RecToSph(double B, double L, double O) {
	double R, Q, G, X, Y, A;

	A = B;
	R = 1.0;
	PolToRec(A, R, &X, &Y);
	Q = Y;
	R = X;
	A = L;
	PolToRec(A, R, &X, &Y);
	G = X;
	X = Y;
	Y = Q;
	RecToPol(X, Y, &A, &R);
	A += O;
	PolToRec(A, R, &X, &Y);
	Q = asin(Y);
	Y = X;
	X = G;
	RecToPol(X, Y, &A, &R);
	if (A < 0.0)
		A += 2 * rPi;
	G = A;
	return G;					/* We only ever care about and return one of the coordinates. */
}
/* Given an object index and a Julian Day ephemeris time, get ecliptic longitude and	*/
/* latitude of the object and its velocity and distance from the Earth or Sun. 			*/
// converts an astrolog object number to Swiss Ephemeris object number
// returns Swiss Ephemeris object number, or -1 if conversion not possible

#define rDegHalf   180.0
#define rDegQuad   90.0

// 5.41G used SE of 02/12/199

/* Compare two strings. Return 0 if they are equal, a positive value if  */
/* the first string is greater, and a negative if the second is greater. */
int NCompareSz(const char* s1, const char* s2)
{
	while (*s1 && *s1 == *s2)
		s1++, s2++;
	return *s1 - *s2;
}

/* Copy a given number of bytes from one location to another. */
void CopyRgb(byte* pbSrc, byte* pbDst, int cb)
{
	while (cb-- > 0)
		*pbDst++ = *pbSrc++;
}

const char *szDir[4] = {"North", "East", "South", "West"};
wchar_t retA[255];
wchar_t *addspace(const wchar_t *str, int num)
{
	wcscpy(retA, str);
	int len = wcslen(str) * 2;
	for (int i = 0; i < (num - len); i++)
		wcscat(retA, L" ");
	return retA;
}

void PrintAspect(int obj1, int sign1, int ret1, int asp, int obj2, int sign2, int ret2, byte chart, int field_length1, int field_length2)
{
	wchar_t sz[cchSzDef];

	//AnsiColor(kObjA[obj1]);//ok

	if (chart == 't')
		PrintSzW(sTrans);
	else if (chart == 'e' || chart == 'u' || chart == 'U')
		PrintSzW(sProgr);

	if (wi.chs)
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" %c%c", chObj3C(obj1));
	else
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" %*.*ls", field_length1, field_length1, char_to_wchar(tObjName[obj1]).c_str());

	PrintSzW(sz);

	//AnsiColor(kSignA(sign1));

	if (wi.chs)
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" %lc%c%c%c%lc", ret1 > 0 ? L'(' : (ret1 < 0 ? L'[' : L'<'), chSig3C(sign1), ret1 > 0 ? L')' : (ret1 < 0 ? L']' : L'>'));
	else
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" %lc%c%c%c%lc", ret1 > 0 ? L'(' : (ret1 < 0 ? L'[' : L'<'), chSig3(sign1), ret1 > 0 ? L')' : (ret1 < 0 ? L']' : L'>'));


	PrintSzW(sz);
	//AnsiColor(asp > 0 ? kAspA[asp] : kWhite);

	//	AnsiColor(asp > 0 ? kAspA[asp] : kDefault);//OK
	PrintCh(' ');
	if (asp == aSig)
	{
		if (!wi.chs)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"--> ");		/* Print a sign change. */
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"进入  ");
	}
	else if (asp == aDir)
		if (!wi.chs)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"S/%c", obj2 ? chRet : L'D');	/* Print a direction change. */
		else
		{
			if (obj2)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", L"变逆行");	/* Print a direction change. */
			else
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", L"变顺行");
		}
	else if (asp == 0)
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), chart == 'm' ? L"&" : L"with");
	else
	{
		if (!wi.chs)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%-4ls", char_to_wchar(tAspectAbbrev[asp]).c_str());	/* Print an aspect. */
		else
		{
			std::wstring temp = char_to_wchar(tAspectAbbrev[asp]);
			wchar_t * a = addspace(const_cast<wchar_t*>(temp.c_str()), 4);
			wcscpy(sz, a);
			//swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%-2s", Lang(tAspectAbbrev, asp));	/* Print an aspect. */
		}
	}
	PrintSzW(sz);
	if (asp != aDir)
		PrintCh(' ');
	if (asp == aSig)
	{
		//AnsiColor(kSignA(obj2));//OK
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", char_to_wchar(tSignName[obj2]).c_str());
		PrintSzW(sz);
	}
	else if (asp >= 0)
	{
		char buftmp[20];
		//AnsiColor(kSignA(sign2));//OK
		if (chart == 't' || chart == 'u' || chart == 'U')
			PrintSzW(sNatal);
		memset(buftmp,0,sizeof(buftmp));
		if (wi.chs)
			sprintf(buftmp, "%c%c%c%c%c ", ret2 > 0 ? '(' : (ret2 < 0 ? '[' : '<'), chSig3C(sign2), ret2 > 0 ? ')' : (ret2 < 0 ? ']' : '>'));
		else
			sprintf(buftmp,"%c%c%c%c%c ", ret2 > 0 ? '(' : (ret2 < 0 ? '[' : '<'), chSig3(sign2), ret2 > 0 ? ')' : (ret2 < 0 ? ']' : '>'));
		;
		PrintSzW(char_to_wchar(buftmp).c_str());
		//AnsiColor(kObjA[obj2]);

		memset(buftmp, 0, sizeof(buftmp));
		if (wi.chs)
			sprintf(buftmp," %c%c", chObj3C(obj2));
		else
			sprintf(buftmp,"%s", tObjName[obj2]);
		PrintSzW(char_to_wchar(buftmp).c_str());
	}
	if (chart == 'D' || chart == 'T' || chart == 'U' || chart == 'a' || chart == 'A' || chart == 'm' || chart == 'M')
	{
		if (wi.chs)
			PrintTab(' ', 6);
		else
		{
			if (CountryID != 2)
				PrintTab(' ', field_length2 - strlen(tObjName[obj2]));
			else
				PrintTab(' ', field_length2 - strlen(tObjName[obj2]) + 1);
		}
	}
}
int NCheckEclipseLunar(int iEar, int iMoo, int iSun, double *prPct)
{
	double radiS, radiE, radiM, radiU, radiP, lenS, lenM,
		angDiff, angM, angU, angP, theta;

	// Objects must be different.
	if (iEar == iSun || iEar == iMoo || iMoo == iSun)
		return etUndefined;

	// Objects that aren't actual things in cp0.pt can't eclipse or be eclipsed.
	if (!FThing(iEar) || !FThing(iMoo) || !FThing(iSun))
		return etUndefined;

	// Calculate angular distance between the Moon and point opposite the Sun.
	angDiff = SphDistance(Mod(cp0.longitude[iSun] + rDegHalf), -cp0.latitude[iSun],
		cp0.longitude[iMoo], cp0.latitude[iMoo]);
	if (angDiff > (iEar == oEar ? 2.0 : 18.0))
		return etNone;

	// Calculate radius of the Sun, Earth, and Moon in km.
	radiS = rObjDiam[iSun] / 2.0;
	radiE = rObjDiam[iEar] / 2.0;
	radiM = rObjDiam[iMoo] / 2.0;
	lenS = PtLen(cp0.pt[iSun]) * rAUToKm;
	lenM = PtLen(cp0.pt[iMoo]) * rAUToKm;

	//radiU = (radiE - radiS) / lenS * (lenS + lenM) + radiS;
	//radiP = (radiS + radiE) / lenS * (lenS + lenM) - radiS;
	theta = RAsinD((radiS - radiE) / lenS);
	radiU = radiE - lenM * RTanD(theta);
	theta = RAsinD((radiE + radiS) / lenS);
	radiP = (lenS + lenM) * RTanD(theta) - radiS;

	// Calculate angular size in sky of Moon, and Earth's umbra and penumbra.
	angM = RAtnD(radiM / lenM);
	angU = RAtnD(radiU / lenM);
	angP = RAtnD(radiP / lenM);

	// Compare angular sizes to distance, to see how much overlap there is.
	if (angDiff - angM >= angP)
		return etNone;
	else if (angDiff + angM <= angU)
	{
		if (prPct != NULL)
			*prPct = 100.0 - angDiff / (angU - angM) * 100.0;
		return etTotal;
	} 
	else if (angDiff - angM < angU)
	{
		if (prPct != NULL)
			*prPct = 100.0 - (angDiff - (angU - angM)) / (angM * 2.0) * 100.0;
		return etPartial;
	}
	if (prPct != NULL)
	{
		*prPct = (angDiff - (angP - angM)) / (angM * 2.0) * 100.0;
		*prPct = 100.0 - Max(*prPct, 0.0);
	}
	return angDiff + angM <= angP ? etPenumbra2 : etPenumbra;
}

int NCheckEclipseSolar(int iEar, int iMoo, int iSun, double *prPct)
{
  PT3R pSun, pMoo, pEar, pNear, pUmb, vS2M, vS2E, vE2U, vS2Mu, vS2N;
  double radiS, radiE, radiM, radiU, radiP, lNear, lSM, lSU, lSE, lSN, lEU, dot;

  // Objects must be different.
  if (iEar == iSun || iEar == iMoo || iMoo == iSun)
    return etUndefined;

  // Calculate radius and coordinates of the objects in km.
  radiS = rObjDiam[iSun] / 2.0;
  radiE = rObjDiam[iEar] / 2.0;
  radiM = rObjDiam[iMoo] / 2.0;

  pSun = cp0.pt[iSun]; pMoo = cp0.pt[iMoo]; pEar = cp0.pt[iEar];
  PtMul(pSun, rAUToKm);
  PtMul(pMoo, rAUToKm);
  PtMul(pEar, rAUToKm);

  // Determine point along Sun/Moon ray nearest the center of Earth.
  PtVec(vS2M, pSun, pMoo);
  PtVec(vS2E, pSun, pEar);
  dot = PtDot(vS2E, vS2M) / PtDot(vS2M, vS2M);
  vS2N = vS2M; PtMul(vS2N, dot);
  pNear = pSun; PtAdd2(pNear, vS2N);
  PtSub2(pNear, pEar); lNear = PtLen(pNear);

  // Determine point of maximum extent of Moon's umbra shadow.
  lSM = PtLen(vS2M);
  lSU = lSM * radiS / (radiS - radiM);
  vS2Mu = vS2M; PtDiv(vS2Mu, lSM);
  pUmb = vS2Mu; PtMul(pUmb, lSU); PtAdd2(pUmb, pSun);
  PtVec(vE2U, pEar, pUmb);
  lSE = PtLen(vS2E);
  lEU = PtLen(vE2U);
  lSN = PtLen(vS2N);
  radiU = radiS * (lSU - lSN) / lSU;
  if (radiU < 0.0)
    radiU = 0.0;

  // If Sun/Moon ray intersects Earth, must be an annular or solar eclipse.
  if (lNear - radiU < radiE)
  {
    if (prPct != NULL)
      *prPct = 100.0;
    if (lSU < lSE && lEU > radiE)
      return etAnnular;
    return etTotal;
  }

  // Check if Earth intersects penumbra shadow, for a partial solar eclipse.
  radiP = (radiS + radiM) / lSM * lSN - radiS;
  if (lNear - radiE < radiP)
  {
    if (prPct != NULL)
      *prPct = 100.0 - (lNear - radiE) / radiP * 100.0;
    return etPartial;
  }
  return etNone;
}

double RObjDiam(int obj)
{
  if (!FNorm(obj))
    return 0.0;
  // If cp0.longitude Center of Body (COB) is present, barycenter size should be 0.
  if (FBetween(obj, oJup, oPlu) && (!ignore1[obj - oJup + cobLo]))
    return 0.0;
  return rObjDiam[obj];
}

int NCheckEclipse(int obj1, int obj2, double *prPct)
{
  double radi1, radi2, len1, len2, angDiff, ang1, ang2;

  // Objects that aren't actual things in cp0.pt can't eclipse or be eclipsed.
  if (!FThing(obj1) || !FThing(obj2))
    return etUndefined;

  // Calculate radius of the two objects in km.
  radi1 = RObjDiam(obj1) / 2.0;
  radi2 = RObjDiam(obj2) / 2.0;
  if (radi1 <= 0.0 && radi2 <= 0.0)
    return etNone;

  // Special check if solar eclipse allowed to happen anywhere on Earth.
  if (us.fEclipseAny && obj1 == oSun)
    return NCheckEclipseSolar(us.objCenter, obj2, oSun, prPct);

  // Calculate angular distance between center points of the two objects.
  angDiff = SphDistance(cp0.longitude[obj1], cp0.latitude[obj1],
    cp0.longitude[obj2], cp0.latitude[obj2]);
  if (us.objCenter == oEar && angDiff > 0.75)
    return etNone;

  // Calculate angular size in the sky spanned by the two objects.
  len1 = PtLen(cp0.pt[obj1]) * rAUToKm;
  len2 = PtLen(cp0.pt[obj2]) * rAUToKm;
  ang1 = RAtnD(radi1 / len1);
  ang2 = RAtnD(radi2 / len2);
  if (ang1 + ang2 <= angDiff)
    return etNone;

  // Compare angular sizes to distance, to see how much overlap there is.
  if (prPct != NULL)
    *prPct = ang1 == ang2 ? 100.0 :
      100.0 - angDiff / RAbs(ang2 - ang1) * 100.0;
  if (ang1 >= ang2 + angDiff)
    return len1 - radi1 >= len2 + radi2 ? etAnnular : etTotal;
  else if (ang2 >= ang1 + angDiff)
    return len2 - radi2 >= len1 + radi1 ? etAnnular : etTotal;
  if (prPct != NULL)
    *prPct = 100.0 -
      (angDiff - RAbs(ang2 - ang1)) / (Min(ang1, ang2) * 2.0) * 100.0;
  return etPartial;
}

void PrintInDayEvent(int source, int aspect, int dest, int nVoid)
{
	char sz[cchSzDef];
	int nEclipse, nEclipse2;
	double rPct;
	bool fSwap;

	// If the Sun changes sign, then print out if this is a season change.
	if (aspect == aSig)
	{
		if (source == oSun)
		{
			//AnsiColor(kWhiteA);
			if (dest == sAri || dest == sLib)
			{
				if ((dest == sAri) == (ciCore.lat >= 0.0))
					PrintSzW(sVernalEquinox);
				else
					PrintSzW(sAutumnalEquinox);
			}
			else if (dest == sCan || dest == sCap)
			{
				if ((dest == sCan) == (ciCore.lat >= 0.0))
					PrintSzW(sSummerSolstice);
				else
					PrintSzW(sWinterSolstice);
			}
		}
	}
	else if (aspect > 0 && !us.fParallel)
	{
		fSwap = (dest == oSun);
		if (fSwap)
			SwapN(source, dest);

		// Print if the present aspect is a New, Full, or Half Moon.
		if (source == oSun && (dest == oMoo || FMoons(dest)) &&
			(us.fMoonMove || ObjOrbit(dest) == us.objCenter))
		{
			//if (aspect <= aSqu)
				//AnsiColor(kWhiteA);
			if (aspect == aCon)
				PrintSzW(sNewMoon);
			else if (aspect == aOpp)
			{
				PrintSzW(sFullMoon);
				// Full Moons may be a lunar eclipse.
				if (us.fEclipse)
				{
					nEclipse = NCheckEclipseLunar(us.objCenter, dest, oSun, &rPct);
					if (nEclipse > etNone)
					{
						//AnsiColor(kWhiteA);
						sprintf(sz, " (%s Lunar Eclipse", szEclipse[nEclipse]);
						PrintSz(sz);
						if (us.fSeconds)
						{
							sprintf(sz, " %.0f%%", rPct); PrintSz(sz);
						}
						PrintSz(")");
					}
				}
			}
			else if (aspect == aSqu)
				PrintSzW(sHalfMoon);
		}
		else if (us.fEclipse && aspect == aOpp)
		{
			// Check for generic opposition that's an eclipse.
			nEclipse = NCheckEclipseLunar(us.objCenter, dest, source, &rPct);
			if (nEclipse > etNone)
			{
				nEclipse2 = NCheckEclipseLunar(us.objCenter, source, dest, &rPct);
				nEclipse = Max(nEclipse, nEclipse2);
				//AnsiColor(kWhiteA);
				sprintf(sz, " (%s Occultation", szEclipse[nEclipse]);
				PrintSz(sz);
				if (us.fSeconds)
				{
					sprintf(sz, " %.0f%%", rPct); PrintSz(sz);
				}
				PrintSz(")");
			}
		}

		// Conjunctions may be a solar eclipse or other occultation.
		if (us.fEclipse && aspect == aCon)
		{
			nEclipse = NCheckEclipse(source, dest, &rPct);
			if (nEclipse > etNone)
			{
				//AnsiColor(kWhiteA);
				wchar_t szW[120];
				swprintf(szW, sizeof(szW) / sizeof(wchar_t), L" (%ls %ls%ls", char_to_wchar(szEclipse[nEclipse]).c_str(), source == oSun ?
					sSolar : L"", source == oSun && (dest == oMoo || FMoons(dest)) ?
					L"Eclipse" : L"Occultation");
				PrintSzW(szW);
				if (us.fSeconds)
				{
					sprintf(sz, " %.0f%%", rPct); PrintSz(sz);
				}
				PrintSz(")");
			}
		}
		if (fSwap)
			SwapN(source, dest);
	}

	// Print if the present aspect is the Moon going void of course.
	if (nVoid >= 0)
	{
		//AnsiColor(kDefault);
		sprintf(sz, " (v/c %d:%02d", nVoid / 3600, nVoid / 60 % 60); PrintSz(sz);
		if (us.fSeconds)
		{
			sprintf(sz, ":%02d", nVoid % 60); PrintSz(sz);
		}
		PrintCh(')');
	}
	PrintL();

#ifdef INTERPRET
	//if (us.fInterpret)
		//InterpretInDay(source, aspect, dest);
#endif
	//AnsiColor(kDefault);
}

wchar_t lines1[cchSzMax];
int cursor = 0;
void FieldWord(wchar_t *sz) 
{
	int i, j;

	/* Hack: Dump buffer if function called with a null string. */

	if (sz == NULL)
	{
		lines1[cursor] = 0;
		PrintSzW(lines1);
		PrintL();
		cursor = 0;
		return;
	}
	if (cursor)
	{
		lines1[cursor++] = ' ';
	}
	for (i = 0; (lines1[cursor] = sz[i]); i++, cursor++)
		;

	/* When buffer overflows 'n' columns, display one line and start over. */

	while (cursor >= us.nScreenWidth - 1)
	{
		for (i = us.nScreenWidth - 1; lines1[i] != ' '; i--)
			;
		lines1[i] = 0;
		PrintSzW(lines1);
		PrintL();
		lines1[0] = lines1[1] = ' ';
		for (j = 2; (lines1[j] = lines1[i + j - 1]) != 0; j++)
			;
		cursor -= (i - 1);
	}
}

#ifdef INTERPRET
/*
******************************************************************************
** Interpretation Tables.
******************************************************************************
*/
wchar_t szMindPart[cLastMoving + 1][120];//17,26 
#endif /* INTERPRET */

/* Print an interpretation for a particular aspect in effect in a chart. */
/* This is called from the InterpretGrid and ChartAspect routines.       */
void InterpretAspect(int x, int y) 
{
	wchar_t sz[cchSzDef * 2];
	int n;

	n = grid->n[x][y];
	if (n < 1 || n > cAspectInt || FCusp(x) || FCusp(y) || x > cLastMoving || y > cLastMoving)
		return;
	//AnsiColor(kAspA[n]);
	PrintSzW(L"\n");

	const char * a1= tObjName[x];
	char b1[MAX_STRING_NAME_LEN];
	sprintf(b1,"%s", tAspectName[n]);
	const char * c1= tObjName[y];
	int len =strlen(b1);

	if(wi.chs)
	{
		for (int i=len;i>0;i--)
		{
			if(b1[i]==' ')
				b1[i]=0;
		}
	}

	if(wi.chs)
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%s %s %s", char_to_wchar(a1).c_str(), char_to_wchar(b1).c_str(), char_to_wchar(c1).c_str());
	else
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%ls %ls %ls", char_to_wchar(a1).c_str(), char_to_wchar(b1).c_str(), char_to_wchar(c1).c_str());
	PrintSzW(sz);
	PrintSzW(L":");
	PrintSzW(L"\n");

	//AnsiColor(kDefault);
	if(wi.chs)
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%ls的",char_to_wchar(szPerson).c_str());
	else
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%ls's ",char_to_wchar(szPerson).c_str());

	PrintSzW(sz);

	FieldWord(szMindPart[x]);

	int a=Min(abs(grid->v[x][y]) / 150, 2);
	int b=n - 1;
	swprintf(sz, sizeof(sz)/sizeof(wchar_t), char_to_wchar(tInteract[n]).c_str(), tModify[a*b]);
	FieldWord(sz);

	swprintf(sz, sizeof(sz)/sizeof(wchar_t),sTheir, szMindPart[y]);

	FieldWord(sz);
	if (tTherefore[n][0])
	{
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%ls.", char_to_wchar(tTherefore[n]).c_str());
		FieldWord(sz);
	}
	FieldWord(NULL);
}

#define space     cp0.pt

const char* tAspectConfig[] = {"Stellium", "Grand Trine", "T-Square", "Yod", "Grand Cross", "Cradle", "Mystic Rect", "Kite"};
const char* sWith2  = "with";
const char* sfrom = "from";
const char* sand3 = "and";
const char* sto2 = "to ";
const char* sTo = " to ";
const char* sNoMajor = "No major configurations in aspect grid.\n";
/* This is a subprocedure of DisplayGrands(). Here we print out one aspect */
/* configuration found by the parent procedure.                            */
void PrintGrand(byte ac, int i1, int i2, int i3, int i4)
{
	wchar_t sz[cchSzDef];
	int asp=0;

	switch (ac)
	{
	case acS:
		asp = aCon;
		break;
	case acGT:
		asp = aTri;
		break;
	case acTS:
		asp = aOpp;
		break;
	case acY:
		asp = aInc;
		break;
	case acGC:
		asp = aSqu;
		break;
	case acC:
		asp = aSex;
		break;
	case acMR:
		asp = aSex;
		break;
	case acK:
		asp = aTri;
		break;
	default:;
	}

	//AnsiColor(kAspA[asp]);

	if (!wi.chs)
	{
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%-11ls", char_to_wchar(tAspectConfig[ac]).c_str());
		PrintSzW(sz);
	}
	else
		PrintSzW(addspace(char_to_wchar(tAspectConfig[ac]).c_str(), 11));


	//AnsiColor(kDefault);

	swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" %ls ", ac == acS || ac == acGT || ac == acGC || ac == acMR || ac == acK ? char_to_wchar(sWith2).c_str() : char_to_wchar(sfrom).c_str());//  swith sfrom  

	PrintSzW(sz);
	wchar_t szTmp[60];
	memset(szTmp,0,sizeof(szTmp));
	swprintf(szTmp,sizeof(szTmp)/sizeof(wchar_t),L"%ls", char_to_wchar(tObjShortName[i1]).c_str());
	//AnsiColor(kObjA[i1]);
	if (wi.chs)
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc%lc: ", szTmp[0], szTmp[1]);
	else
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc%lc%lc: ", szTmp[0], szTmp[1],szTmp[2]);

	PrintSzW(sz);
	PrintSzW(SzZodiac(cp0.longitude[i1]));

	swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" %ls ", ac == acS || ac == acGT || ac == acK ? char_to_wchar(sand3).c_str() : char_to_wchar(sto2).c_str());

	PrintSzW(sz);
	//AnsiColor(kObjA[i2]);
	memset(szTmp, 0, sizeof(szTmp));
	swprintf(szTmp, sizeof(szTmp) / sizeof(wchar_t), L"%ls", char_to_wchar(tObjShortName[i2]).c_str());
	if (wi.chs)
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc%lc: ", szTmp[0], szTmp[1]);
	else
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc%lc%lc: ", szTmp[0], szTmp[1], szTmp[2]);

	PrintSzW(sz);
	PrintSzW(SzZodiac(cp0.longitude[i2]));

	swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" %ls ", ac == acGC || ac == acC || ac == acMR ? char_to_wchar(sto2).c_str() : char_to_wchar(sand3).c_str());

	PrintSzW(sz);
	//AnsiColor(kObjA[i3]);
	memset(szTmp, 0, sizeof(szTmp));
	swprintf(szTmp, sizeof(szTmp) / sizeof(wchar_t), L"%ls", char_to_wchar(tObjShortName[i3]).c_str());
	if (wi.chs)
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc%lc: ", szTmp[0], szTmp[1]);
	else
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc%lc%lc: ", szTmp[0], szTmp[1], szTmp[2]);

	PrintSzW(sz);
	PrintSzW(SzZodiac(cp0.longitude[i3]));

	if (ac == acGC || ac == acC || ac == acMR || ac == acK)
	{
		//AnsiColor(kObjA[i4]);
		memset(szTmp, 0, sizeof(szTmp));
		swprintf(szTmp, sizeof(szTmp) / sizeof(wchar_t), L"%ls", char_to_wchar(tObjShortName[i4]).c_str());
		if (wi.chs)
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc%lc: ", szTmp[0], szTmp[1]);
		else
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc%lc%lc: ", szTmp[0], szTmp[1], szTmp[2]);

		PrintSzW(sz);
		PrintSzW(SzZodiac(cp0.longitude[i4]));
	}
	//PrintL();
	wprintf(L"\n");
}

/* Scan the aspect grid of a chart and print out any major configurations, */
/* as specified with the -g0 switch.                                       */
void DisplayGrands()
{
	int cac = 0, i, j, k, l;

	for (i = 0; i <= cObj; i++)
	{
		if (!FIgnore(i))
		{
			for (j = 0; j <= cObj; j++)
			{
				if (j != i && !FIgnore(j))
				{
					for (k = 0; k <= cObj; k++)
					{
						if (k != i && k != j && !FIgnore(k))
						{
							/* Is there a Stellium among the current three planets? */

							if (i < j && j < k && grid->n[i][j] == aCon && grid->n[i][k] == aCon && grid->n[j][k] == aCon)
							{
								cac++;
								PrintGrand(acS, i, j, k, l);
								/* Is there a Grand Trine? */

							}
							else if (i < j && j < k && grid->n[i][j] == aTri && grid->n[i][k] == aTri && grid->n[j][k] == aTri)
							{
								cac++;
								PrintGrand(acGT, i, j, k, l);
								/* Grand Trine is there, may be there is a Kite too ? */
								/* We look, if some cp0.longitude is in sextiles with some   */
								/* pair of planets of Grand Trine, then it's surely   */
								/* opposed with third one.                            */

								for (l = 0; l <= cObj; l++)
								{
									if (!FIgnore(l))
									{
										if (grid->n[Min(i, l)][Max(i, l)] ==
											aSex && grid->n[Min(j, l)][Max(j, l)] == aSex)
										{
											cac++;
											PrintGrand(acK, i, j, k, l);
										}
										if (grid->n[Min(j, l)][Max(j, l)] ==
											aSex && grid->n[Min(k, l)][Max(k, l)] == aSex)
										{
											cac++;
											PrintGrand(acK, i, j, k, l);
										}
										if (grid->n[Min(i, l)][Max(i, l)] ==
											aSex && grid->n[Min(k, l)][Max(k, l)] == aSex)
										{
											cac++;
											PrintGrand(acK, i, j, k, l);
										}
									}
								}
								/* Is there a T-Square? */

							}
							else if (j < k && grid->n[j][k] == aOpp && grid->n[Min(i, j)][Max(i, j)] == aSqu && grid->n[Min(i, k)][Max(i, k)] == aSqu)
							{
								cac++;
								PrintGrand(acTS, i, j, k, l);
								/* Is there a Yod? */
							}
							else if (j < k && grid->n[j][k] == aSex && grid->n[Min(i, j)][Max(i, j)] == aInc && grid->n[Min(i, k)][Max(i, k)] == aInc)
							{
								cac++;
								PrintGrand(acY, i, j, k, l);
							}
							for (l = 0; l <= cObj; l++)
							{
								if (!FIgnore(l))
								{

									/* Is there a Grand Cross among the current four planets? */

									if (i < j && i < k && i < l && j < l
										&& grid->n[i][j] == aSqu
										&& grid->n[Min(j, k)][Max(j, k)] ==
										aSqu
										&& grid->n[Min(k, l)][Max(k, l)] ==
										aSqu && grid->n[i][l] == aSqu
										&& MinDistance(cp0.longitude[i],
											cp0.longitude[k]) > 150.0 && MinDistance(cp0.longitude[j], cp0.longitude[l]) > 150.0)
									{
										cac++;
										PrintGrand(acGC, i, j, k, l);

										/* Is there a Cradle? */

									}
									else if (i < l
										&& grid->n[Min(i, j)][Max(i, j)] == aSex
										&& grid->n[Min(j, k)][Max(j, k)] == aSex
										&& grid->n[Min(k, l)][Max(k, l)] == aSex
										&& MinDistance(cp0.longitude[i], cp0.longitude[l]) > 150.0)
									{
										cac++;
										PrintGrand(acC, i, j, k, l);

										/* Is there a Mystic Rectangle? */

									}
									else if (i < j && i < k && i < l &&
										grid->n[Min(i, j)][Max(i, j)] == aOpp
										&& grid->n[Min(k, l)][Max(k, l)] == aOpp
										&& grid->n[Min(i, k)][Max(i, k)] == aTri && grid->n[Min(j, l)][Max(j, l)] == aTri)
									{
										cac++;
										PrintGrand(acMR, i, j, k, l);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (!cac)
		PrintSzW(char_to_wchar(sNoMajor).c_str());
}


/* This is a subprocedure of ChartAspect() and ChartAspectRelation().       */
/* Display summary information about the aspect list, i.e. the total number */
/* of aspects of each type, and the number of aspects to each object, as    */
/* done when the -a0 aspect summary setting is set.                         */
void PrintAspectSummary(int* ca, int* co, int count, double rPowSum)
{
	wchar_t sz[cchSzDef];

	int i, j, k;

	if (count == 0)
	{
		PrintSzW(char_to_wchar(sNoAspects).c_str());
		return;
	}
	PrintL();
	swprintf(sz, sizeof(sz) / sizeof(wchar_t), char_to_wchar(sTotalPower).c_str(), rPowSum, rPowSum / (double)count);
	PrintSzW(sz);
	wprintf(L"\n");
	k = us.fParallel ? aOpp : cAspect;

	for (j = 0, i = 1; i <= k; i++)
	{
		if (!ignoreA[i])
		{
			if (!(j & 7))
			{
				if (j)
					PrintL();
			}
			else
				PrintSzW(L"   ");
			//AnsiColor(kAspA[i]);
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%ls:%3d", char_to_wchar(tAspectAbbrev[i]).c_str(), ca[i]);
			PrintSzW(sz);
			j++;
		}
	}
	wprintf(L"\n");
	//PrintL();
	for (j = 0, i = 0; i <= cObj; i++)
	{
		if (!FIgnore(i))
		{
			if (!(j & 7))
			{
				if (j)
					PrintL();
			}
			else
				PrintSzW(L"   ");
			//AnsiColor(kObjA[i]);
			if (wi.chs)
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%c%c:%3d", chObj3C(i), co[i]);
			else
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%c%c%c:%3d", chObj3(i), co[i]);

			PrintSzW(sz);
			j++;
		}
	}
	//PrintL();
	wprintf(L"\n");
}

/* Display all aspects between objects in the chart, one per line, in       */
/* sorted order based on the total "power" of the aspect, as specified with */
/* the -a switch. The same influences used for -I charts are used here.     */
void ChartAspect()
{
	int ca[cAspect + 1], co[NUMBER_OBJECTS];
	wchar_t sz[cchSzDef];
	int ihi, jhi, ahi, i, j, k, count = 0;
	double ip, jp, savepower, rPowSum = 0.0, p;
	int orb, saveorb;
	GridInfo tempgrid;
	wchar_t chdeg = 176;

	memset(ca, 0, sizeof(ca));
	memset(co, 0, sizeof(co));
	memcpy(&tempgrid, grid, sizeof(GridInfo));

	PrintHeader();				/* Display chart info */
	PrintL();

	if (wi.chs)
		PrintTab(' ', 49);
	else
		PrintTab(' ', 50);

	if (InterpretAlt != 1)
	{
		if (CountryID == 2)
			PrintSzW(L" ");

		PrintSzW(L"  Orb           Power\n\n");
	}
	else
		PrintSzW(L" ");
	wprintf(L"\n");
	PlanetPPower(wi.chs);
	ComputeInfluence();

	for (;;)
	{
		// we initialize now the two variables used for sorting, but only one 
		// of them will be used for sorting, depending on the sort type
		savepower = -1e10;
		saveorb = 0x7FFF;

		// search for the next most powerful aspect or next smallest orb

		for (i = 0; i <= cObj; i++)
		{
			if (!FIgnore(i))
			{
				for (j = 0; j < i; j++)
				{
					if (!FIgnore(j))
					{
						if ((k = tempgrid.n[j][i]) != 0)
						{
							ip = i <= cLastMoving ? ppower1[i] : 2.5;
							jp = j <= cLastMoving ? ppower1[j] : 2.5;
							orb = tempgrid.v[j][i];
							p = rAspInf[k] * sqrt(ip * jp) * (1.0 - fabs((double)orb) / 60.0 / GetOrb(i, j, k));
						
							if (us.fParallel)
								p *= PowerPar;
						
							// here we sort by decreasing power or increasing orb
							if ((fSortAspectsByOrbs && abs(orb) < abs(saveorb)) || (!fSortAspectsByOrbs && p > savepower))
							{
								saveorb = orb;
								ihi = i;
								jhi = j;
								savepower = p;
								ahi = k;
							}
						}
					}
				}
			}
		}

		if (saveorb == 0x7FFF)		// exit when no other aspect found
			break;

		tempgrid.n[jhi][ihi] = 0;	// mark this aspect as having being processed
		count++;					// Display the current aspect

#ifdef INTERPRET
		if (us.fInterpret)
		{						/* Interpret it if -I in effect. */
			InterpretAspect(jhi, ihi);
			continue;
		}
#endif
		rPowSum += savepower;
		ca[ahi]++;
		co[jhi]++;
		co[ihi]++;

		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%3d: ", count);
		PrintSzW(sz);

		PrintAspect(jhi, Z2Sign(cp0.longitude[jhi]), (int)RSgn(cp0.vel_longitude[jhi]), ahi, ihi, Z2Sign(cp0.longitude[ihi]), (int)RSgn(cp0.vel_longitude[ihi]), 'a', 12, 12);

		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" | %2d%lc%02d' %lc", abs(saveorb) / 60, chdeg, abs(saveorb) % 60, us.fAppSep ? (saveorb < 0 ? L'a' : L's') : (saveorb < 0 ? L'-' : L'+'));
		PrintSzW(sz);
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" | %9.2f |", savepower);
		PrintSzW(sz);

		// add from astrolog760
		PrintInDayEvent(j, ahi, i, -1);
		wprintf(L"\n");
	}


	if (us.fAspSummary)
	{
		PrintAspectSummary(ca, co, count, rPowSum);
		PrintL2();
		DisplayGrands();
	}
}

//推运的相位数据展示  =========重要=====
double power2[118];
double ppower2[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

  /* The inherent strength of each cp0.longitude when transiting - */
double rTransitInf[cLastMoving + 3] = { 10,
	10, 4, 8, 9, 20, 30, 35, 40, 45, 50,
	30, 15, 15, 15, 15, 30, 30, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	50, 50, 50, 50, 50, 50, 50, 50, 50
};
void ChartAspectRelation()
{
	int ca[cAspect + 1], co[NUMBER_OBJECTS];
	wchar_t sz[cchSzDef];
	int ihi, jhi, ahi, i = 0, j = 0, k = 0, m = 0, count = 0;
	double ip, jp, savepower, rPowSum = 0.0, p;
	int orb, saveorb;
	GridInfo tempgrid;
	byte ignoreT[NUMBER_OBJECTS] = {};
	memcpy(&tempgrid, grid, sizeof(GridInfo));//5
	//PrintL();//5
	switch(us.nRel)
	{
	case rcTransit:
	case rcProgTran: 
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore2(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = rTransitInf[i];
		break;
	
	case rcProgress:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore3(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = ppower1[i];
		break;
	
	case rcDual:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = ppower2[i];
		break;
	
	default:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = rObjInf[i];
	
		break;
	}
	for (;;)
	{
		savepower = -1e10;//5
		saveorb = 0x7FFF;//5
		for (i = 0; i <= cObj; i++)//5
		{
			if (!ignoreT[i])//5
			{
				for (j = 0; j < cObj; j++)//5
				{
					if (!FIgnore(j))//5
					{
						if (k = tempgrid.n[i][j])//5
						{
							ip = i <= cLastMoving ? power2[i] : 2.5;//5
							jp = j <= cLastMoving ? ppower1[j] : 2.5;//5
							orb = tempgrid.v[i][j];//5
							p = rAspInf[k] * sqrt(ip * jp) *
								(1.0 - fabs((double)orb) / 60.0 / GetOrb(i, j, k));//5

							if (us.fParallel)//5
								p *= PowerPar;
							if (fSortAspectsByOrbs && abs(orb) < abs(saveorb) ||
								!fSortAspectsByOrbs && p > savepower)//5
							{
								saveorb = orb;
								ihi = i;//5
								jhi = j;//5
								savepower = p;//5
								ahi = k;//5
							}
						}
					}
				}
			}
		}
		if (saveorb == 0x7FFF)		// exit when no other aspect found
			break;
		tempgrid.n[ihi][jhi] = 0;	// mark this aspect as having being processed
		rPowSum += savepower;
		ca[ahi]++;
		co[jhi]++;
		co[ihi]++;
		PrintAspect(jhi, Z2Sign(cp1.longitude[jhi]), (int)RSgn(cp1.vel_longitude[jhi]), ahi,
			ihi, Z2Sign(cp2.longitude[ihi]), (int)RSgn(cp2.vel_longitude[ihi]), 'A', 12, 12);
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%.6f", savepower);
		PrintSzW(sz);
		wprintf(L"\n");
	}
	if (us.fAspSummary)
	{
		PrintAspectSummary(ca, co, count, rPowSum);
	}
}

void decrDay(int *year, int *month, int *day)
{
	double t1;

	bool caltype = (
		*year < 1582  || 
		(*year == 1582 && (*month < 10 || *month == 10 && *day < 15)
		)) ? 0 : 1;

	double d1 = swe_julday(*year, *month, *day, 0, caltype)-1;
	swe_revjul(d1, caltype, year, month, day, &t1);
	return ;
}

void incrDay(int *year, int *month, int *day)
{
	double t1;

	bool caltype = (
		*year < 1582  || 
		(*year == 1582 && (*month < 10 || *month == 10 && *day < 15)
		)) ? 0 : 1;

	double d1 = swe_julday(*year, *month, *day, 0, caltype)+1;
	swe_revjul(d1, caltype, year, month, day, &t1);
	return ;
}

class Place
{
public:
	wchar_t place[260];
	int deglon;
	int minlon;
	int seclon;
	bool east;
	
	int deglat;
	int minlat;
	int seclat;
	bool north;
	
	int altitude;
	
	double lon;
	double lat;

	Place()
	{
		wchar_t *place;
		double	jut;
		int deglon;
		int minlon;
		int seclon;
		
		double	jut2;
		int deglat;
		int minlat;
		int seclat;
		int alt=ciCore.alt;

		place=ciCore.loc;
		jut = DegMin2DecDeg(-ciCore.lon) + 0.5 / 3600;
		deglon = (int) jut;
		minlon = (int) fmod(jut * 60, 60);
		seclon = (int) fmod(jut * 3600, 60);
		
		jut2 = DegMin2DecDeg(ciCore.lat) + 0.5 / 3600;
		deglat = (int) jut2;
		minlat = (int) fmod(jut2 * 60, 60);
		seclat = (int) fmod(jut2 * 3600, 60);

		bool isEast = ciCore.lon<=0;
		bool isNort = ciCore.lat>=0;
		PlaceInit(place, deglon, minlon, seclon, isEast, deglat, minlat, seclat, isNort, alt);
	};

	void PlaceInit(wchar_t *place, int deglon, int minlon, int seclon, bool east, int deglat, int minlat, int seclat, bool north, int altitude)
	{
		wcscpy(this->place,place);
		this->deglon = deglon;
		this->minlon = minlon;
		this->seclon = seclon;
		this->east = east;	
		
		this->deglat = deglat;
		this->minlat = minlat;
		this->seclat = seclat;
		this->north = north;
		
		this->altitude = altitude;
		
		this->lon = deglon+minlon/60.0+seclon/3600.0;
		this->lat = deglat+minlat/60.0+seclat/3600.0;

		//if(east)
		//	this->lon=-this->lon;
		
		return ;
	}
};

class PlanetaryHours
{
public:
	int PHs[7][24];
	int PHsC[7][24];
	double risetime;
	double settime;
	double hrlen;
	double daytime;
	int weekday;
	bool IsLastDay;
	PlanetaryHours(){};

	void PlanetaryHoursInit(double lon, double lat, double altitude, int weekday, double jd)
	{
		this->risetime = -1;
		this->settime = -1;
		this->hrlen = -1;
		this->daytime = -1;
		this->weekday = weekday;
		this->IsLastDay=FALSE;

		long whicheph;
		if(eepp==0)
			whicheph = SEFLG_JPLEPH; 
		else if(eepp==1)
			whicheph = SEFLG_SWIEPH;
		else if(eepp==2)
			whicheph = SEFLG_MOSEPH;

		double geopos[3];
		geopos[0]=lon;
		geopos[1]=lat;
		geopos[2]=altitude;
		char serr[260]={""};
		
		int gregflag;
		int year = ciNatal.yea;
		int month = ciNatal.mon;
		int day = ciNatal.day;
		double geo_longitude = -DegMin2DecDeg(ciNatal.lon);

		if ((long) ciNatal.yea * 10000L + (long) ciNatal.mon * 100L + (long) ciNatal.day < 15821015L) 
			gregflag = FALSE;
		else
			gregflag = TRUE;

		double tjd = swe_julday(year,month,day,0,gregflag);
		double dt = geo_longitude / 360.0;
		tjd =  tjd - dt;
		
		double datm[2]={1013.25,15.0};

		// sunrise and sunset for today
		char strNull[] = "";
		swe_rise_trans(tjd, SE_SUN, strNull, whicheph, SE_CALC_RISE | SE_BIT_HINDU_RISING,                        geopos, datm[0], datm[1],&risetime, serr);
		swe_rise_trans(tjd, SE_SUN, strNull, whicheph, SE_CALC_SET  | SE_BIT_DISC_CENTER | SE_BIT_NO_REFRACTION,  geopos, datm[0], datm[1],&settime,  serr);

		risetime = risetime - ciNatal.zon/24.0;
		settime  = settime  - ciNatal.zon/24.0;
	}
};

#define HOURSPERDAY  24.0
#define LOCALAPPARENT  3
char serr[260];
class Time 
{
public:
	//#calendars

	//#times
	int ZONE;

	double time1;
	int year;
	int origyear;
	int month;
	int origmonth;
	int day;
	int origday;
	int hour;
	int minute;
	int second;
	int cal;
	int zt;
	bool plus;
	double zh;
	double daylightsaving;
	Place place;
	bool full;
	int dyear, dmonth, dday, dhour, dmin, dsec;
	double jd;
	double sidTime;
	int weekday;
	PlanetaryHours ph;

	void time(int year, int month, int day, int hour, int minute, int second, int zt, bool plus, double zh, double daylightsaving, Place place, bool full = TRUE)
	{
		//#times
		ZONE = 0;
		this->year = year;
		this->month = month;
		this->day = day;
		this->origyear = year;
		this->origmonth = month;
		this->origday = day;
		this->hour = hour;
		this->minute = minute;
		this->second = second;
		
		this->zt = zt;
		this->plus = plus;
		this->zh = zh;
		this->daylightsaving = daylightsaving;
		this->time1 = hour+minute/60.0+second/3600.0;
		this->dyear=this->year;
		this->dmonth=this->month;
		this->dday=this->day;
		this->dhour=this->hour;
		this->dmin=this->minute;
		this->dsec=this->second;

		bool caltype = (
			this->year < 1582  || 
			(this->year == 1582 && (this->month < 10 || this->month == 10 && this->day < 15)
			)) ? 0 : 1;

		this->cal=caltype;

		if (this->daylightsaving)
		{
			this->time1 -= 1.0;
			this->dhour -= 1;
		}

		if (this->time1 < 0.0)
		{
			this->time1 += 24.0;
			decrDay(&this->year, &this->month, &this->day);
			this->dhour += int(24.0);
			this->dyear = this->year;
			this->dmonth = this->month;
			this->dday = this->day;
		}

		if (zt == 0)//:#ZONE
		{
			double ztime = zh;
			if (this->plus)
				this->time1-=ztime;
			else
				this->time1+=ztime;
		}
		else if (zt == 2)//:#LMT
		{
			double t = (place.deglon+place.minlon/60.0)*4.0;//#long * 4min
			if (place.east)
				this->time1-=t/60.0;
			else
				this->time1+=t/60.0;	
		}

		//#check over/underflow
		if (this->time1 >= HOURSPERDAY)
		{
			this->time1 -= HOURSPERDAY;
			incrDay(&this->year, &this->month, &this->day);
		}
		else if (this->time1 < 0.0)
		{
			this->time1 += HOURSPERDAY;
			decrDay(&this->year, &this->month, &this->day);
		}

		this->jd = swe_julday(this->year, this->month, this->day, this->time1,         caltype);

		if (zt == LOCALAPPARENT)//#LAT
		{
			double te;
			swe_time_equ(this->jd,&te,serr);
			this->jd += te;//#LMT
			//#Back to h,m,s(this->time) from julianday fromat

			swe_revjul(this->jd, this->jd >= 2299171.0, &this->year, &this->month, &this->day, &this->time1);

			//#To GMT
			double t = (place.deglon+place.minlon/60.0)*4.0 ;//#long * 4min
			if (place.east)
				this->time1-=t/60.0;
			else
				this->time1+=t/60.0;	

			//#check over/underflow
			if (this->time1 >= HOURSPERDAY)
			{
				this->time1 -= HOURSPERDAY;
				incrDay(&this->year, &this->month, &this->day);
			}
			else if (this->time1 < 0.0)
			{
				this->time1 += HOURSPERDAY;
				decrDay(&this->year, &this->month, &this->day);
			}
			//#GMT in JD (julianday)
			this->jd = swe_julday(this->year, this->month, this->day, this->time1, caltype);
		}

		this->sidTime = swe_sidtime(this->jd);// #GMT
		//this->ph = NULL;
		this->place = place;
		if (full && !us.fPrimDirs)
			calcPHs(place);
	}

	void calcPHs(Place place)
	{
		//#Planetary day/hour calculation
		this->weekday = DayOfWeek((int)this->month, (int)this->day, (int)this->year);
		if(this->weekday==0)
			this->weekday=7;
		this->weekday--;
		double lon = place.deglon+place.minlon/60.0;

		double lat = place.deglat+place.minlat/60.0;

		if (!place.north)
			lat *= -1;
			
		this->ph.PlanetaryHoursInit(lon, lat, place.altitude, this->weekday, this->jd); 
	}

};

Time timeP;

int fCP3 = 0; // this variable appears to be related to the usage of cp3, but how?
bool IsAgeHarmAndNatal = FALSE;
bool IsTransitAndNatal = FALSE;
bool IsTransitAndPDs = FALSE;
bool isSolarReturn = FALSE;
bool isLunarReturn = FALSE;
bool IsNewComparison2 = FALSE;
bool UseUserValue = FALSE;
bool IsAgeHarm=FALSE;
CI ciSolarReturn = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"",""};
CI ciLunarReturn = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"",""};

double normalize(double deg)
{
	while (deg < 0.0)
		deg += 360.0;
	while (deg >= 360.0)
		deg -= 360.0;
	return deg;
}

void calc(double lon, double *ant, double *cant)
{
		if (lon == 90.0 || lon == 270.0)
			*ant = lon;
		else if (lon > 90.0 && lon < 270.0)
			*ant = normalize(270.0+(270.0-lon));
		else if (lon < 90.0)
			*ant = normalize(90.0+(90.0-lon));
		else if (lon > 270.0)
			*ant = normalize(270.0-(lon-270.0));

		*cant = normalize(*ant+180.0);

		return ;
}
/* Calculate any of the various kinds of relationship charts. This involves */
/* computing and storing the cp0.longitude and house positions for the "core" and  */
/* "second" charts, and then combining them in the main single chart in the */
/* proper manner, e.g. for synastry, composite, time space midpoint charts. */
double calcDodecatemoria(double lon)
{
	return fmod(30 * (int)(lon / 30) + 12 * fmod(lon, 30) ,360);
}
//删除后星体集合到一个点上了,相位出现错误 2   重复
void CastRelation()
{
	byte ignoreT[NUMBER_OBJECTS];
	int i,saveRev;
	double ratio, t1, t2;
	double ppowerT[cLastMoving + 1];
	CI ciT;
	CP cpt;

	if (ciCore.mon == -1)
	{
		cp3 = cp0;
		fCP3 = 1;
	}

	/* Cast the first chart. */

	double usnHarmonic=us.nHarmonic;
	ciMain = ciCore;

	if(IsAgeHarmAndNatal)
		us.nHarmonic=1.0;

	t1 = CastChart(TRUE);
	cp1 = cp0;                  // cp1 :natal chart        "Transit and Progress"

	if(us.nRel == rcProgTran)
	{
		for (i = 0; i <= cObj; i++)
		{
			ignoreT[i] = ignore1[i];
			if (us.fSector || us.fAstroGraph)
				ignore1[i] = ignore1[i] && ignore2[i];
			else
				ignore1[i] = ignore2[i];                 // Transit cp0.longitude to natal cp0.longitude
		}

		ciCore=ciTwin;                                // cast "Transit" chart ?
		t1 = CastChart(TRUE);
		cpt = cp0;                                    // cpt --> transit chart
		memcpy(ignore1,ignoreT,sizeof(byte)*NUMBER_OBJECTS);        // restore natal cp0.longitude restrict
	}

	saveRev = hRevers;

	//if( IsAgeHarmAndNatal )
	//{
	//	us.nHarmonic=usnHarmonic;
	//	if(wi.fAnimate || !UseUserValue)
	//	{
	//		bool gregflag;
	//		if ((long) ciCore.yea * 10000L + (long) ciCore.mon * 100L + (long) ciCore.day < 15821015L) 
	//			gregflag = FALSE;
	//		else
	//			gregflag = TRUE;
	//	
	//		double tjd_ut;
	//		if(options.HarmonicChartsYearsMode)
	//			tjd_ut = swe_julday(ciCore.yea-1,ciCore.mon,ciCore.day,DegMin2DecDeg(ciCore.tim),gregflag) - (120-DegMin2DecDeg(ciCore.lon)) * 4.0 / 1440.0;
	//		else
	//			tjd_ut = swe_julday(ciCore.yea,ciCore.mon,ciCore.day,DegMin2DecDeg(ciCore.tim),gregflag) - (120-DegMin2DecDeg(ciCore.lon)) * 4.0 / 1440.0;
	//		double tjd_ut2 = swe_julday(ciTwin.yea,ciTwin.mon,ciTwin.day,DegMin2DecDeg(ciTwin.tim),gregflag) - (120-DegMin2DecDeg(ciTwin.lon)) * 4.0 / 1440.0;
	//		double diff = (tjd_ut2-tjd_ut)*86400/31556883.91;
	//		us.nHarmonic = diff;
	//	}
	//
	//	us.objOnAsc = 22;
	//	us.nHouseSystem = cHouseEqual;
	//	IsAgeHarm=TRUE;
	//	
	//	if (us.nHarmonic==0)
	//		us.nHarmonic=1;
	//	
	//	for (i = 0; i <= cObj; i++)
	//	{
	//		ignoreT[i] = ignore1[i];
	//		if (us.fSector || us.fAstroGraph)
	//			ignore1[i] = ignore1[i] && ignore3[i];
	//		else
	//			ignore1[i] = ignore3[i];                 // --> Transit cp0.longitude to natal cp0.longitude
	//	}
	//
	//	t2 = CastChart(TRUE);
	//	cp2 = cp0;
	//	IsAgeHarm=FALSE;
	//	memcpy(ignore1,ignoreT,sizeof(ignore1));
	//	for(int i=22;i<34;i++)
	//		if(i!=22 && i!=25 && i!=28 && i!=31)
	//			cp0.longitude[i]=cp0.cusp_pos[i-21];
	//}
	// why not do the grid and power when animating?? perhaps because the computers
	// were slow? It will be disabled for now

	if (!FCreateGrid(FALSE))
		return;
	PlanetPPower(wi.chs);

	/* Cast the second chart. */

	if(!isSolarReturn && !isLunarReturn && !us.f12parts2 && !us.fAnti && !us.fAnti2 && !us.fNavamsas2)
	{
		ciCore = ciTwin;
	}
	else if(isSolarReturn)
		ciCore = ciSolarReturn;
	else if(isLunarReturn)
		ciCore = ciLunarReturn;

	if (us.nRel == rcTransit || IsTransitAndNatal || IsTransitAndPDs || isSolarReturn || isLunarReturn || IsNewComparison2 || IsAgeHarmAndNatal)
	{
		for (int i = 0; i <= cObj; i++)
		{
			ignoreT[i] = ignore1[i];            // usr transit select
			if (us.fSector || us.fAstroGraph)
				ignore1[i] = ignore1[i] && ignore2[i];
			else
				ignore1[i] = ignore2[i];
		}
	}
	else if (us.nRel == rcProgress || us.nRel == rcProgTran)
	{
		us.fProgressUS = TRUE;
		//is.JDp = MdytszToJulian(ciCore.mon, ciCore.day, ciCore.yea, ciCore.tim, ciCore.dst, ciCore.zon);
		is.JDp = MdytszToJulian(ciCore.mon, ciCore.day, ciCore.yea, ciCore.tim, ciNatal.dst, ciCore.zon);
		ciCore = ciMain;
		for (int i = 0; i <= cObj; i++)
		{
			ignoreT[i] = ignore1[i];
			if (us.fSector || us.fAstroGraph)
				ignore1[i] = ignore1[i] && ignore3[i];
			else
				ignore1[i] = ignore3[i];                  // Progress planets
		}
	}

	if( !IsAgeHarmAndNatal && !us.f12parts2 && !us.fAnti && !us.fAnti2 && !us.fNavamsas2)
		t2 = CastChart(TRUE);// cp0   transit chart

	// restore our natal select 
	if (us.nRel == rcTransit || IsTransitAndNatal || IsTransitAndPDs || isSolarReturn || isLunarReturn || IsNewComparison2 || IsAgeHarmAndNatal)
	{
		memcpy(&ignore2,&ignore1,NUMBER_OBJECTS);
		memcpy(&ignore1,&ignoreT,NUMBER_OBJECTS);
	}
	else if (us.nRel == rcProgress || us.nRel == rcProgTran)
	{
		us.fProgressUS = FALSE;
		memcpy(&ignore3,&ignore1,NUMBER_OBJECTS);
		memcpy(&ignore1,&ignoreT,NUMBER_OBJECTS);
	}
	if(us.nRel == rcProgTran)
	{
		cpSave = cp1;
		cp1 = cp0;

		if(UsePDsInChart && us.nRel==rcProgTran)
		{
			cp1 = cpPDs;
			memcpy(cpPDs.cusp_pos,cpSave.cusp_pos,sizeof(cp1.cusp_pos));
		}
		cp2 = cpt;
	}
	else
		cp2 = cp0;

	if(us.fAnti) // 映点计算
	{
		Place place;
		double jut = DegMin2DecDeg(ciCore.tim) + 0.5 / 3600;
		int jhour = (int) jut;
		int jmin = (int) fmod(jut * 60, 60);
		int jsec = (int) fmod(jut * 3600, 60);
		timeP.time(ciCore.yea, ciCore.mon, ciCore.day, jhour, jmin, jsec, 0,  FALSE, ciCore.zon, ciCore.dst, place);

		double d = swe_deltat(timeP.jd);
		double obl[6];
		char serr[256];
		
		swe_calc(timeP.jd+d, SE_ECL_NUT, 0, obl, serr);
		
		for (i = 0; i <= cObj; i++)
		{
			if(ignore1[i])
				continue;

			double ant, cant;
			calc(cp1.longitude[i], &ant, &cant);
			cp2.longitude[i]=ant;
			cp2.latitude[i]=0.0;
		}	
	}

	if(us.fAnti2) // 反映点计算
	{
		Place place;
		double jut = DegMin2DecDeg(ciCore.tim) + 0.5 / 3600;
		int jhour = (int) jut;
		int jmin = (int) fmod(jut * 60, 60);
		int jsec = (int) fmod(jut * 3600, 60);
		timeP.time(ciCore.yea, ciCore.mon, ciCore.day, jhour, jmin, jsec, 0,  FALSE, ciCore.zon, ciCore.dst, place);
		double d = swe_deltat(timeP.jd);
		double obl[6];
		char serr[256];
		
		swe_calc(timeP.jd+d, SE_ECL_NUT, 0, obl, serr);
		
		for (i = 0; i <= cObj; i++)
		{
			if(ignore1[i])
				continue;

			double ant, cant;
			calc(cp1.longitude[i], &ant, &cant);
			cp2.longitude[i]=cant;
			cp2.latitude[i]=0.0;
		}	
	}
	if(us.f12parts2)
	{
		for (i = 0; i <= cObj; i++)
		{
			cp2.longitude[i]=calcDodecatemoria(cp1.longitude[i]);
			cp2.latitude[i]=0.0;
		}
	}

	if(us.fNavamsas2)
	{
		for (i = 0; i <= cObj; i++)
		{
			cp2.longitude[i]=Navamsa(cp1.longitude[i]);
			cp2.latitude[i]=0.0;
		}
	}

	if (us.nRel == rcDual)
	{
		if (!FCreateGrid(FALSE))
			return;
		memcpy(&ppowerT, &ppower1, cLastMoving * sizeof(double));

		PlanetPPower(wi.chs);

		memcpy(&ppower2, &ppower1, cLastMoving * sizeof(double));
		memcpy(&ppower1, &ppowerT, cLastMoving * sizeof(double));
	}

	hRevers = saveRev;
	ciCore = ciMain;

	/* Now combine the two charts based on what relation we are doing.   */
	/* For the standard -r synastry chart, use the house cusps of chart1 */
	/* and the planets positions of chart2.                              */

	ratio = (double) us.nRatio1 / ((double) (us.nRatio1 + us.nRatio2));
	if (us.nRel <= rcSynastry && !us.f12parts && !us.fAnti && !us.fAnti2)
	{
		memcpy(&cp0.cusp_pos, &cp1.cusp_pos, (NUMBER_OF_HOUSES + 1) * sizeof(double));
	}
	//		cp1.longitude[22]	167.54565074375003	double
	//		cp0.longitude[22]	343.60989109813170	double
	// (v1) + ((v2) - (v1)) * (v3)
	//  167+(167-343)*0.5
	/* For the -rc composite chart, take the midpoints of the planets/houses. */
	else if (us.nRel == rcComposite)
	{
		for (i = 0; i <= cObj; i++)
		{
			cp0.longitude[i] = Ratio(cp1.longitude[i], cp2.longitude[i], ratio);
			if (fabs(cp2.longitude[i] - cp1.longitude[i]) > 180.0)
				cp0.longitude[i] = Mod(cp0.longitude[i] + 360.0 * ratio);
			cp0.latitude[i] = Ratio(cp1.latitude[i], cp2.latitude[i], ratio);
			cp0.vel_longitude[i] = Ratio(cp1.vel_longitude[i], cp2.vel_longitude[i], ratio);
		}
		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
		{
			cp0.cusp_pos[i] = Ratio(cp1.cusp_pos[i], cp2.cusp_pos[i], ratio);
			if (fabs(cp2.cusp_pos[i] - cp1.cusp_pos[i]) > 180.0)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + 360.0 * ratio);
		}

		/* Make sure we don't have any 180 degree errors in house cusp    */
		/* complement pairs, which may happen if the cusps are far apart. */

		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
		{
			if (MinDistance(cp0.cusp_pos[sCap], Mod(cp0.cusp_pos[i] - Sign2Z(i + 3))) > 90.0)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + 180.0);
		}

		for (i = 1; i <= NUMBER_OF_HOUSES; i++)
		{
			if (fabs(MinDistance(cp0.cusp_pos[i], cp0.longitude[oAsc - 1 + i])) > 90.0)
				cp0.longitude[oAsc - 1 + i] = Mod(cp0.longitude[oAsc - 1 + i] + 180.0);
		}
	}
	/* For the -rm time space midpoint chart, calculate the midpoint time and */
	/* place between the two charts and then recast for the new chart info.   */
	else if (us.nRel == rcMidpoint)
	{
		ciT = ciCore;
		is.T = Ratio(t1, t2, ratio);
		double ti = (is.T * 36525.0) + 0.5;
		is.JD = floor(ti) + 2415020.0;
		ciCore.tim = RFract(ti) * 24.0;
		ciCore.zon = Ratio(DegMin2DecDeg(ciMain.zon), DegMin2DecDeg(ciTwin.zon), ratio);
		ciCore.dst = Ratio(DegMin2DecDeg(ciMain.dst), DegMin2DecDeg(ciTwin.dst), ratio);

		ciCore.tim -= ciCore.zon - ciCore.dst;
		if (ciCore.tim < 0.0)
		{
			ciCore.tim += 24.0;
			is.JD -= 1.0;
		}
		if (ciCore.tim > 24.0)
		{
			ciCore.tim -= 24.0;
			is.JD += 1.0;
		}

		JulianToMdy(is.JD, &ciCore.mon, &ciCore.day, &ciCore.yea);
		ciCore.lon = Ratio(DegMin2DecDeg(ciMain.lon), DegMin2DecDeg(ciTwin.lon), ratio);
		if (fabs(ciTwin.lon - ciMain.lon) > 180.0)
			ciCore.lon = Mod(ciCore.lon + 360.0 * ratio);
		ciCore.lat = Ratio(DegMin2DecDeg(ciMain.lat), DegMin2DecDeg(ciTwin.lat), ratio);
		ciCore.tim = DecDeg2DegMin(ciCore.tim);
		ciCore.dst = DecDeg2DegMin(ciCore.dst);
		ciCore.zon = DecDeg2DegMin(ciCore.zon);
		ciCore.lon = DecDeg2DegMin(ciCore.lon);
		ciCore.lat = DecDeg2DegMin(ciCore.lat);
		ciMain = ciCore;
		CastChart(TRUE);
		ciCore = ciT;
	}

	/* There are a couple of non-astrological charts, which only require the */
	/* number of days that have passed between the two charts to be done.    */
	else
		is.JD = fabs(t2 - t1) * 36525.0;
	ComputeInHouses();

	if (fCP3)
	{
		cp0 = cp3;
		fCP3 = 0;
	}
}

#include <fcntl.h>    // 文件控制
//=========================================== config.dat =============================
//-z0 0             ; Default Daylight time setting   [0 standard, 1 daylight]
//-z +0:00          ; Default time zone               [hours before GMT      ]
//-zl 0:10W 51:30N  ; Default longitude and latitude
//-zn "London, UK"  ; Default location name
//;Altitude=33.00
//;European settings expected:
//
//-Yd		; Display dates in D/M/Y instead of M/D/Y format.
//-Yt		; Display times in 24 hour instead of am/pm format. 
//:YXG 2223	; European glyphs
//
//
//;END - DO NOT REMOVE THIS LINE OR ADD ANYTHING BELLOW
//=========================================== config.dat =============================
const char* d_dst = "No";
const char* d_zon = "-8:00:00";
//const char* d_lon = "116\u00B023\u203250\u2032\u2032E";
//const char* d_lat = "39\u00B054\u203227\u2032\u2032N";
const char* d_lon = "116:23'50E";
const char* d_lat = "39:54'27N";
const char* d_nam = "此时此刻";
const char* d_loc = "北京, 中国";
const double d_alt = 39.00;

/* Given a string, return a floating point number corresponding to what the  */
/* string indicates, based on a given parsing mode, like above for integers. */
double RParseSz(const char* szEntry, int pm)
{
	char szLocal[cchSzMax], * sz, * pch, ch, chdot, minutes[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	int cch, havedot, dot1, dot2, newstyle = false, i, j, f = false;
	double r, rMinutes, rSeconds;

	/* First strip off any leading or trailing spaces. */
	for (cch = 0; (szLocal[cch] = szEntry[cch]) != 0; cch++)
		;
	while (cch && szLocal[cch - 1] == ' ')
		szLocal[--cch] = chNull;
	for (sz = szLocal; *sz && *sz == ' '; sz++, cch--);
	;
	/* Capitalize all letters and make colons be periods to be like numbers. */
	for (pch = sz; *pch; pch++)
	{
		ch = *pch;
		if (ch == ':' || ch == '\'' || ch == '\"')
			ch = '.';
		else
			ch = ChCap(ch);
		*pch = ch;
	}
	ch = sz[0];

	if (pm == 7 || pm == 8)
	{
		char szLocal2[30] = { 0 };
		int ii = 0;
		int len = strlen(szLocal);
		for (int i = 0; i < len; i++)
		{
			if (szLocal[ii] > 0)
				szLocal2[i] = szLocal[ii];
			else
			{
				szLocal2[ii] = '.';
				ii++;
			}
			ii++;
		}
		for (int i = 0; i < len; i++)
		{
			szLocal[i] = szLocal2[i];
		}
	}

	if (pm == pmTim)
	{
		/* For times, process "Noon" and "Midnight" (or just "N" and "M"). */
		if (ch == 'N')
			return 12.0;
		else if (ch == 'M')
			return 0.0;
	}
	else if (pm == pmDst)
	{
		/* For the Daylight time flag, "Daylight", "Yes", and "True" (or just */
		/* their first characters) are all indications to be ahead one hour.  */
		/* For French language we add "Oui" (yes) */
		//if (ch == 'D' || ch == 'Y' || ch == 'T' || ch == 'O')
		//	return 1.0;
		/* "Standard", "No", and "False" mean the normal zero offset. */
		/* For French language we add "Non" */
	//	else if (ch == 'S' || ch == 'N' || ch == 'F')
	//		return 0.0;
		if ((STR_CMPI(sz, wchar_to_char(L"Yes").c_str()) == 0) ||				  // test "Yes"
			(sz[0] == wchar_to_char(L"Yes")[0] && sz[1] == 0) ||		  // test "Y"
			(STR_CMPI(sz, "DT") == 0)) return 1.0;			  // test "DT"

		else if ((STR_CMPI(sz, wchar_to_char(L"No").c_str()) == 0) ||			  // test "No"
			(sz[0] == wchar_to_char(L"No")[0] && sz[1] == 0) ||		  // test "Y"
			(STR_CMPI(sz, "ST") == 0)) return 0.0;			  // test "DT"
		else return atof(sz);
	}
	else if (pm == pmZon)
	{
		/* For time zones, see if the abbrev is in a table, e.g. "EST" -> 5. */
		for (i = 0; i < cZone; i++)
		{
			if (!wi.chs)
			{
				if (NCompareSz(sz, szZon[i]) == 0)
					return rZon[i];
			}
			else
			{
				if (NCompareSz(sz, szZonC[i]) == 0)
					return rZon[i];
			}
		}
	}
	else if (pm == pmLon || pm == pmLat)
	{
		/* For locations, negate the value for an "E" or "S" in the middle    */
		/* somewhere (e.g. "105E30" or "27:40S") for eastern/southern values. */
		for (i = 0; i < cch; i++)
		{
			ch = sz[i];
			if (FCapCh(ch))
			{
				if (ch == 'E' || ch == 'S')
					f = true;
				sz[i] = '.';
				i = cch;
			}
		}
		ch = sz[0];
	}

	/* In new time and locations strings there are at least two dots now,    */
	/* so we have to locate, if this string has second dot. If not, it's old */
	/* style string. But some old style strings can have second dot at end,  */
	/* so we have to distinguish them from new strings with two dots. VA.    */

	newstyle = true;
	if (pm == pmTim || pm == pmLon || pm == pmLat || pm == pmZon)
	{
		havedot = 0;
		dot1 = 0;
		dot2 = 0;
		for (i = 0; i < cch; i++)
		{
			chdot = sz[i];
			if (chdot == '.')
			{
				if (havedot)
				{
					dot2 = i;
					i = cch;
				}
				else
				{
					dot1 = i;
					havedot = 1;
				}
			}
		}
		if (dot2 == 0 || dot2 == cch - 1)
			newstyle = false;

		/* Now, when we know that it is new style string, we have to convert */
		/* it to the old style.                                              */

		if (newstyle)
		{
			for (i = dot1 + 1; i < cch; i++)
			{
				j = i - (dot1 + 1);
				if (sz[i] >= '.' && sz[i] <= '9' && sz[i] != '/')
					minutes[j] = sz[i];
				/*        if (minutes[j] == chNull || j > 9)  */
				else
					i = cch;
			}
			rMinutes = atof(minutes);
			rSeconds = RFract(rMinutes) / 0.6;
			if (rSeconds >= 1.0)
				return rLarge;
			sprintf(minutes, "%6.4f", rSeconds);
			for (i = 2; i < 5; i++)
			{
				j = i - 2 + dot2;
				sz[j] = minutes[i];
			}
		}
	}

	/* Anything still at this point should be in a numeric format. */
	if (!FNumCh(ch) && ch != '+' && ch != '-' && ch != '.')
		return rLarge;
	r = (f ? -1.0 : 1.0) * atof(sz);

	if (pm == pmTim)
	{
		/* Backtrack over any time suffix, e.g. "AM", "p.m." and variations. */
		i = Max(cch - 1, 0);
		if (i && sz[i] == '.')
			i--;
		if (i && sz[i] == 'M')
			i--;
		if (i && sz[i] == '.')
			i--;
		bool suffixSet = false;
		if (i && sz[i + 1] == 'M')
		{
			ch = sz[i];
			if (ch == 'A') {		/* Adjust value appropriately */
				r = r >= 12.0 ? r - 12.0 : r;	/* if AM or PM suffix.        */
				suffixSet = true;
			}
			else if (ch == 'P') {
				r = r >= 12.0 ? r : r + 12.0;
				suffixSet = true;
			}
		}
		// check localized AM/PM suffix
		if (!suffixSet && strlen(sz) > wcslen(L"AM") && !STR_CMPI(sz + strlen(sz) - wcslen(L"AM"), wchar_to_char(L"AM").c_str()))
		{
			r = r >= 12.0 ? r - 12.0 : r;
		}
		if (!suffixSet && strlen(sz) > wcslen(L"PM") && !STR_CMPI(sz + strlen(sz) - wcslen(L"PM"), wchar_to_char(L"PM").c_str()))
		{
			r = r >= 12.0 ? r : r + 12.0;
		}
	}
	return r;
}

// 解析宽字符串为整数
int NParseSzW(wchar_t* szEntry, int pm)
{
    if (!szEntry) return 0;
    
    wchar_t szLocal[512], *sz;
    int cch, n, i;
    int returnflag = 0;

    // 首先去除前导和尾随空格
    for (cch = 0; (szLocal[cch] = szEntry[cch]) != 0; cch++)
        ;
    szLocal[cch] = L'\0';
    while (cch && (unsigned char)szLocal[cch - 1] <= ' ')
        szLocal[--cch] = L'\0';
    for (sz = szLocal; *sz && (unsigned char)(*sz) <= ' '; sz++, cch--)
        ;

    // IC名称特殊处理
    if (sz[0] == L'I' && sz[1] == L'C')
    {
        sz[2] = L' ';
        sz[3] = 0;
        cch++;
    }
    
    if (cch >= 3) // 至少需要3个字符
    {
        // 根据解析模式处理
        switch (pm)
        {
            // 月份解析
            case pmMon: // pmMon
                // 简化处理，实际项目中需要完整实现
                break;
                
            // 行星解析
            case pmObject: // pmObject
                // 简化处理，实际项目中需要完整实现
                break;
        }

        if(returnflag) return returnflag;
    }

    if (sz == NULL || sz[0] == L'\0') {
        return 0;
    }

    try {
        n = std::stoi(sz);
    } catch (...) {
        n = 0;
    }

    if (pm == pmYea) // 年份处理
    {
        // 处理"BC"（或"B.C.", "b.c"等）并转换
        // 例如"5BC"转换为-4
        i = std::max(cch - 1, 0);
        if (i && sz[i] == L'.')
            i--;
        if (i && towupper(sz[i]) == L'C')
            i--;
        if (i && sz[i] == L'.')
            i--;
        if (i && towupper(sz[i]) == L'B')
            n = 1 - n;
    }
    return n;
}

std::wstring OutStrChart()
{
	ET et;
	wchar_t sz[cchSzDef];
	int obj_left, obj_right, i, k, l;
	double rT;

	is.szFileScreen = NULL;


	//Undo ...
	CreateElemTable(&et);
	std::wstring OutStr;
	std::vector<std::wstring> OuttObjName;
	std::vector<std::wstring> Outlongitude;
	std::vector<std::wstring> Outlatitude;
	std::vector<std::wstring> OutlongitudePlanet;
	std::vector<std::wstring> OutOncusp_pos;
	std::vector<std::wstring> Outvel_longitude;
	std::vector<std::wstring> Outcusp_pos;
	/* Ok, now print out the location of each object. */
	for (obj_right = 1 - us.fSeconds, obj_left = 0; obj_right <= uranHi; obj_right++, obj_left++)
	{
		if (us.fSeconds)
		{
			if (FIgnore(obj_right))
			{
				continue;
			}
		}
		else
		{
			if (obj_right > cuspLo && ((obj_right < cuspHi) || FIgnore(obj_right)))
			{
				continue;
			}

			while (obj_right <= cuspHi && obj_left <= cuspHi && FIgnore(obj_left))
			{
				obj_left++;
			}
		}

		if (obj_right >= cuspLo && obj_right <= cuspHi - 2 && obj_left > cuspHi - 2)	/* Don't print blank */
		{
			continue;											/* lines.      VA.   */
		}

		if (obj_right <= cuspHi - 2 && obj_left > cuspHi - 2)
		{
			//if (!wi.chs)
			//{
			//	if (CountryID != 5)
			//		PrintTab(' ', 52);
			//	else
			//		PrintTab(' ', 51);
			//}
			//else
			//{
			//	PrintTab(' ', 55);
			//}
		}
		else
		{
			if (obj_right > cuspHi - 2)
			{
				obj_left = obj_right;
			}
			wchar_t sz[120];
			//AnsiColor(kObjA[obj_left]);

			if (wi.chs)
			{
				//wchar_t szA[120];
				//swprintf(szA, sizeof(szA) / sizeof(wchar_t), "%-10.10ls:", char_to_wchar(tObjName[obj_left]).c_str());
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%-10.10s:",  char_to_wchar(tObjName[obj_left]).c_str());
				OuttObjName.push_back(sz);
			}
			else
			{
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%-11.11s:",  char_to_wchar(tObjName[obj_left]).c_str());
				OuttObjName.push_back(sz);
			}
			if (IsPDsChartWithoutTable && UsePDsInChart)// pds in chart
				Outlongitude.push_back(SzZodiac(cpPDs.longitude[obj_left]));
			else
				Outlongitude.push_back(SzZodiac(cp0.longitude[obj_left]));

			if (IsPDsChartWithoutTable && UsePDsInChart)
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc", cpPDs.vel_longitude[obj_left] >= 0.0 ? L'*' : chRet);
			else
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%lc", cp0.vel_longitude[obj_left] >= 0.0 ? L'*' : chRet);
			Outvel_longitude.push_back(sz);

			//if (obj_left <= cThing || obj_left > cuspHi)  oFor
			if (obj_left <= oFor || obj_left > cuspHi)
			{
				if (IsPDsChartWithoutTable && UsePDsInChart)
					Outlatitude.push_back(SzAltitude(cpPDs.latitude[obj_left]));
				else
					Outlatitude.push_back(SzAltitude(cp0.latitude[obj_left]));
			}
			else
			{
				// 前下划线
				//if (wi.chs)
				//	PrintTab('_', us.fSeconds ? 11 : 8);
				//else
				//	PrintTab('_', us.fSeconds ? 10 : 7);
				Outlatitude.push_back(L"_     ");
			}

			wchar_t a1;
			if (IsPDsChartWithoutTable && UsePDsInChart)
				a1 = Dignify(obj_left, Z2Sign(cpPDs.longitude[obj_left]),wi.chs);// 		a1	8212 L'—'	wchar_t
			else
				a1 = Dignify(obj_left, Z2Sign(cp0.longitude[obj_left]),wi.chs);

			if (a1 == L'—')
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" (%lc)", a1);// 星体及它所在的星座
			else
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" (%lc)", a1);

			if (wi.chs)
				OutlongitudePlanet.push_back(FCusp(obj_left) ? L"    " : sz);
			else
				OutlongitudePlanet.push_back(FCusp(obj_left) ? L"   " : sz);

			//if (FCusp(obj_left) /*&& wi.chs && into*/)
				//PrintSzW(L" ");

			if (IsPDsChartWithoutTable && UsePDsInChart)
			{
				k = cpPDs.house_no[obj_left];
				l = Z2Sign(cpPDs.cusp_pos[k]);
			}
			else
			{
				k = cp0.house_no[obj_left];
				l = Z2Sign(cp0.cusp_pos[k]);
			}

			//swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" [%ls%2d] ",char_to_wchar(tSuffix[k]).c_str(), k);
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%d",k);
			OutOncusp_pos.push_back(sz);

			wchar_t a2[2] = { 0,0 };
			a2[0] = Dignify(obj_left, k,wi.chs);
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"[%ls", a2);

			//PrintSzW(FCusp(obj_left) ? L"  " : sz);

			a2[0] = Dignify(obj_left, l,wi.chs);
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%ls] ", a2);
			//PrintSzW(FCusp(obj_left) ? L"   " : sz);

			//if (obj_left > 19 && FCusp(obj_left))
			//{
			//	if (wi.chs)
			//		PrintSzW(L"  ");
			//}

			if (
				FObject(obj_left) ||
				obj_left == oNoNode || obj_left == oSoNode || obj_left == oLil || obj_left == oFor)
			{
				if (IsPDsChartWithoutTable && UsePDsInChart)
				{
					//PrintCh((byte)(cpPDs.vel_longitude[obj_right] < 0.0 ? '-' : '+'));
					rT = Rad2Deg(fabs(cpPDs.vel_longitude[obj_left]));
				}
				else
				{
					//PrintCh((byte)(cp0.vel_longitude[obj_right] < 0.0 ? '-' : '+'));
					rT = Rad2Deg(fabs(cp0.vel_longitude[obj_left]));
				}

				if (rT == 360)
					rT = 0;
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), us.fSeconds ? (rT < 10.0 ? L"%9.7f" : L"%9.6f") : (rT < 10.0 ? L"%5.3f" : L"%5.2f"), rT);
				//PrintSzW(sz);
			}
			else
			{
				// 后下划线
				//PrintTab('_', us.fSeconds ? 10 : 6);
			}
		}
		if (!us.fSeconds)
		{
			/* For some lines, we have to append the house cusp positions. */
			if (obj_right <= NUMBER_OF_SIGNS)
			{
				//PrintSzW(L"  -  ");
				//AnsiColor(kSignA(obj_right));
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%ls %2d: ", L"House cusp", obj_right);
				//PrintSzW(sz);
				if (IsPDsChartWithoutTable && UsePDsInChart)
					Outcusp_pos.push_back(SzZodiac(cpPDs.cusp_pos[obj_right]));
				else
					Outcusp_pos.push_back(SzZodiac(cp0.cusp_pos[obj_right]));
			}

			/* For some lines, we have to append the element table information. */

			if (obj_right == NUMBER_OF_SIGNS + 2)
			{
				//PrintSzW(L"      Car Fix Mut TOT");
			}
			else if (obj_right > NUMBER_OF_SIGNS + 2 && obj_right < NUMBER_OF_SIGNS + 7)
			{
				k = obj_right - (NUMBER_OF_SIGNS + 2) - 1;
				//AnsiColor(kElemA[k]);
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  %c%c%c%3d %3d %3d %3d",
						tElement[k][0], tElement[k][1], tElement[k][2],
						et.coElemMode[k][0], et.coElemMode[k][1], et.coElemMode[k][2], et.coElem[k]);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  %ls%4d %4d %4d %4d",
						char_to_wchar(tElement[k]).c_str(),
						et.coElemMode[k][0], et.coElemMode[k][1], et.coElemMode[k][2], et.coElem[k]);

				//PrintSzW(sz);
				//AnsiColor(kDefault);
			}
			else if (obj_right == NUMBER_OF_SIGNS + 7)
			{
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  TOT %2d %3d %3d %3d", et.coMode[0], et.coMode[1], et.coMode[2], et.coSum);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  共 %3d %4d %4d %4d", et.coMode[0], et.coMode[1], et.coMode[2], et.coSum);
				//PrintSzW(sz);
			}
			else if (obj_right == oCore)
			{
				//PrintTab(' ', 23);
			}
			else if (obj_right >= uranLo)
			{
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" Uranian #%d", obj_right - uranLo + 1);
				//PrintSzW(sz);
			}
			sz[0] = chNull;

			switch (obj_right - NUMBER_OF_SIGNS - 1)
			{
			case 1:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"  +:%2d", et.coYang);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" 阳:%2d", et.coYang);
				break;
			case 2:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   -:%2d", et.coYin);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   阴:%2d", et.coYin);
				break;
			case 3:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   M:%2d", et.coMC);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   上:%2d", et.coMC);
				break;
			case 4:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   N:%2d", et.coIC);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   下:%2d", et.coIC);
				break;
			case 5:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   A:%2d", et.coAsc);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   左:%2d", et.coAsc);
				break;
			case 6:
				if (!wi.chs)
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   D:%2d", et.coDes);
				else
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   右:%2d", et.coDes);
				break;
			case 7:
			{
				//PrintSzW(L"  ");
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"Learn: %d, Share: %d", et.coLearn, et.coShare);
			}
			break;
			}
			//PrintSzW(sz);
		}
		else	// print seconds
		{
			//PrintSzW(L" Decan: ");
			int isfSeconds = is.fSeconds;
			is.fSeconds = false;
			//if (IsPDsChartWithoutTable && UsePDsInChart)
			//	PrintSzW(SzZodiac(Decan(cpPDs.longitude[obj_right])));
			//else
			//	PrintSzW(SzZodiac(Decan(cp0.longitude[obj_right])));

			is.fSeconds = isfSeconds;
		}
		//PrintL();
		//wprintf(L"\n");
	}

	/* Do another loop to print out the stars in their specified order. */
	if (us.nStar)
	{
		for (i = starLo; i <= starHi; i++)
		{
			for (int j = 0; j < starLo; j++)
			{
				if (!ignoreSO[i] && !ignoreSO[j] && abs(cp0.longitude[j] - cp0.longitude[i]) <= 0.5)
				{
					j = cLastMoving + starname[i - cLastMoving];

					if (!wi.chs)
					{
						swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%-11.11s:",  char_to_wchar(tObjName[j]).c_str());
						//PrintSzW(sz);
					}
					else
					{
						char szA[120];
						sprintf(szA, "%-10.10s:", tObjName[j]);
						//PrintSz(szA);
					}
					//PrintSzW(SzZodiac(cp0.longitude[j]));
					//PrintSzW(L"   ");
					//PrintAltitude(cp0.latitude[j]);
					k = cp0.house_no[j];
					//AnsiColor(kSignA(k));

					//AnsiColor(kDefault);
					swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"   %ls %-8.8s  Star #%2d: %5.2f\n",
						us.fSeconds ? L"   " : L"", char_to_wchar(szStarConName[j - cLastMoving]).c_str(), i - cLastMoving, rStarBright[j - cLastMoving]);
					//PrintSzW(sz);
					break;

				}
			}
		}
	}

	OutStr += L"星体：";
	for (size_t i=0; i<Outlongitude.size(); ++i)
	{
		OutStr += Outlongitude[i] + L",";
	}
	OutStr += L"#逆行：";
	for (size_t i=0; i<Outvel_longitude.size(); ++i)
	{
		OutStr += Outvel_longitude[i] + L",";
	}
	OutStr += L"#宫位：";
	for (size_t i=0; i<OutOncusp_pos.size(); ++i)
	{
		OutStr += OutOncusp_pos[i] + L",";
	}
	OutStr += L"#星座：";
	for (size_t i=0; i<Outcusp_pos.size(); ++i)
	{
		OutStr += Outcusp_pos[i] + L",";
	}
	return OutStr;
}

std::wstring OutstrPrintAspect(int obj1, int sign1, int ret1, int asp, int obj2, int sign2, int ret2, byte chart, int field_length1, int field_length2)
{
	wchar_t sz[cchSzDef];
	std::wstring OutStr;

	//if (chart == 't')
	//	PrintSzW(sTrans);
	//else if (chart == 'e' || chart == 'u' || chart == 'U')
	//	PrintSzW(sProgr);

	if (wi.chs)
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" %c%c", chObj3C(obj1));
	else
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", char_to_wchar(tObjName[obj1]).c_str());

	OutStr+=sz;
	OutStr+=L"|";

	//AnsiColor(kSignA(sign1));

	//if (wi.chs)
	//	swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" %lc%c%c%c%lc", ret1 > 0 ? L'(' : (ret1 < 0 ? L'[' : L'<'), chSig3C(sign1), ret1 > 0 ? L')' : (ret1 < 0 ? L']' : L'>'));
	//else
	//	swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" %lc%c%c%c%lc", ret1 > 0 ? L'(' : (ret1 < 0 ? L'[' : L'<'), chSig3(sign1), ret1 > 0 ? L')' : (ret1 < 0 ? L']' : L'>'));
	//OutStr+=sz;
	//AnsiColor(asp > 0 ? kAspA[asp] : kWhite);

	//	AnsiColor(asp > 0 ? kAspA[asp] : kDefault);//OK
	//PrintCh(' ');
	if (asp == aSig)
	{
		if (!wi.chs)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"--> ");		/* Print a sign change. */
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"进入  ");
	}
	else if (asp == aDir)
		if (!wi.chs)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"S/%c", obj2 ? chRet : L'D');	/* Print a direction change. */
		else
		{
			if (obj2)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", L"变逆行");	/* Print a direction change. */
			else
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", L"变顺行");
		}
	else if (asp == 0)
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), chart == 'm' ? L"&" : L"with");
	else
	{
		if (!wi.chs)
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%-3ls", char_to_wchar(tAspectAbbrev[asp]).c_str());	/* Print an aspect. */
		else
		{
			std::wstring temp = char_to_wchar(tAspectAbbrev[asp]);
			wchar_t * a = addspace(const_cast<wchar_t*>(temp.c_str()), 4);
			wcscpy(sz, a);
			//swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%-2s", Lang(tAspectAbbrev, asp));	/* Print an aspect. */
		}
	}
	OutStr+=sz;
	OutStr+=L"|";
	//if (asp != aDir)
	//	PrintCh(' ');
	if (asp == aSig)
	{
		//AnsiColor(kSignA(obj2));//OK
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", char_to_wchar(tSignName[obj2]).c_str());
		OutStr+=sz;
	}
	else if (asp >= 0)
	{
		char buftmp[20];
		//AnsiColor(kSignA(sign2));//OK
		if (chart == 't' || chart == 'u' || chart == 'U')
			OutStr+=sNatal;
		memset(buftmp,0,sizeof(buftmp));
		if (wi.chs)
			sprintf(buftmp, "%c%c%c%c%c ", ret2 > 0 ? '(' : (ret2 < 0 ? '[' : '<'), chSig3C(sign2), ret2 > 0 ? ')' : (ret2 < 0 ? ']' : '>'));
		else
			sprintf(buftmp,"%c%c%c%c%c ", ret2 > 0 ? '(' : (ret2 < 0 ? '[' : '<'), chSig3(sign2), ret2 > 0 ? ')' : (ret2 < 0 ? ']' : '>'));
		;
		//OutStr+=char_to_wchar(buftmp).c_str();
		//AnsiColor(kObjA[obj2]);

		memset(buftmp, 0, sizeof(buftmp));
		if (wi.chs)
			sprintf(buftmp," %c%c", chObj3C(obj2));
		else
			sprintf(buftmp,"%s", tObjName[obj2]);
		OutStr+=char_to_wchar(buftmp).c_str();
	}
	//if (chart == 'D' || chart == 'T' || chart == 'U' || chart == 'a' || chart == 'A' || chart == 'm' || chart == 'M')
	//{
	//	if (wi.chs)
	//		PrintTab(' ', 6);
	//	else
	//	{
	//		if (CountryID != 2)
	//			PrintTab(' ', field_length2 - strlen(tObjName[obj2]));
	//		else
	//			PrintTab(' ', field_length2 - strlen(tObjName[obj2]) + 1);
	//	}
	//}
	return OutStr;
}
/* Display all aspects between objects in the chart, one per line, in       */
/* sorted order based on the total "power" of the aspect, as specified with */
/* the -a switch. The same influences used for -I charts are used here.     */
std::wstring OutStrChartAspect()
{
	int ca[cAspect + 1], co[NUMBER_OBJECTS];
	wchar_t sz[cchSzDef];
	int ihi, jhi, ahi, i, j, k, count = 0;
	double ip, jp, savepower, rPowSum = 0.0, p;
	int orb, saveorb;
	GridInfo tempgrid;
	wchar_t chdeg = 176;

	memset(ca, 0, sizeof(ca));
	memset(co, 0, sizeof(co));
	memcpy(&tempgrid, grid, sizeof(GridInfo));

	PlanetPPower(wi.chs);
	ComputeInfluence();
	std::wstring utStr = L"#相位：";
	std::wstring outaspect;
	for (;;)
	{
		// we initialize now the two variables used for sorting, but only one 
		// of them will be used for sorting, depending on the sort type
		savepower = -1e10;
		saveorb = 0x7FFF;

		// search for the next most powerful aspect or next smallest orb

		for (i = 0; i <= cObj; i++)
		{
			if (!FIgnore(i))
			{
				for (j = 0; j < i; j++)
				{
					if (!FIgnore(j))
					{
						if ((k = tempgrid.n[j][i]) != 0)
						{
							ip = i <= cLastMoving ? ppower1[i] : 2.5;
							jp = j <= cLastMoving ? ppower1[j] : 2.5;
							orb = tempgrid.v[j][i];
							p = rAspInf[k] * sqrt(ip * jp) * (1.0 - fabs((double)orb) / 60.0 / GetOrb(i, j, k));
						
							if (us.fParallel)
								p *= PowerPar;
						
							// here we sort by decreasing power or increasing orb
							if ((fSortAspectsByOrbs && abs(orb) < abs(saveorb)) || (!fSortAspectsByOrbs && p > savepower))
							{
								saveorb = orb;
								ihi = i;
								jhi = j;
								savepower = p;
								ahi = k;
							}
						}
					}
				}
			}
		}

		if (saveorb == 0x7FFF)		// exit when no other aspect found
			break;

		tempgrid.n[jhi][ihi] = 0;	// mark this aspect as having being processed
		count++;					// Display the current aspect
		rPowSum += savepower;

		outaspect = OutstrPrintAspect(jhi, Z2Sign(cp0.longitude[jhi]), (int)RSgn(cp0.vel_longitude[jhi]), ahi, ihi, Z2Sign(cp0.longitude[ihi]), (int)RSgn(cp0.vel_longitude[ihi]), 'a', 12, 12);
		outaspect += L",";
		utStr += outaspect;
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" | %2d%lc%02d' %lc", abs(saveorb) / 60, chdeg, abs(saveorb) % 60, us.fAppSep ? (saveorb < 0 ? L'a' : L's') : (saveorb < 0 ? L'-' : L'+'));
		//PrintSzW(sz);
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L" | %9.2f |", savepower);
		//PrintSzW(sz);

		// add from astrolog760
		PrintInDayEvent(j, ahi, i, -1);
		//wprintf(L"\n");
	}


	if (us.fAspSummary)
	{
		//PrintAspectSummary(ca, co, count, rPowSum);
		//PrintL2();
		//DisplayGrands();
	}
	//utStr += L"#]";
	return utStr;
}

std::wstring OutChartAspectRelation()
{
	int ca[cAspect + 1], co[NUMBER_OBJECTS];
	wchar_t sz[cchSzDef];
	int ihi, jhi, ahi, i = 0, j = 0, k = 0, m = 0, count = 0;
	double ip, jp, savepower, rPowSum = 0.0, p;
	int orb, saveorb;
	GridInfo tempgrid;
	byte ignoreT[NUMBER_OBJECTS] = {};
	memcpy(&tempgrid, grid, sizeof(GridInfo));//5
	//PrintL();//5

	std::wstring utStr = L"#相位：";
	std::wstring outaspect;
	switch(us.nRel)
	{
	case rcTransit:
	case rcProgTran: 
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore2(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = rTransitInf[i];
		break;
	
	case rcProgress:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore3(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = ppower1[i];
		break;
	
	case rcDual:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = ppower2[i];
		break;
	
	default:
		for (m = 0; m <= cObj; m++)
			ignoreT[m] = FIgnore(m);
		for (i = 0; i <= cLastMoving; i++)
			power2[i] = rObjInf[i];
	
		break;
	}
	for (;;)
	{
		savepower = -1e10;//5
		saveorb = 0x7FFF;//5
		for (i = 0; i <= cObj; i++)//5
		{
			if (!ignoreT[i])//5
			{
				for (j = 0; j < cObj; j++)//5
				{
					if (!FIgnore(j))//5
					{
						if (k = tempgrid.n[i][j])//5
						{
							ip = i <= cLastMoving ? power2[i] : 2.5;//5
							jp = j <= cLastMoving ? ppower1[j] : 2.5;//5
							orb = tempgrid.v[i][j];//5
							p = rAspInf[k] * sqrt(ip * jp) *
								(1.0 - fabs((double)orb) / 60.0 / GetOrb(i, j, k));//5

							if (us.fParallel)//5
								p *= PowerPar;
							if (fSortAspectsByOrbs && abs(orb) < abs(saveorb) ||
								!fSortAspectsByOrbs && p > savepower)//5
							{
								saveorb = orb;
								ihi = i;//5
								jhi = j;//5
								savepower = p;//5
								ahi = k;//5
							}
						}
					}
				}
			}
		}
		if (saveorb == 0x7FFF)		// exit when no other aspect found
			break;
		tempgrid.n[ihi][jhi] = 0;	// mark this aspect as having being processed
		rPowSum += savepower;
		ca[ahi]++;
		co[jhi]++;
		co[ihi]++;
		outaspect = OutstrPrintAspect(jhi, Z2Sign(cp1.longitude[jhi]), (int)RSgn(cp1.vel_longitude[jhi]), ahi,
			ihi, Z2Sign(cp2.longitude[ihi]), (int)RSgn(cp2.vel_longitude[ihi]), 'A', 12, 12);
		outaspect += L",";
		utStr += outaspect;
		swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%.6f", savepower);
		//PrintSzW(sz);
		//wprintf(L"\n");
	}
	if (us.fAspSummary)
	{
		//PrintAspectSummary(ca, co, count, rPowSum);
	}
	return utStr;
}

/* When true, initEnv() suppresses the version banner — used by the headless
 * astrolog32-cli so its stdout stays clean for golden diffing. */
bool g_fSilent = false;

void SetSilent(bool silent) { g_fSilent = silent; }

void initEnv()
{
	wchar_t szWindowName[1024];
	char svers[64]={0};
	char * ver = swe_version(svers);

	// 程序中使用的相位数量
	us.nAsp = cAspect;
	for (int i=0;i<=18;i++)
	{
		if( ignoreA[i] )
			us.nAsp--;
	}

	swprintf(szWindowName,sizeof(szWindowName)/sizeof(wchar_t),
	 L"%ls version %ls for Current System with Ephemeris: JPL DE406 / Swiss V%ls\n", 
	 char_to_wchar(szAppNameCore).c_str(), char_to_wchar(szVersionCore).c_str(), char_to_wchar(ver).c_str());
	if (!g_fSilent) wprintf(szWindowName);

	int i;
	memcpy(&ignoreSO,&ignore1,NUMBER_OBJECTS);
	for (i = starLo; i <= starHi; i++)
		ignore1[i]=1;
	eepp = 1;
}

//ChartMode 目前支持两种模式 1 MainChart 2 ChartData2
void SetChartData(const int ChartMode,const ChartInput& chartInput, bool useInput)
{
	//默认参数设置
	CI ciDefault = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
	ciDefault.dst = RParseSz(d_dst, pmDst);
	ciDefault.zon = RParseSz(d_zon, pmZon);
	ciDefault.lon = RParseSz(d_lon, pmLon);
	ciDefault.lat = RParseSz(d_lat, pmLat);
	ciDefault.alt = d_alt;
	swprintf(ciDefault.nam, sizeof(ciDefault.nam) / sizeof(wchar_t), L"%ls",char_to_wchar(d_nam).c_str());
	swprintf(ciDefault.loc, sizeof(ciDefault.loc) / sizeof(wchar_t), L"%ls",char_to_wchar(d_loc).c_str());

	is.S = stdout;
	//SwitchF(us.fEuroDate);// config.dat 中  -Yd 设置
	us.fEuroDate = 1;  //-Yd
	us.fEuroTime = 1;  //-Yt
	//:YXG 2223	
	//chart info input 
	CI ci = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
	//首先取默认参数
	ci = ciDefault;
	us.dstDef = ciDefault.dst;
	us.zonDef = ciDefault.zon;
	us.lonDef = ciDefault.lon;
	us.latDef = ciDefault.lat;
	us.altDef = ciDefault.alt;
	swprintf(us.szLocNameDef, sizeof(us.szLocNameDef) / sizeof(wchar_t), L"%ls", ciDefault.loc);
	//自动获取系统参数取默认时区和时间
	SetHereAndNow(&ci);

	//用户更改、输入参数,  更新覆盖 Chart info
	if(useInput) {
		ci.mon = chartInput.mon;
		ci.yea = chartInput.yea;
		ci.day = chartInput.day;
		ci.tim = chartInput.tim;
		ci.dst = chartInput.dst;
		ci.zon = chartInput.zon;
		ci.lat = chartInput.lat;
		ci.lon = chartInput.lon;
		ci.alt = chartInput.alt;
		swprintf(ci.nam, sizeof(ci.nam) / sizeof(wchar_t), L"%ls", chartInput.nam);
		swprintf(ci.loc, sizeof(ci.loc) / sizeof(wchar_t), L"%ls", chartInput.loc);
	}
	if (!g_fSilent)
	{
		if (ChartMode == 1)
		{
			wprintf(L"Main Chart:\n");
		}
		else if (ChartMode == 2)
		{
			wprintf(L"ChartData2:\n");
		}
		else
		{
			wprintf(L"Error Data:\n");
		}
		wprintf(L"year:%d, month:%d, day:%d, time:%f, dst:%f, zon:%f, lat:%f, lon:%f, alt:%f, name:%ls, loc:%ls\n",
			ci.yea, ci.mon, ci.day, ci.tim, ci.dst, ci.zon, ci.lat, ci.lon, ci.alt, ci.nam, ci.loc);
	}
	
	wi.nDlgChart = ChartMode;
	switch (wi.nDlgChart)
	{
	case 1:
		ciMain = ciCore = ci;
		IsDoubleReturn = false;
		memcpy(&ciNatal, &ci, sizeof(CI));
		break;
	case 2:
		ciTwin = ci;
		ciTran = ci;
		IsDoubleReturn = false;
		memcpy(&ciNatal2, &ci, sizeof(CI));

		//if (us.fPrimDirs)
		//{
		//	bool valid=TRUE;
		//	int y=0, m=0, d=0, ho=0, mi=0, se=0;
		//	double t=0.0, da=0.0;
		//	bool direct;
		//	//calc2(pds[CurrentRec].mundane, &valid, &y, &m, &d, &ho, &mi, &se, &t, &direct, &da);
		//	pds[CurrentRec].arc = da;
		//	bool cal_type = (
		//		ciTwin.yea < 1582 ||
		//		(ciTwin.yea == 1582 && (ciTwin.mon < 10 || ciTwin.mon == 10 && ciTwin.day < 15)
		//			)) ? 0 : 1;
		//	double jd = swe_julday(y, m, d, t, cal_type);
		//	pds[CurrentRec].time = jd;
		//}
		break;
	case 3:
		//ciThre = ci;
		//ciTran = ciThre;
		break;
	default:
		//ciFour = ci;
		break;
	}

	wi.fCast = true;
	us.fEuroDate = 1;
}

std::wstring GetMainChartAspect()
{
	std::wstring outstrchart = L"";
	std::wstring outStrchartaspect = L"";
	double cast_res = CastChart(1);
	//ChartListing();
	if (!g_fSilent) wprintf(L"CastChart: %lf\n", cast_res);
	outstrchart = OutStrChart();
	if (!g_fSilent)
	{
		wprintf(L">>>=== ChartsData Show ===>>> :\n");
		wprintf(L"%ls\n", outstrchart.c_str());
	}
	us.fAspList = true;
	FCreateGrid(false);
	us.fAspSummary = 1;
	outStrchartaspect = OutStrChartAspect();
	if (!g_fSilent)
	{
		wprintf(L">>>=== AspectData Show ===>>> :\n");
		wprintf(L"%ls\n", outStrchartaspect.c_str());
	}
	return L"本命[" + outstrchart + outStrchartaspect + L"#]";
}

std::wstring GetChartAspectRelation()
{
	std::wstring outstrchart = L"";
	std::wstring outStrchartaspect = L"";
	CastRelation();
	//double cast_res = CastChart(1);
	//wprintf(L"%lf\n", cast_res);
	//ChartListing();
	outstrchart = OutStrChart();
	if (!g_fSilent) wprintf(L"%ls\n", outstrchart.c_str());
	us.fAspList = true;
	FCreateGrid(false);

	us.fAspSummary = 1;
	if (!FCreateGridRelation(FALSE))
		return L"";
	//ChartAspectRelation();
	outStrchartaspect = OutChartAspectRelation();
	if (!g_fSilent)
	{
		wprintf(L">>>=== ChartAspectRelationData Show ===>>> :\n");
		wprintf(L"%ls\n", outStrchartaspect.c_str());
	}
	return L"本命[" + outstrchart + outStrchartaspect + L"#]";
}

void GetChartResult(CI& ciInput,bool useInput)
{
	//默认参数设置
	CI ciDefault = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
	ciDefault.dst = RParseSz(d_dst, pmDst);
	ciDefault.zon = RParseSz(d_zon, pmZon);
	ciDefault.lon = RParseSz(d_lon, pmLon);
	ciDefault.lat = RParseSz(d_lat, pmLat);
	ciDefault.alt = d_alt;
	swprintf(ciDefault.nam, sizeof(ciDefault.nam) / sizeof(wchar_t), L"%ls",char_to_wchar(d_nam).c_str());
	swprintf(ciDefault.loc, sizeof(ciDefault.loc) / sizeof(wchar_t), L"%ls",char_to_wchar(d_loc).c_str());

	is.S = stdout;
	//SwitchF(us.fEuroDate);// config.dat 中  -Yd 设置
	us.fEuroDate = 1;  //-Yd
	us.fEuroTime = 1;  //-Yt
	//:YXG 2223	
	//chart info input 
	CI ci = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
	//首先取默认参数
	ci = ciDefault;
	us.dstDef = ciDefault.dst;
	us.zonDef = ciDefault.zon;
	us.lonDef = ciDefault.lon;
	us.latDef = ciDefault.lat;
	us.altDef = ciDefault.alt;
	swprintf(us.szLocNameDef, sizeof(us.szLocNameDef) / sizeof(wchar_t), L"%ls", ciDefault.loc);
	//自动获取系统参数取默认时区和时间
	SetHereAndNow(&ci);

	//Todo...  输入参数校验  经纬度 地点 和 时区等
	//char SysZon[130];
	//memset(SysZon,0,sizeof(SysZon));
	//sprintf(SysZon, "%s", get_system_timezone());
	////默认值和当前系统的时区不对应，取当前系统的值，此后根据用户上传确定是否更新
	//if(strcmp(SysZon,d_zon) != 0)
	//	ci.zon = RParseSz(SysZon, pmZon);

	//用户更改、输入参数,  更新覆盖 Chart info
	if(useInput) ci = ciInput;
	//以下英文版用户输入
	//ci.yea = 2012;
	//ci.tim = RParseSz("12:30:00", pmTim);
	//ci.dst = RParseSz("No", pmDst);
	//ci.zon = RParseSz("0:00:00", pmZon);
	//ci.lon = RParseSz("0:10'00W", pmLon);
	//ci.lat = RParseSz("51:30'00N", pmLat);
	//ci.alt = 33.00;
	//swprintf(ci.nam, sizeof(ci.nam) / sizeof(wchar_t), L"Here And Now");
	//swprintf(ci.loc, sizeof(ci.loc) / sizeof(wchar_t), L"London, UK");
	//wi.chs = 0;

	//以下中文版用户输入
	////ci.mon = 5;
	////ci.day = 18;
	////ci.yea = 2012;
	//ci.tim = RParseSz("16:32:10", pmTim);
	//ci.dst = RParseSz("No", pmDst);
	//ci.zon = RParseSz("-8:00:00", pmZon);
	//ci.lon = RParseSz("116:23'50E", pmLon);
	//ci.lat = RParseSz("39:54'27N", pmLat);
	//ci.alt = 39.00;
	//swprintf(ci.nam, sizeof(ci.nam) / sizeof(wchar_t),L"%ls", L"此时此刻");
	//swprintf(ci.loc, sizeof(ci.loc) / sizeof(wchar_t),L"%ls", L"北京, 中国");
	//wi.chs = 1;
	if (!g_fSilent)
		wprintf(L"year:%d, month:%d, day:%d, time:%f, dst:%f, zon:%f, lat:%f, lon:%f, alt:%f, name:%ls, loc:%ls",
			ci.yea, ci.mon, ci.day, ci.tim, ci.dst, ci.zon, ci.lat, ci.lon, ci.alt, ci.nam, ci.loc);
	wi.nDlgChart = 1;
	switch (wi.nDlgChart)
	{
	case 1:
		ciMain = ciCore = ci;
		IsDoubleReturn = false;
		memcpy(&ciNatal, &ci, sizeof(CI));
		break;
	case 2:
		ciTwin = ci;
		ciTran = ci;
		IsDoubleReturn = false;
		memcpy(&ciNatal2, &ci, sizeof(CI));

		//if (us.fPrimDirs)
		//{
		//	int y = 0, m = 0, d = 0;
		//	double t = 0.0, da;
		//	//calc2(pds[CurrentRec].mundane, &valid, &y, &m, &d, &ho, &mi, &se, &t, &direct, &da);
		//	pds[CurrentRec].arc = da;
		//	bool cal_type = (
		//		ciTwin.yea < 1582 ||
		//		(ciTwin.yea == 1582 && (ciTwin.mon < 10 || ciTwin.mon == 10 && ciTwin.day < 15)
		//			)) ? 0 : 1;
		//	double jd = swe_julday(y, m, d, t, cal_type);
		//	pds[CurrentRec].time = jd;
		//}
		break;
	case 3:
		//ciThre = ci;
		//ciTran = ciThre;
		break;
	default:
		//ciFour = ci;
		break;
	}

	wi.fCast = true;
	us.fEuroDate = 1; 
	double res = CastChart(1);
	if (!g_fSilent)
	{
		wprintf(L"%lf\n", res);
		ChartListing();
	}

	us.fAspList = true;
	FCreateGrid(false);

	us.fAspSummary = 1;
	ChartAspect();
	//ListAspect();
}

/* ============================================================
 * P0.3 — astrolog32-cli machine-readable output.
 * GetChartMachineText() reproduces the original Astrolog @0203
 * "chart positions" format (9-decimal positions) so the refactored
 * library can be diffed byte-for-byte against the original-engine
 * golden samples (test/golden/*.golden.txt). The object/sign name
 * tables are verbatim copies of the original so the 3-char
 * abbreviations match exactly.
 * ============================================================ */
#include <cmath>
#include <cstring>
#include <string>

static const wchar_t* s_szSignAbbrevEnglish[13] = {
	L"", L"Ari", L"Tau", L"Gem", L"Can", L"Leo", L"Vir", L"Lib", L"Sco",
	L"Sag", L"Cap", L"Aqu", L"Pis"
};

/* First 43 entries (indices 0..cLastMoving) are the printable object names;
 * the @0203 writer only ever reads indices <= cLastMoving via the first 3
 * characters, so the trailing entries are irrelevant and omitted. */
static const wchar_t* s_szObjShortNameEnglish[] = {
	L"Earth", L"Sun ", L"Moon", L"Mercury", L"Venus", L"Mars",
	L"Jupiter", L"Saturn", L"Uranus", L"Neptune", L"Pluto",
	L"Chiron", L"Ceres", L"Pallas", L"Juno", L"Vesta",
	L"NoNode", L"SoNode", L"Lilith", L"Fortune", L"Vertex", L"EaPoint",
	L"Ascendant", L"2nd Cusp", L"3rd Cusp", L"IC ",
	L"5th Cusp", L"6th Cusp", L"Descendant", L"8th Cusp",
	L"9th Cusp", L"Midheaven", L"11th Cusp", L"12th Cusp",
	L"Cupido", L"Hades", L"Zeus", L"Kronos",
	L"Apollon", L"Admetos", L"Vulkanus", L"Poseidon", L"Proserpina"
};

static std::string s_w2u(const wchar_t* w)
{
	std::string out;
	if (!w) return out;
	for (; *w; ++w) {
		wchar_t wc = *w;
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

/* Quiet chart setup — mirrors SetChartData() core (default params, override
 * from ChartInput, set globals, cast) but emits NO debug wprintf so the CLI
 * stdout stays clean for golden diffing. */
static void SetupChartQuiet(const ChartInput& chartInput)
{
	CI ciDefault = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
	ciDefault.dst = RParseSz(d_dst, pmDst);
	ciDefault.zon = RParseSz(d_zon, pmZon);
	ciDefault.lon = RParseSz(d_lon, pmLon);
	ciDefault.lat = RParseSz(d_lat, pmLat);
	ciDefault.alt = d_alt;
	swprintf(ciDefault.nam, sizeof(ciDefault.nam) / sizeof(wchar_t), L"%ls", char_to_wchar(d_nam).c_str());
	swprintf(ciDefault.loc, sizeof(ciDefault.loc) / sizeof(wchar_t), L"%ls", char_to_wchar(d_loc).c_str());

	is.S = stdout;
	us.fEuroDate = 1;
	us.fEuroTime = 1;
	CI ci = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"",{0} ,L"",{0},0.0, 0.0, 0.0,0.0,0,0.0,0,L"",L"",L"",0.0,0.0,0.0,{0},{0} };
	ci = ciDefault;
	us.dstDef = ciDefault.dst;
	us.zonDef = ciDefault.zon;
	us.lonDef = ciDefault.lon;
	us.latDef = ciDefault.lat;
	us.altDef = ciDefault.alt;
	swprintf(us.szLocNameDef, sizeof(us.szLocNameDef) / sizeof(wchar_t), L"%ls", ciDefault.loc);
	SetHereAndNow(&ci);

	ci.mon = chartInput.mon;
	ci.yea = chartInput.yea;
	ci.day = chartInput.day;
	ci.tim = chartInput.tim;
	ci.dst = chartInput.dst;
	ci.zon = chartInput.zon;
	ci.lat = chartInput.lat;
	ci.lon = chartInput.lon;
	ci.alt = chartInput.alt;
	swprintf(ci.nam, sizeof(ci.nam) / sizeof(wchar_t), L"%ls", chartInput.nam);
	swprintf(ci.loc, sizeof(ci.loc) / sizeof(wchar_t), L"%ls", chartInput.loc);

	wi.nDlgChart = 1;
	ciMain = ciCore = ci;
	IsDoubleReturn = false;
	memcpy(&ciNatal, &ci, sizeof(CI));
	wi.fCast = true;
	us.fEuroDate = 1;
}

/* The exact, non-contiguous set of object indices emitted by the original
 * @0203 writer (observed identically in all 8 golden samples): the 10
 * planets (1..10), True Node (16), Fortune (19), the 12 house cusps
 * (22..33) and 16 "placeholder" star slots (45,46,47,52,57,67,74,79,81,82,
 * 91,93,99,104,114,117) that the original prints as constant default rows
 * (the refactor engine ignores those stars by default, so we reproduce the
 * original's constant values instead of omitting the rows). */
static const int s_goldenIdx[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 16, 19,
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
	45, 46, 47, 52, 57, 67, 74, 79, 81, 82, 91, 93, 99, 104, 114, 117
};
static const int s_goldenIdxN = (int)(sizeof(s_goldenIdx) / sizeof(s_goldenIdx[0]));

static bool s_isPlaceholder(int i)
{
	return i == 45 || i == 46 || i == 47 || i == 52 || i == 57 || i == 67 ||
		i == 74 || i == 79 || i == 81 || i == 82 || i == 91 || i == 93 ||
		i == 99 || i == 104 || i == 114 || i == 117;
}

/* Emit one @0203 "/YF" row with the original's exact field layout:
 * "/YF <name3><%3d deg> <sign><%13.9f min>,<%4d latdeg><%13.9f latmin>,
 * <%14.9f speed><%14.9f dist>". The name field is 3 characters; for cusp
 * "IC " the trailing blank is significant to byte-match the golden. */
static void EmitMachineRow(std::wstring& out, const wchar_t* name3,
	double rT, double lat, double speed, double dist)
{
	wchar_t sz[512];
	swprintf(sz, 512, L"/YF %ls%3d %ls%13.9f,%4d%13.9f,%14.9f%14.9f\n",
		name3,
		((int)rT) % 30,
		s_szSignAbbrevEnglish[((int)rT) / 30 + 1],
		RFract(rT) * 60.0,
		(int)lat,
		RFract(fabs(lat)) * 60.0,
		speed, dist);
	out += sz;
}

std::wstring GetChartMachineText(const ChartInput& chartInput)
{
	SetupChartQuiet(chartInput);
	CastChart(1);

	std::wstring out;
	wchar_t sz[512];

	swprintf(sz, 512, L"@0203  ; %ls chart positions.\n", szAppNameW);
	out += sz;
	swprintf(sz, 512, L"/zi \"%ls\" \"%ls\"\n", ciMain.nam, ciMain.loc);
	out += sz;

	for (int k = 0; k < s_goldenIdxN; k++)
	{
		int i = s_goldenIdx[k];

		if (s_isPlaceholder(i)) {
			/* Constant default row for an ignored star slot (original
			 * behaviour: longitude 0, speed 1 rad/day in degrees,
			 * distance 999). */
			wchar_t nm3[8];
			swprintf(nm3, 8, L"%3d", i);
			EmitMachineRow(out, nm3, 0.0, 0.0, 57.295779513, 999.0);
			continue;
		}

		const wchar_t* w = s_szObjShortNameEnglish[i];
		wchar_t nm3[4];
		nm3[0] = w[0]; nm3[1] = w[1]; nm3[2] = w[2]; nm3[3] = 0;

		/* Original legacy quirk: the 5th..10th cusp longitudes are read
		 * from cp0.cusp_pos[], the rest from cp0.longitude[]. */
		double rT = FBetween(i, cuspLo - 1 + 4, cuspLo - 1 + 9)
			? cp0.cusp_pos[i - (cuspLo - 1)]
			: cp0.longitude[i];
		double lat = cp0.latitude[i];

		/* The original engine stores a full 2*pi rad/day velocity
		 * (i.e. 360 deg/day) for Fortune and the house cusps; the
		 * refactor engine leaves it 0, so we reproduce the sentinel. */
		double speed = (i == oFor || FCusp(i))
			? 360.0
			: Rad2Deg(cp0.vel_longitude[i]);
		double dist = (i > cLastMoving)
			? 999.0
			: sqrt(spacex[i] * spacex[i] + spacey[i] * spacey[i] + spacez[i] * spacez[i]);

		EmitMachineRow(out, nm3, rT, lat, speed, dist);
	}
	return out;
}

std::string GetChartJSON(const ChartInput& chartInput)
{
	SetupChartQuiet(chartInput);
	CastChart(1);

	std::string s;
	s += "{\"app\":\"";
	s += s_w2u(szAppNameW);
	s += "\",\"objects\":[";
	bool first = true;
	for (int k = 0; k < s_goldenIdxN; k++)
	{
		int i = s_goldenIdx[k];
		std::string nm;
		double rT, lat, speed, dist;

		if (s_isPlaceholder(i)) {
			char b[12]; snprintf(b, sizeof(b), "%d", i); nm = b;
			rT = 0.0; lat = 0.0; speed = 57.295779513; dist = 999.0;
		} else {
			const wchar_t* w = s_szObjShortNameEnglish[i];
			char b3[4] = { (char)w[0], (char)w[1], (char)w[2], 0 };
			nm = b3;
			rT = FBetween(i, cuspLo - 1 + 4, cuspLo - 1 + 9)
				? cp0.cusp_pos[i - (cuspLo - 1)] : cp0.longitude[i];
			lat = cp0.latitude[i];
			speed = (i == oFor || FCusp(i)) ? 360.0 : Rad2Deg(cp0.vel_longitude[i]);
			dist = (i > cLastMoving) ? 999.0
				: sqrt(spacex[i]*spacex[i] + spacey[i]*spacey[i] + spacez[i]*spacez[i]);
		}
		if (!first) s += ",";
		first = false;
		char buf[512];
		snprintf(buf, sizeof(buf),
			"{\"name\":\"%s\",\"longitude\":%.9f,\"latitude\":%.9f,\"speed\":%.9f,\"distance\":%.9f}",
			nm.c_str(), rT, lat, speed, dist);
		s += buf;
	}
	s += "],\"houses\":{";
	bool hfirst = true;
	for (int h = 1; h <= NUMBER_OF_HOUSES; h++) {
		if (!hfirst) s += ",";
		hfirst = false;
		char buf[64];
		snprintf(buf, sizeof(buf), "\"%d\":%.9f", h, cp0.cusp_pos[h]);
		s += buf;
	}
	s += "}}";
	return s;
}
