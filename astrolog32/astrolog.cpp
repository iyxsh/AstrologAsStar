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


/*
******************************************************************************
** Warning Level Switches For Debugging.
******************************************************************************
*/

/* Compiler warnings switches needed for MS Visual C++ when warning level
is set to 4. Used for linting. Don't remove. */


#include "TransU/TransU.h"
#include "astrolog.h"
/*
warning 4706: assignment within conditional expression
warning 4057: 'function' : 'unsigned char *' differs in indirection
	 to slightly different base types from 'char [3]'
warning 4131: 'HousePlaceIn' : uses old-style declarator
warning 4244: '=' : conversion from 'int ' to 'unsigned char ',
	 possible loss of data
warning 4701: local variable 't_plac' may be used without
	 having been initialized
warning 4100: 'lParam' : unreferenced formal parameter
2010
*/
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1
#define _WIN32_WINNT 0x0500 
#define WIN32_LEAN_AND_MEAN 1
#pragma comment ( linker,"/ALIGN:4096" )
#pragma warning( disable : 4706 4057 4131 4244 4701 4100 4005 4996)

/*
******************************************************************************
** Configuration Constants.
******************************************************************************
*/

/* FEATURES SECTION: */

//#define TESTNEW
#define WIN

#define _CRT_SECURE_NO_DEPRECATE

#define PS			/* Comment out this #define if you don't want the ability to */
					/* generate charts in the PostScript graphics format.        */

#define META		/* Comment out this #define if you don't want the ability to  */
					/* generate charts in the MS Windows metafile picture format. */

#define INTERPRET	/* Comment out this #define if you don't want the ability */
					/* to display interpretations of the various chart types. */

#define INTERPRETALT

#define ARABIC		/* Comment out this #define if you don't want any chart     */
					/* lists that include Arabic parts included in the program. */

#define CONSTEL		/* Comment out this #define if you don't want any of the */
					/* astronomical constellation charts in the program.     */

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

/* For graphics, this char affects how bitmaps are written. 'N' is written
like with the 'bitmap' program, 'C' is compacted somewhat (files have
less spaces), and 'V' is compacted even more. 'A' means write as rectangular
Ascii text file. 'B' means write as Windows bitmap (.bmp) file. */
#define BITMAPMODE 'B'	

/*
** By the time you reach here and the above values are customized as
** desired, Astrolog is ready to be compiled! Be sure to similarly
** change the values in the astrolog.dat file, which will override any
** corresponding compile time values here. Don't change any of the
** values in the section below unless you know what you're doing.
*/

#define BITMAPX    2730			/* Maximum window size allowed */
#define BITMAPY    2730
#define BITMAPX1    180			/* Minimum window size allowed */
#define BITMAPY1    180
#define DEFAULTY    740
#define DEFAULTX    DEFAULTY+SIDESIZE-43		/* Default window size */

#define MAXMETA 200000L			/* Max bytes allowed in a metafile.            */
#define METAMUL      12			/* Metafile coordinate to chart pixel ratio.   */
#define PSMUL        11			/* PostScript coordinate to chart pixel ratio. */
#define CELLSIZE     14			/* Size for each cell in the aspect grid.      */
#define DEGINC        2			/* Number of degrees per segment for circles.  */
#define DEFORB      7.0			/* Min distance glyphs can be from each other. */
#define MAXSCALE    400			/* Max scale factor as passed to -Xs swtich.   */
#define TILTSTEP  11.25			/* Degrees to change when pressing '[' or ']'. */

#define BIODAYS      14			/* Days to include in graphic biorhythms.      */

#define chH    '-'
#define chV    '|'
#define chC    '|'
#define chNW   '+'
#define chNE   '+'
#define chSW   '+'
#define chSE   '+'
#define chJN   '-'
#define chJS   '-'
#define chJW   '|'
#define chJE   '|'

#define chDeg 0x20 
#define chDeg0 (unsigned char)(us.fAnsiChar ? /*0xB0*/ 0xb0 : 0x20) 
#define chDeg1 (unsigned char) 0xb0
//#define chDeg1 (unsigned char) ':'

#define chMin1 '\''
#define chSec1 '"'

// Atlas values
#define cchSzAtl 54
#define cchSzZon 13
#define icnewMax 252
#define icnusMax 51
#define icncaMax 13
#define icnUS    234  // United States
#define icnCA    38   // Canada
#define icnFR    76   // France
#define icnUK    78   // England
#define iznMax   425
#define ilistMax 200
#define rInvalid   (1.23456789E-09)

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
//#include "xml/xmlParser.h"
//#include "HtmlHelp.h"
//#pragma comment(lib, "HtmlHelp.lib")
//#pragma comment(lib, "urlmon.lib")

int fSouthIndian = 0, fNorthIndian = 0, fEastIndian = 0;
unsigned char chDeg2 = ':';
//void XChartIndian();
//char bigstr[10240];

char topic[260] = { "HELPFILE.html" };
BOOL MacroActivated = FALSE;
int dragflag;
int numA, numB;
BOOL IsHideLine = FALSE;
int SubMacVersion = 0;
void PrintCalculateParameters(BOOL print_flag);
void PrintNatalMsg();
void PrintChartDirMsg();
/*
******************************************************************************
** Program Constants.
******************************************************************************
*/

#ifdef PS
#define STROKE
#endif

#ifdef META
#define STROKE
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

static char *dms(double x, long iflag);
#define MoveTo(hdc, x, y) MoveToEx(hdc, x, y, NULL)
#define NAbs(n) abs(n)

typedef struct
{
	int Y;
	int M;
	int D;
	int h;
	int m;
	int s;
} DateTime;

DateTime dt;

//HWND PDs2=0;
double rise_set[2];
double location[3];
void DisplayJyotish();
void restore2single();
int MARKS = 0;
BOOL UseUserValue = FALSE;

#define nScrollDiv		24
#define nScrollPage		4.5
#define szFileTempCore	"ASTROLOG.TMP"
#define szFileTempCoreW	L"ASTROLOG.TMP"

//#define szTtyCore  "tty"
#define cchSzDef   200
#define cchSzMax   510

#define dayJ2G1    4
#define dayJ2G2    15
#define chNull     L'\0'
#define chEscape   '\33'
#define chBell     '\7'
#define chReturn   '\r'
#define chTab      '\t'
#define chDelete   '\b'
#define chBreak    '\3'
#define chRet      L'R'
#define starMaxName 12
#define starMaxNName 8
#define cStar		75

/* Array index limits */
#define cCnstl     88
#define cZone      69
#define cSector    36
#define cPart      177
#define cAspConfig 8
#define cWeek      7
#define cColor     16
#define xFont      6
int yFont = 12;

#define NUMBER_OF_MONTHS	12
#define NUMBER_OF_SIGNS		12
#define NUMBER_OF_HOUSES	12
#define NUMBER_OF_MACROS	121
int NUMBER_OF_MACROS2 = 0;

#define FIRST_SIDEREAL_MODE		SE_SIDM_FAGAN_BRADLEY	// NOTE: this must always be zero!!
#define LAST_SIDEREAL_MODE		SE_SIDM_GALCENT_0SAG

//StatusBar parts
#define SBP_POSITION   0
#define SBP_MODIFY     1
#define SBP_INSERT     2
#define SBP_NEWLINE    3
#define SBP_CODEPAGE   4
#define SBP_USER       5

BOOL showWiki = TRUE;
BOOL showJson = TRUE;
BOOL IsSpeculum = FALSE;
bool ischs = true;

double  SIDESIZE = 220;			/* Size of wheel chart information sidebar.    */
bool IsDoublePanel = false;

int CountryID = 1;
bool aspos = false;
char ret[6] = { -1,-1,-1,-1,-1,-1 };
int firstchoice3 = 0;
int firstchoice = 0;
int firstchoice2 = 0;
BOOL IsLahiri = FALSE;
BOOL IsNorth = FALSE;
BOOL IsEast = FALSE;
BOOL IsSouth = FALSE;
BOOL inmacro = FALSE;
BOOL IsAgeHarm = FALSE;
BOOL IsAgeHarmAndNatal = FALSE;
int eepp = -1;
char sMenu[16][120];
int nStatusParts = 0;
#define ID_STATUS         10002
BOOL IsZhengshi = FALSE;
BOOL IsF74 = FALSE;
BOOL disReception = FALSE;
BOOL disBazi = FALSE;
BOOL IsPDsData = FALSE;
BOOL OnPrint = FALSE;
int DataType = 1;

char ignoreS[118];          // for fixed star
char ignoreS1[118];
char ignoreSO[118];

int dir = 0;
std::wstring wstrLine;
wchar_t menutitle[128];
int where = -1;
int flag = 1;
int timeout = 0;
//CHOOSEFONTA cf;
char myBuff[64] = "Arial";
int sizea = 14;
int cWeight = 400;
unsigned long  bItalic = 0;

int codepage = -1;
int nTimer = 0;
int currentzod;
int currentsys;
int currentSid;
char ignore16;
char ignore17;
#define BUFLEN  8000
char asp[512];
char *buf;
BOOL oldColor;

BOOL fAnsiColor;
BOOL fAnsiChar;

BOOL IntoAni = FALSE;
int SolarReturnYear;
BOOL isSolarReturn = FALSE;
BOOL isLunarReturn = FALSE;
BOOL FoundSolarReturn = FALSE;

char FileName1[16];
char ephemeris[260] = { "" };
BOOL fSortForward = 1;
BOOL fConverseArc = 0;

void drawPlanets5();
wchar_t *SzLocation2(double lon, double lat);

BOOL IncludeAsteroids = FALSE;
BOOL IsPlanetModern = FALSE;
BOOL IsPlanetTraditional = TRUE;
BOOL IsPlanetLuminaries = FALSE;

BOOL IsDignitiesModern = FALSE;
BOOL PrintPrimDirToText = FALSE;
BOOL PrintAlmutenCToText = FALSE;
BOOL PrintAlmutenPToText = FALSE;
BOOL PrintPHsToText = FALSE;
BOOL PrintPDsPostions = FALSE;

BOOL IsBACKTOMAIN = FALSE;

BOOL UsePDsInChart = FALSE;
BOOL IsPDsChartWithoutTable = FALSE;   // Table
BOOL IsPDsChartWithoutTable2 = FALSE;  // PDs and natal

BOOL IsProfections = FALSE;
BOOL IsProfectionsDouble = FALSE;
BOOL IsDraconic = FALSE;
BOOL IsSyzygy = FALSE;
BOOL Is12Part = FALSE;

BOOL IsNewComparison2 = FALSE;
BOOL IsNewComparison = FALSE;
BOOL IsComparison = FALSE;
BOOL IsIngress = FALSE;
BOOL IsPassage = FALSE;
BOOL IsTransitAndNatal = FALSE;
BOOL IsProgressAndTransit = FALSE;
BOOL IsProgressAndNatal = FALSE;
BOOL IsTransitAndPDs = FALSE;
BOOL IsPDsChart = FALSE;

int PAGENUM = 0;
int CurrentPage = 0;
int CurrentLine = 0;
int CurrentRec = 0;
int LinePerPage = 21;
int pdrange = 0;
int DoPD = 0;
int direction = 0;
BOOL AUTOSAVE = FALSE;
BOOL Diurnal = FALSE;
int DoPr = 0;
BOOL isDayBirth;
int PDNum1 = 0;
double posio[10];
double posi[9];
bool retr[10];
int graha[9] = { 0,1,2,3,4,5,6,7,8 };
wchar_t as[9][120];
bool aflag[9];
int lagna;
bool useFixedLagna;
#define beginline 0
double plpos[13];
double plpos2[13];
double plpos3[13];
void drawPlanets();
void drawPlanets2();
void drawPlanets3();
void RestoreEnv();
void XChartSphere();
BOOL computeRiseSet();
void lunar_eclipse(int flag, char *buf);
void solar_eclipse(int flag, char *buf);

void XChartWheel2();
void XChartWheelRelation2();
void XChartWheelRelation12();
void XChartWheel0();
void XChartWheel0Comp();
void XChartWheelRelationX();
void XChartWheelRelation3();

int DIVISION_ORD = 0;
int DIVISION = 1;
bool isChalit = false;
int divs[] = { 1, 2, 3, 4, 7, 9, 10, 12, 16, 20, 24, 27, 30, 40, 45, 60, 1, 1, 1 };
int withRetr = false;
int rashiOfGraha[10];
wchar_t *SzZodiacJ(double);

int mahadasha = 0;
double nakshatraYears[] = { 7, 20, 6, 10, 7, 18, 16, 19, 17 };
int antaradasha = 0;
int periodAGrahas[18][9];
int periodPGrahas[18][9][9];
double periods[1458];
int pratyantaradasha = 0;
double jdStart = 0.0;
int selection = 0;
double selJD = 0.0;
const char* nkNames[9] = { "KT", "SK", "SY", "CH", "MA", "RH", "GU", "SA", "BU" };
int periodMGrahas[18] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 8 };

void makeTable();

int fenChart[19];
typedef enum
{
	Janma = 1,
	Hora,
	Drekkana,
	Chaturthamsha,
	Saptamsha,
	Navamsha,
	Dasamsha,
	Dvadasamsha,
	Shodasamsha,
	Vimshamsha,
	Siddhamsha,
	Bhamsha,
	Trimshamsha,
	Khavedamsha,
	Akshvedamsha,
	Shashtiamsha,
	Chalit,
	ChandraLagna,
	SuryaLagna
} partchart;

/* Elements */
typedef enum
{
	eFir,
	eEar,
	eAir,
	eWat
} elements_t;

/* Zodiac signs */
typedef enum
{
	sAri = 1,
	sTau,
	sGem,
	sCan,
	sLeo,
	sVir,
	sLib,
	sSco,
	sSag,
	sCap,
	sAqu,
	sPis
} zodiac_signs_t;

/* Objects */
typedef enum
{
	oEar,						// 0
	oSun,
	oMoo,
	oMer,
	oVen,
	oMar,
	oJup,
	oSat,
	oUra,
	oNep,
	oPlu,
	cLastMainPlanet = oPlu,		// 10

	oChi,						// 11
	oCeres,
	oPallas,
	oJuno,
	oVesta,
	cLastPlanet = oVesta,		// 15

	oNoNode,					// 16
	oSoNode,
	oLil,
	cThing = oLil,				// 18

	oFor,						// 19
	oVtx,
	oEP,
	oCore = oEP,				// 21

	cuspLo,						// 22
	oAsc = cuspLo,
	oCusp2nd,
	oCusp3rd,
	oNad,
	oCusp5th,
	oCusp6th,
	oDes,
	oCusp8th,
	oCusp9th,
	oMC,
	oCusp11th,
	oCusp12th,
	cuspHi = oCusp12th,			// 33

	uranLo,						// 34
	oCupido = uranLo,
	oHades,
	oZeus,
	oKronos,
	oApollon,
	oAdmetos,
	oVul,
	oPoseidon,
	oProserpina,
	uranHi = oProserpina,		// 42
	cLastMoving = uranHi,		// 42	

	starLo,						// 43
	starHi = starLo + cStar - 1,// 117
	cObj = starHi,				// 117

	NUMBER_OBJECTS				// 118
} objects_t;

#define cUran (uranHi - uranLo + 1)	// 9
#define oOri (starLo - 1 + 10)
#define oAnd (starLo - 1 + 47)

/* Aspects */
typedef enum
{
	aCon = 1,
	aOpp,
	aSqu,
	aTri,
	aSex,
	aInc,				// 6
	aSSx,
	aSSq,
	aSes,
	aQui,
	aBQn,
	cAspectInt = aBQn,	// 11
	aSQn,
	aSep,
	aNov,
	aBNv,
	aBSp,
	aTSp,
	aQNv,
	cAspect = aQNv		// 18
} aspects_t;

typedef enum
{
	cHousePlacidus,
	cHouseKoch,
	cHouseEqual,
	cHouseCampanus,
	cHouseMeridian,				// axial rotation system
	cHouseRegiomontanus,
	cHousePorphyry,
	cHouseMorinus,
	cHouseTopocentric,			// Polich/Page 
	cHouseAlcabitius,
	cHouseEqualMidheaven,
	cHouseNeoPorphyry,
	cHouseWhole,
	cHouseVedic,				// Vehlow
	cHouseNull,					// each sign is a house, starting from Aries
	cHouseShripati,				// Sripati, Shripati, Bhava Chakra, Hindu Bhava, Hindu Unequal
	NUMBER_OF_HOUSE_SYSTEMS,	// 16
} house_system_t;

enum SwitchCode
{
	scError = -1,
	scGlyph,
	scCharSet,
	scDegCharacter,
	scUseInternationalAtlas,
	scOmitWizard,
	scOmitRegionalWizard,
	scDebugFont,
	scSimplifiedMode,
	scAdvancedMode,
	scDisableMacroShortcuts,
	scDisableSave,
	scEnableSave,
	scTerminator
};

#define aDir -2
#define aSig -1

/* Biorhythm cycle constants */
#define brPhy 23.0
#define brEmo 28.0
#define brInt 33.0

/* Relationship chart modes */
typedef enum
{
	rcMarks2 = -12,
	rcMarks1,
	rcHarm,
	rcHarm2,
	rcSolarret,
	rcLunarret,
	rcProgTran,
	rcProgress,
	rcTransit,
	rcQuadWheel,
	rcTriWheel,
	rcDual,
	rcNone,			// 0
	rcSynastry,
	rcComposite,
	rcMidpoint,
	rcDifference,
	rcBiorhythm,	// 5
} relat_chart_ty;

/* Aspect configurations */
#define acS  1
#define acGT 2
#define acTS 3
#define acY  4
#define acGC 5
#define acC  6
#define acMR 7
#define acK  8

/* chart modes */
#define gWheel      1
#define gHouse      2
#define gGrid       3
#define gHorizon    4
#define gOrbit      5
#define gSector     6
#define gCalendar   7
#define gDisposit   8
#define gAstroGraph 9
#define gEphemeris 10
#define gWorldMap  11
#define gGlobe     12
#define gPolar     13
#define gBiorhythm 14
#define gAspect    15
#define gMidpoint  16
#define gArabic    17
#define gSign      18
#define gObject    19
#define gHelpAsp   20
#define gConstel   21
#define gPlanet    22
#define gMeaning   23
#define gSwitch    24
#define gObscure   25
#define gKeystroke 26
#define gRising    28
#define gTraTraHit 29
#define gTraTraInf 30
#define gTraNatHit 31
#define gTraNatInf 32
#define gProgramStatus 33
#define gTelescope 34
//////////////////////////////////////////////
#define gFirdaria 34
#define gPlanetaryHours 35
#define gSquareChart 36
#define gAlmtsPoint 37
#define gAlmtsChart 38
#define gPrimDirs   39
#define gProfection   40
#define gJyotish   41
#define gPrimDirs2      42
#define gPrimDirs3      43
//////////////////////////////////////////////
#define gDebugFont 44
#define gSolarEclipse 45
#define gLunarEclipse 46
#define gLunarPhases 47
#define gDifference 48
#define gSphere    49
#define g12parts   50
#define g12parts2   51
#define gAnti   52
#define gAnti2   53
#define gNavamsa   54
#define gZodRel	55
#define gDecennials	56
#define gHouseLord	57
#define gEmptyASP 58

/* Colors */
#define kReverse -2
#define kDefault -1
#define kBlack   0
#define kMaroon  1
#define kDkGreen 2
#define kOrange  3
#define kDkBlue  4
#define kPurple  5
#define kDkCyan  6
#define kLtGray  7
#define kDkGray  8
#define kRed     9
#define kGreen   10
#define kYellow  11
#define kBlue    12
#define kMagenta 13
#define kCyan    14
#define kWhite   15
#define kNull    16

/* Arabic parts */
#define apFor 0
#define apSpi 1

/* Draw text formatting flags */
#define dtCent   0x0
#define dtLeft   0x1
#define dtBottom 0x2
#define dtErase  0x4
#define dtScale  0x8
#define dtScale2 0x20
#define dtTop    0x10

/* User string parse modes */
#define pmMon			1
#define pmDay			2
#define pmYea			3
#define pmTim			4
#define pmDst			5
#define pmZon			6
#define pmLon			7
#define pmLat			8
#define pmObject		9
#define pmAspect		10
#define pmHouseSystem	11
#define pmSign			12
#define pmColor			13
#define pmMonEn			14
#define pmSignEn		15
#define pmObjectEn		16
#define pmWeek          18
/* Termination codes */
#define tcError -1
#define tcOK    0
#define tcFatal 1
#define tcForce 2

#define chDirSep '\\'
#define chSwitch '/'
#define hdcNil ((HDC)NULL)

// File type and version numbers appear at the head of files, with the 
// form @xxyy, where xx is the file type and yy the version number. 
// These numbers are used for compatibility checks.

#define FILE_TYPE_CHART					"01"
#define FILE_VERSION_CHART				"02"

#define FILE_TYPE_PLANET_POSITION		02
#define FILE_VERSION_PLANET_POSITION	3

#define FILE_TYPE_CONFIGURATION			3
#define FILE_VERSION_CONFIGURATION		11

#define FILE_TYPE_COUNTRY_LIST			"04"	// for international and american atlas
#define FILE_VERSION_COUNTRY_LIST		"00"

#define FILE_TYPE_COUNTRY_FILE			"05"	// for international atlas
#define FILE_VERSION_COUNTRY_FILE		"00"

#define FILE_TYPE_USA_STATE_FILE		"06"	// for american atlas
#define FILE_VERSION_USA_STATE_FILE		"00"

//#define FILE_TYPE_ALT_INTERPRET			"09"
//#define FILE_VERSION_ALT_INTERPRET		"01"

#define BUILD_DATE "BUILD_DATE"

/*
******************************************************************************
** Type Definitions.
******************************************************************************
*/

typedef unsigned char byte;
typedef unsigned char *pbyte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned int uint;
typedef int KI;

// Windows accepts names up to 256 characters, which can be 512 bytes for 
// Unicode, however Astrolog does not do Unicode, so we use a size of 256. Note 
// that the Windows defile MAX_PATH actually has 260 bytes. For our purposes 
// using that would cause us some difficulties when comparing sizes, so we 
// create our own definition MAX_FILE_NAME. 
#define MAX_FILE_NAME	256
typedef char file_name_t[MAX_FILE_NAME + 1];	// File name or directory or a combination

typedef struct
{
	char		name[MAX_STRING_NAME_LEN];	// name used in user's interface
	file_name_t dir;	// actual directory
}
directory_t;

typedef struct
{
	// This is the drive plus directory where astrolog32.exe resides. This 
	// directory is one of the places Astrolog looks for files.
	directory_t executable;

	// we have several directories where we put different sorts of files.

	// Astrolog32.exe, astrolog32.dat, icons, URLs, documentation files, 
	// fixed stars catalogue.
	directory_t main;

	// This directory will store Swiss Ephemeris files. Note that while a list 
	// of search directories is passed by Astolog32 to Swiss Ephemeris, Swiss 
	// Ephemeris has its own file search routine. For example, one of the places 
	// it looks for is the directory specified by the environment variable 
	// SE_EPHE_PATH. This is the current behaviour and will not be changed, as 
	// Astolog (currently) uses Swiss Ephemeris in its unchanged form. 
	directory_t ephemeris;

	// Charts and cp0.longitude position files.
	directory_t charts;

	// Interpretation files (pl01.dat, house10.dat, etc).
	directory_t interpretation;

	// Miscellaneous files, such as chart saved as either text, bitmap, Windows 
	// Meta File, or Postscript.
	directory_t miscel;

	// Atlas files. 
	directory_t american_atlas;
	directory_t international_atlas;
}
global_directories_t;

typedef struct _GridInfo
{
	int n[NUMBER_OBJECTS][NUMBER_OBJECTS];
	short v[NUMBER_OBJECTS][NUMBER_OBJECTS];
}
GridInfo;

typedef struct _CrossInfo
{
	double lat[MAXCROSS];
	double lon[MAXCROSS];
	int obj1[MAXCROSS];
	int obj2[MAXCROSS];
}
CrossInfo;

typedef struct _UserSettings
{
	/* Chart types */
	BOOL fListing;				/* -v */
	BOOL fWheel;				/* -w */
	BOOL fGrid;					/* -g */
	BOOL fAspList;				/* -a */
	BOOL fMidpoint;				/* -m */
	BOOL fHorizon;				/* -Z */
	BOOL fOrbit;				/* -S */
	BOOL fSector;				/* -l */
	BOOL fInfluence;			/* -j */
	BOOL fAstroGraph;			/* -L */
	BOOL fCalendar;				/* -K */
	BOOL fInDay;				/* -d */
	BOOL fInDayInf;				/* -D */
	BOOL fEphemeris;			/* -E */
	BOOL fTransit;				/* -t */
	BOOL fTransitInf;			/* -T 16*/

	///////////////////////////////////////////////////
	BOOL fFirdaria;
	BOOL fPlanetaryHours;
	BOOL fSquareChart;
	BOOL fAlmtsPoint;
	BOOL fAlmtsChart;
	BOOL fPrimDirs;
	BOOL fProfection;
	BOOL fJyotish;
	BOOL fPrimDirs2;
	BOOL fPrimDirs3;
	///////////////////////////////////////////////////

	/* Chart suboptions */
	BOOL fVelocity;				/* -v0 27*/
	BOOL fWheelReverse;			/* -w0 */
	BOOL fGridConfig;			/* -g0 */
	BOOL fAppSep;				/* -ga */
	BOOL fParallel;				/* -gp */
	BOOL fAspSummary;			/* -a0 */
	BOOL fMidSummary;			/* -m0 */
	BOOL fMidAspect;			/* -ma */
	BOOL fPrimeVert;			/* -Z0 */
	BOOL fHorizonSearch;		/* -Zd */
	BOOL fSectorApprox;			/* -l0 */
	BOOL fInfluenceSign;		/*38 -j0 */
	BOOL fLatitudeCross;		/* -L0 */
	BOOL fCalendarYear;			/* -K  */
	BOOL fInDayMonth;			/* -dm */
	BOOL fArabicFlip;			/* -P0 42*/

	/* Table chart types */
	BOOL fSwitch;				/* -H  */
	BOOL fSwitchRare;			/* -Y  */
	BOOL fKeyGraph;				/* -HX */
	BOOL fSign;					/* -HC */
	BOOL fObject;				/* -HO */
	BOOL fAspect;				/* -HA */
	BOOL fConstel;				/* -HF */
	BOOL fOrbitData;			/* -HS */
	BOOL fMeaning;				/* -HI */
	BOOL fPrintProgramStatus;
	BOOL fDebugFont;
	BOOL fSolarEclipse;
	BOOL fLunarEclipse;
	BOOL fLunarPhases;
	BOOL f12parts;
	BOOL f12parts2;
	BOOL fAnti;
	BOOL fAnti2;
	BOOL fNavamsas2;

	/* Main flags */
	BOOL fSidereal;			/* -s 57*/
	BOOL fProgressUS;		/* Are we doing a -p progressed chart?           */
	BOOL fInterpret;		/* Is -I interpretation switch in effect?        */
	BOOL fDecan;			/* -3 */
	BOOL fFlip;				/* -f */
	BOOL fGeodetic;			/* -G */
	BOOL fVedic;			/* -J */
	BOOL fNavamsa;			/* -9 */
	BOOL fWriteFile;		/* -o */
	BOOL fAnsiColor;		/* -k */
	BOOL fGraphics;			/* -X 67*/

	/* Main subflags */
	BOOL fNoSwitches;
	BOOL fSeconds;			/* -b0 */
	BOOL fEquator;			/* -sr                      */
	BOOL fSolarArc;			/* -p0, etc */
	BOOL fWritePos;			/* -o0 */
	BOOL fAnsiChar;			/* -k0 */

	/* Rare flags */
	BOOL fTrueNode;			/* -Yn */
	BOOL fEuroDate;			/* -Yd */
	BOOL fEuroTime;			/* -Yt */
	BOOL fSmartAspects;		/* -YC */
	BOOL fClip80;			/* -Y8 */
	BOOL fHouseAngle;		/* -Yc */
	BOOL fIgnoreSign;		/* -YR0 */
	BOOL fIgnoreDir;		/* -YR0 */
	BOOL fNoWrite;			/* -0o */
	BOOL fNoRead;			/* -0i */
	BOOL fNoQuit;			/* -0q */
	BOOL fNoGraphics;		/* -0X */

	/* Value settings */
	int nEphemYears;			/* -Ey */
	int nArabic;				/* -P */
	int nRel;					/* What relationship chart are we doing, if any? */
	int nHouseSystem;			/* -c */
	int   nHouse3D;      // -c3
	int objCenter;				/* -h */
	int nStar;					/* -U */

	//////////////////////////////////////////////////////////
	int nTerms;
	int nFace;
	int nTriplicities;
	int nDode;
	//////////////////////////////////////////////////////////

	double nHarmonic;				/* Harmonic chart value passed to -x switch.     */
	int objOnAsc;				/* House value passed to -1 or -2 switch.        */
	int dayDelta;				/* -+, -- */
	int nDegForm;				/* -s */
	int nDivision;				/* -d */
	int nScreenWidth;			/* -I */
	int nSiderealMode;			/* -sm */
	double dstDef;				/* -z0 */
	double zonDef;				/* -z  */
	double lonDef;				/* -zl  */
	double latDef;				/* -zl  */
	wchar_t szLocNameDef[120];			/* -zn */
	BOOL useInternationalAtlas;	/*#UseInternationalAtlas*/
	BOOL advancedMode;			/* #AdvancedMode or #SimplifiedMode*/
	BOOL omitWizard;			/* #OmitWizard */
	BOOL omitRegionalWizard;	/* #OmitRegionalWizard */

	/* Value subsettings */
	int nWheelRows;				/* Number of rows per house to use for -w wheel. */
	int nAstroGraphStep;		/* Latitude step rate passed to -L switch.       */
	int nArabicParts;			/* Arabic parts to include value passed to -P.   */
	double rSiderealCorrection;	/* Position shifting value passed to -s switch
									used in sidereal charts only.				 */
	double rProgDay;				/* Progression day value passed to -pd switch.   */
	int nRatio1;				/* Chart ratio factors passed to -rc or -rm.     */
	int nRatio2;
	int nScrollRow;				/* -YQ */
	int nArabicNight;			/* -YP */
	int nBioday;				/* -Yb */
	int fSmartSave;
	int fHouse3D;     /* -c3 */
	BOOL fDifference;
	BOOL fSphere;
	double rZodiacOffset;   /* Position shifting value passed to -s switch.  */
	int nTriplicities2;
	BOOL fTopocentric;
	BOOL fEphemFiles;
	BOOL fPlacalcPla;
	double altDef;
	double Nutation;
	int   nAtlasList;       // Number of rows to display value passed to -N.
	int fEuroDist;     // -Yv
	int  nAstroGraphDist;  // Maximum crossing distance passed to -L0 switch.
	BOOL fGridMidpoint;
	BOOL fGraphAll;
	BOOL fIndian;
	BOOL fSmartCusp;
	int objRequire;
	BOOL fEclipse;
	BOOL fEclipseAny;
	BOOL fDistance;
	int nAppSep;
	BOOL fZodRel;
	BOOL fDecennials;
	BOOL fHouseLord;
	BOOL fEmptyASP;
	BOOL fListDecan;
	BOOL fEquator2;
	BOOL fParallel2;
	int nAsp;
	BOOL fAspect3D;
	BOOL fAspectLat;
	int nAspectSort;
	int nSignDiv;
	int nCharset;
	BOOL fMoonMove;
}
US;

typedef struct _AtlasEntry {
	double lon;              // Longitude of city
	double lat;              // Latitude of city
	short icn;             // Country or region of city
	short istate;          // State or province of city, if US or CA
	wchar_t szNam[cchSzAtl];  // Name of city
	short izn;             // Time zone area of city
} AtlasEntry;

typedef struct _TimezoneChange {
	int zon;      // Time zone value (in seconds before UTC)
	int irun;     // Daylight Saving rule (if any)
	int dst;      // Daylight offset to always use (if no rule)
	short yea;    // Year time zone value ends
	char mon;     // Month time zone value ends
	char day;     // Day time zone value ends
	int tim;      // Time that time zone value ends (in seconds)
	int timtype;  // Type of time (0=local, 1=standard, 2=UTC)
} ZoneChange;

typedef struct _TimezoneRuleName {
	wchar_t szNam[cchSzZon];  // Name of rule
	int irue;              // Start index of rule entries
} RuleName;

typedef struct _TimezoneRuleEntry {
	short yea1;    // Start year rule applies to
	short yea2;    // End year rule applies to
	char mon;      // Month in each year rule takes place
	char daytype;  // Type of day (0=num, 1=lastDOW, 2=DOW>=num, 3=DOW<=num)
	char daynum;   // Day within month rule takes place
	char dayweek;  // Day of week (DOW) rule takes place
	int tim;       // Time within day rule takes place
	int timtype;   // Type of time (0=local, 1=standard, 2=UTC)
	int dst;       // Rule applies this Daylight offset (in seconds before UTC)
} RuleEntry;

typedef struct _InternalSettings
{
	BOOL fProgressIS;			/* Are we doing a chart involving progression?       */
	BOOL fReturn;				/* Are we doing a transit chart for returns?         */
	int fMult;					/* Have we already printed at least one text chart?  */
	BOOL fSeconds;				/* Do we print locations to nearest second?          */
	BOOL fSzInteract;			/* Are we in middle of chart so some setting fixed?  */
	BOOL fNoEphFile;			/* Have we already had a ephem file not found error? */
	char *szProgName;			/* The name and path of the executable running.      */
	wchar_t *szFileScreen;			/* The file to send text output to as passed to -os. */
	wchar_t *szFileOut;			/* The output chart filename string as passed to -o. */
	char **rgszComment;			/* Points to any comment strings after -o filename.  */
	int cszComment;				/* The number of strings after -o that are comments. */
	int cchCol;					/* The current column text charts are printing at.   */
	int cchRow;					/* The current row text charts have scrolled to.     */
	double rSid;					/* Sidereal offset degrees to be added to locations. */
	double JD;					/* Fractional Julian day for current chart.          */
	double JDp;					/* Julian day that a progressed chart indicates.     */
	FILE *S;					/* File to write text to.   */
	double T;						/* Julian time for chart (expressed as number of	 */
								/* Julian centuries since noon GMT 1/Jan/1900)		 */
	double MC;					/* Midheaven at chart time. */
	double Asc;					/* Ascendant at chart time. */
	double RAa;					/* Right ascension at time. */
	double rObliquity;					/* Obliquity of ecliptic.   */
	double latMC;
	double lonMC;
	double rSidPD;
	double OB;  /* Obliquity of ecliptic.   */
	AtlasEntry *rgae;    // List of atlas entries for city coordinates.
	FILE *fileIn;        // The switch file currently being read from.
	int cae;             // Number of atlas entries of city locations loaded.
	ZoneChange *rgzc;    // List of time zone change entries for zone areas.
	RuleName *rgrun;     // List of Daylight Saving change rule names.
	RuleEntry *rgrue;    // List of all Daylight Saving change rule entries.
	int cAlloc;          // Number of memory allocations currently allocated.
	int cAllocTotal;     // Total memory allocations allocated this session.
	int cbAllocSize;     // Total bytes in all memory allocations allocated.
	int crun;            // Number of time zone Daylight rule categories.
	int crue;            // Total number of rule entries in all categories.
	int czcn;            // Number of time zone change areas loaded.
	int czce;            // Total number of change entries in all zone areas.

	double Vtx;            // Vertex at chart time.
	int nObj;
}
IS;

typedef struct _ChartInfo
{
	int mon;					/* Month            */
	int day;					/* Day              */
	int yea;					/* Year             */
	double tim;					/* Time in hours    */
	double dst;					/* Daylight offset  */
	double zon;					/* Time zone        */
	double lon;					/* Longitude        */
	double lat;					/* Latitude         */
	double alt;
	wchar_t nam[256];					/* Name for chart   */
	wchar_t loc[256];					/* Name of location */
	char RoddenRating[20];
	wchar_t Categories[4096];
	char Gender[20];
	double julian_day_ut;
	double sunrise;
	double sunset;
	double life_sign_pos;
	int natal;
	double LAT1;
	BOOL daybirth;
	wchar_t desc[10240];
	wchar_t file[260];
	wchar_t Events[4096];
	double Sun;
	double Moon;
	double ASC;
	char ASP[512];
	char house[8000];
}
CI;

typedef struct _PT3R {
	double x;
	double y;
	double z;
} PT3R;

typedef struct _ChartPositions
{
	double longitude[NUMBER_OBJECTS];			/* The zodiac positions.    */
	double latitude[NUMBER_OBJECTS];			/* Ecliptic declination.    */
	double vel_longitude[NUMBER_OBJECTS];			/* Retrogradation velocity. */
	double vel_latitude[NUMBER_OBJECTS];		/* Speed in declination  */
	double cusp_pos[NUMBER_OF_HOUSES + 1];	/* House cusp positions.    */
	double cusp3[NUMBER_OF_HOUSES + 1];  // 3D house cusp positions.
	byte house_no[NUMBER_OBJECTS];			/* House each object is in. */

	double vel_distance[NUMBER_OBJECTS];  // Distance velocity
	PT3R pt[NUMBER_OBJECTS];      // X,Y,Z coordintes in space
	double dist[NUMBER_OBJECTS];    // Distance to X,Y,Z coordinates

	double deltaT;
}
CP;

typedef struct _GraphicsSettings
{
	BOOL fBitmap;				/* Are we creating a bitmap file (-Xb set).         */
	BOOL fPS;					/* Are we generating a PostScript file (-Xp set).   */
	BOOL fMeta;					/* Are we generating a metafile graphic (-XM set).  */
	BOOL fColor;				/* Are we drawing a color chart (-Xm not set).      */
	BOOL fInverse;				/* Are we drawing in reverse video (-Xr set).       */
	BOOL fRoot;					/* Are we drawing on the X11 background (-XB set).  */
	BOOL fText;					/* Are we printing chart info on bottom of graphics chart (-Xt set).   */
	BOOL fFont;					/* Are we simulating fonts in charts (-XM0 set).    */
	BOOL fBonusMode;			/* Are we drawing in alternate mode (-Xi set).      */
	BOOL fBorder;				/* Are we drawing borders around charts (-Xu set).  */
	BOOL fLabel;				/* Are we labeling objects in charts (-Xl not set). */
	BOOL fJetTrail;				/* Are we not clearing screen on updates (-Xj set). */
	BOOL fMouse;				/* Are we not considering PC mouse inputs.          */
	BOOL fConstel;				/* Are we drawing maps as constellations (-XF set). */
	BOOL fMollewide;			/* Are we drawing maps scaled correctly (-XW0 set). */
	BOOL fPrintMap;				/* Are we printing globe names on draw (-XP0 set).  */
	int xWin;					/* Current size of graphic chart (-Xw).      */
	int yWin;
	int nAnimationJumpRate;		/* Current animation rate (-Xn).			*/
	int nScale;					/* Current character scale factor (-Xs).     */
	int objLeft;				/* Current object to place on Asc (-X1).     */
	int nTextRows;				/* Numb. of rows to set text screen to (-V). */
	int nRot;					/* Current rotation degree of globe.         */
	double rTilt;					/* Current vertical tilt of rotating globe.  */
	char chBmpMode;				/* Current bitmap file type (-Xb).           */
	int nOrient;				/* PostScript paper orientation indicator.   */
	double xInch;					/* PostScript horizontal paper size inches.  */
	double yInch;					/* PostScript vertical paper size inches.    */
	char *szDisplay;			/* Current X11 display name (-Xd).           */
	int nGridCell;				/* Number of cells in -g grids (-Yg).        */
	int nGlyphs;				/* Settings for what gylphs to use (-YXG).    */
	BOOL fEcliptic;
	double rRot;        /* Current rotation degree of globe.         */
	int nDecaType;    /* Type of wheel chart decoration (-YXv).    */
	int nDecaSize;    /* Size of wheel chart decoration (-YXv).    */
	int nDecaLine;    /* Lines in wheel chart decoration (-YXv).   */
	double rspace;      /* Radius in AU of -S orbit chart (-YXS).    */
	int cspace;       /* Number of -S orbit trails allowed (-YXj). */
	int zspace;       /* Height diff of each orbit trail (-YXj0).  */
	int nRayWidth;    /* Column width in -7 esoteric chart (-YX7). */
	int fColorSign;  /* More color for sign boundaries. (-YXk).   */
	int fColorHouse; /* More color for house boundaries. (-YXk0). */
	int nDashMax;     /* Maximum dash allowed for lines (-YXA).    */
	char *szStarsLin; /* Names of extra stars for linking (-YXU).  */
	char *szStarsLnk; /* Indexes of star pairs to link up (-YXU).  */
	int fSouth;      /* Are we focus on south hemisphere (-XX0/XP0 set). */
	int fAlt;        /* Are we drawing in alternate mode (-Xi set).      */
	int fHouseExtra; /* Are we showing additional house info (-XC set).  */
	int fEquator;     /* -sr */
	int charSet;					/* character set for TTF fonts */
	int nDecaFill;     // Fill method for wheel chart sections (-Xv).
	BOOL fLabelAsp;
	BOOL fAllStar;
	int nFontHou;
	int rBackPct;
	BOOL fIndianWheel;
}
GS;

typedef struct _GraphicsInternal
{
	int nCurrChart;				/* Current type of chart to create.           */
	BOOL fMono;					/* Is this a monochrome monitor.              */
	int kiCur;					/* Current color drawing with.                */
	pbyte bm;					/* Pointer to allocated memory.               */
	int cbBmpRow;				/* Horizontal size of bitmap array in memory. */
	wchar_t *szFileOut;			/* Current name of bitmap file (-Xo).         */
	FILE *file;					/* Actual file handle writing graphics to.    */
	int yBand;					/* Vertical offset to current bitmap band.    */
	double rAsc;					/* Degree to be at left edge in wheel charts. */
	BOOL fFile;					/* Are we making a graphics file.             */
	int nScale;					/* Scale ratio, i.e. percentage / 100.        */
	int nScaleT;				/* Relative scale to draw chart text at.      */
	int nPenWid;				/* Pen width to use when creating metafiles.  */
	KI kiOn;					/* Foreground color. */
	KI kiOff;					/* Background color. */
	KI kiLite;					/* Hilight color.    */
	KI kiGray;					/* A "dim" color.    */
	int xOffset;				/* Viewport origin.                           */
	int yOffset;
	int xTurtle;				/* Current coordinates of drawing pen.        */
	int yTurtle;
	int xPen;					/* Cached coordinates where last line ended.  */
	int yPen;
#ifdef PS						/* Variables used by the PostScript generator. */
	BOOL fEps;					/* Are we doing Encapsulated PostScript.    */
	int cStroke;				/* Number of items drawn without fluahing.  */
	BOOL fLineCap;				/* Are line ends rounded instead of square. */
	int nDash;					/* How much long are dashes in lines drawn. */
	int nFont;					/* What system font are we drawing text in. */
	double rLineWid;				/* How wide are lines, et al, drawn with.   */
#endif
#ifdef META						/* Variables used by the metafile generator.  */
	word *pwMetaCur;			/* Current mem position when making metafile. */
	word *pwPoly;				/* Position for start of current polyline.    */
	long cbMeta;
	KI kiLineAct;				/* Desired and actual line color. */
	KI kiLineDes;
	KI kiFillAct;				/* Desired and actual fill color. */
	KI kiFillDes;
	int nFontAct;				/* Desired and actual text font.  */
	int nFontDes;
	KI kiTextAct;				/* Desired and actual text color. */
	KI kiTextDes;
	int nAlignAct;				/* Desired/actual text alignment. */
	int nAlignDes;
#endif
	int nScaleText;
	int fDidSphere;      /* Has a chart sphere been drawn once yet?    */
	int nMode;
	BOOL fBmp;
	int nGridCell;
}
GI;

typedef struct _ArabicInfo
{
	wchar_t form[MAX_STRING_NAME_LEN];					/* The formula to calculate it. */
	wchar_t name[MAX_STRING_NAME_LEN];					/* The name of the Arabic part. */
}
AI;

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

typedef struct _OrbitalElements
{
	double ma0, ma1, ma2;			/* Mean anomaly.           */
	double ec0, ec1, ec2;			/* Eccentricity.           */
	double sma;					    /* Semi-major axis.        */
	double ap0, ap1, ap2;			/* Argument of perihelion. */
	double an0, an1, an2;			/* Ascending node.         */
	double in0, in1, in2;			/* Inclination.            */
}
OE;

typedef struct _WindowInternal
{
	//HINSTANCE hinst;			/* Instance of the Astrolog window class.    */
	//HWND hwndMain;				/* The outer created frame window.           */
	//HWND hwnd;					/* The current window being dealt with.      */
	//HWND hwnd2;					/* The current window being dealt with.      */
	//HMENU hmenu;				/* The Astrolog main menu bar.               */
	//HACCEL haccel;				/* Keyboard accelerator or shortcut table.   */
	//HDC hdc;					/* The current DC bring drawn upon.          */
	//HDC hdc2;					/* The current DC bring drawn upon.          */
	//HDC hdcPrint;				/* The current DC being printed upon.        */
	//HWND hwndAbort;				/* Window of the printing abort dialog.      */
	//HPEN hpen;					/* Pen with the current line color.          */
	//HBRUSH hbrush;				/* Fill if any with the current color.       */
	//HFONT hfont;				/* Font of current text size being printed.  */
	int nTimer;				/* Identifier for the animation timer.       */
	int xScroll;				/* Horizontal & vertical scrollbar position. */
	double yScroll;
	int xClient;				/* Horizontal & vertical window size.		 */
	int yClient;
	int xChar;					/* Horizontal & vertical font character size.*/
	int yChar;
	int xMouse;					/* Horitontal & vertical mouse position.	 */
	int yMouse;
	unsigned short wCmd;					/* The currently invoked menu command.        */
	int nMode;					/* New chart type to switch to if any.        */
	BOOL fMenu;					/* Do we need to repaint the menu bar?        */
	BOOL fMenuAll;				/* Do we need to redetermine all menu checks? */
	BOOL fRedraw;				/* Do we need to redraw the screen?           */
	BOOL fCast;					/* Do we need to recast the chart positions?  */
	BOOL fAbort;				/* Did the user cancel printing in progress?  */
	int nDlgChart;				/* Which chart to set in Open or Info dialog. */
	BOOL fInitProgression;		/* hack to set chart data at beginning of progression,
									to make animation possible */

									/* Window User settings. */
	BOOL fAnimate;				/* Animation runs if true */
	BOOL fDelayScreenRedraw;	/* Are we drawing updates off screen?     */
	BOOL fHourglass;			/* Bring up hourglass cursor on redraws?  */
	BOOL fChartWindow;			/* Does chart change cause window resize? */
	BOOL fWindowChart;			/* Does window resize cause chart change? */
	BOOL fNoUpdate;				/* Do we not automatically update screen? */
	KI kiPen;					/* The current pen scribble color.        */
	int nAnimationJumpFactor;	/* Time increases if positive, decreases if negative  */
	int nTimerDelay;			/* Milliseconds between animation draws.  */
	int xPrint;					/* Hor. and ver. resolution for printing  */
	int yPrint;
	BOOL fPrinting;				/* TRUE if we are printing				  */
	bool fDisableMacroShortcuts; /*true if menu shortcuts for macros should be hidden on update*/
	//HFONT h;
	//HFONT m_hFont;
	//HFONT m_hFont2;
	unsigned short lid;
	BOOL chs;
	//HFONT m_hFontv;
	long lParamRC; /* Coordinates where right click originated.  */
	BOOL fMoved;
	BOOL fSaverRun;
}
WI;

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

// operations on a chart stack
typedef enum
{
	CHART_CREATE,
	CHART_DESTROY,
	CHART_PREVIOUS,
	CHART_NEXT,
} chart_ops_t;

typedef struct _chart_unit
{
	CI					user_chart;	// must always be the first element in structure
	struct _chart_unit	*previous;
	struct _chart_unit	*next;
	int					magic_number;
} chart_unit_t;

typedef enum
{
	CHART_HEAP_SEARCH,
	CHART_HEAP_SORTED,
	NO_CHART_HEAPS,
} chart_heap_types;

/*
******************************************************************************
** Astrolog Include Files.
******************************************************************************
*/

#include "version.h"

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
		0, 0, 0, 0, 0, 0, 0, 0, 0, TRUE, TRUE,

		/* Main subflags */
		FALSE, 0, 0, 0, 0, 0,

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

		4, 5, cPart, 0.0, 365.24219, 1, 1, 24, 0, BIODAYS,0,FALSE,0
};

IS is = {
	FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
	NULL, NULL, NULL, NULL, 0, 0, 0, 0.0, 0.0, 0.0,
	NULL, 0.0, 0.0, 0.0, 0.0, 0.0
};

// chart info
// month == -1 means "no time or space"

// Longitude and latitude in ciCore may be in degrees or radians, depending on location!
// In CastChart ciCore value is saved and later restored, but in the mean time, in 
// ProcessInput value is changed to radians! Mad!
CI ciCore = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"" };
CI ciMain = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciTwin = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciThre = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciFour = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciTran = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciSave = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };

CI ciSave2 = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciSave3 = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciSolarReturn = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciLunarReturn0 = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciLunarReturn = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciBak = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciBak2 = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciPDs = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciNatal = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciNatal2 = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciHereAndNow = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciSyzygy = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciFound = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
CI ciAspos = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
// planet positions
CP cp0, cp1, cp2, cp3, cp4, cpPDs, cpBak, cpIng, cpSave;
BOOL UseFirstReturn = FALSE;
BOOL IsDoubleReturn = FALSE; // 一个月中有两次月亮返照
int fCP3 = 0; // this variable appears to be related to the usage of cp3, but how?

double INVALID = 4.9E-324;
double julian_day_ut, julian_day;
bool setJulianDay(CI ci);
/*
******************************************************************************
** Global Arrays.
******************************************************************************
*/

double spacex[cLastMoving + 1], spacey[cLastMoving + 1];
double spacez[cLastMoving + 1], force[NUMBER_OBJECTS];
GridInfo *grid = NULL;
int starname[cStar + 1], kObjA[NUMBER_OBJECTS];
char *szMacro[NUMBER_OF_MACROS] = { 0 };
wchar_t szMacroCaptionW[NUMBER_OF_MACROS][256] = { 0 };
char szMacroCaption[NUMBER_OF_MACROS][256] = { 0 };
char szMacroMenuType[NUMBER_OF_MACROS][256] = { 0 };

int kObjA1[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		/* Planets  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* Minors   */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* Cusps    */
	0, 0, 0, 0, 0, 0, 0, 0, 0			/* Uranians */
};

/* Restriction status of each object, as specified with -YR switch. */
byte ignore1[NUMBER_OBJECTS] = { 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					/* Planets 10 */
1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 				/* Minors  21 */
0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,				/* Cusps   32 */
1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians 42*/
1,1,0,0,0,1,1,1,1,0,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,
0,1,1,1,1,1,1,0,1,1,1,1,
0,1,0,0,1,1,1,1,1,1,1,1,
0,1,0,1,1,1,1,1,0,1,1,1,
1,0,1,1,1,1,1,1,1,1,1,0,
1,1,0
};

byte ignoreT4[NUMBER_OBJECTS] = { 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					/* Planets 10 */
	1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 				/* Minors  21 */
	0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,				/* Cusps   32 */
	1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians 42*/
1,1,0,0,0,1,1,1,1,0,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,
0,1,1,1,1,1,1,0,1,1,1,1,
0,1,0,0,1,1,1,1,1,1,1,1,
0,1,0,1,1,1,1,1,0,1,1,1,
1,0,1,1,1,1,1,1,1,1,1,0,
1,1,0
};

/* Restriction of objects when transiting, as specified with -YRT switch. */
byte ignore2[NUMBER_OBJECTS] = { 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					/* Planets  */
	1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 				/* Minors   */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,				/* Cusps    */
	1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Stars    */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1
};

byte ignoreT2[NUMBER_OBJECTS] = { 1,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1,					/* Planets  */
	1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 				/* Minors   */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,				/* Cusps    */
	1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Stars    */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1
};

/* Restriction of objects when progressing, as specified with -YRP switch.  */
/* For comparison charts only, else normal ignore1 used.						*/

byte ignore3[NUMBER_OBJECTS] = { 1,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1,					/* Planets  */
	1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 				/* Minors   */
	0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,				/* Cusps    */
	1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Stars    */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1
};

byte ignoreT1[NUMBER_OBJECTS] = { 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					/* Planets 10 */
1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 				/* Minors  21 */
0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,				/* Cusps   32 */
1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians 42*/
1,1,0,0,0,1,1,1,1,0,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,
0,1,1,1,1,1,1,0,1,1,1,1,
0,1,0,0,1,1,1,1,1,1,1,1,
0,1,0,1,1,1,1,1,0,1,1,1,
1,0,1,1,1,1,1,1,1,1,1,0,
1,1,0
};

byte ignoreT3[NUMBER_OBJECTS] = { 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1,					/* Planets  */
	1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 				/* Minors   */
	0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,				/* Cusps    */
	1, 1, 1, 1, 1, 1, 1, 1, 1,						/* Uranians */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* Stars    */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1
};

/* Restriction of aspects as specified with -YRA switch. */
byte ignoreA[cAspect + 1] = { 0,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

byte ignoreA2[cAspect + 1] = { 0,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

byte ignorez[4] = { 0, 0, 0, 0 };	/* Restrictions for -Zd chart events. */

/* Gauquelin sector plus zones, as specified with -Yl switch. */

byte pluszone[cSector + 1] = { 0,
	1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
	1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1
};


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

double rAspAngle[cAspect + 1] = { 0,
	0.0, 180.0, 90.0, 120.0, 60.0, 150.0, 30.0, 45.0, 135.0, 72.0, 144.0,
	36.0, 360.0 / 7.0, 40.0, 80.0, 720.0 / 7.0, 1080.0 / 7.0, 160.0
};

double rAspAngle2[cAspect + 1] = { 0,
	0.0, 180.0, 90.0, 120.0, 60.0, 150.0, 30.0, 45.0, 135.0, 72.0, 144.0,
	36.0, 360.0 / 7.0, 40.0, 80.0, 720.0 / 7.0, 1080.0 / 7.0, 160.0
};

double rAspOrb[cAspect + 1] = { 0, 7.0, 7.0, 7.0, 7.0, 6.0, 3.0, 3.0, 1.5, 1.5, 1.0, 1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
double rAspOrb2[cAspect + 1] = { 0, 7.0, 7.0, 7.0, 7.0, 6.0, 3.0, 3.0, 1.5, 1.5, 1.0, 1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };

double rObjOrb[cLastMoving + 1] = { 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0
};

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

double rObjOrb2[cLastMoving + 1] = { 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
	360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0
};

double rObjAdd[cLastMoving + 1] = { 0.0,
	1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

double rObjAdd2[cLastMoving + 1] = { 0.0,
	1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

char ruler1[cLastMoving + 1] = { 0, 5,  4, 3, 2, 1,  9,10,11, 12, 8, 12, 2, 6, 7, 8, 11, 5, 8, 12, 7, 1, 1,  2, 3, 4, 5,  6, 7, 8, 9, 10, 11, 12, 7,  8, 5,10, 9,  6, 1, 6, 9 };
char ruler2[cLastMoving + 1] = { 0, 0,  0, 6, 7, 8, 12,11, 0, 0,  0, 0,  0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0 };
char exalt[cLastMoving + 1] = { 0, 1,  2, 11,12,10, 4, 7, 8, 9,  6, 4,  4, 10,5, 11,6, 12, 12,9, 12, 10,5,  6, 7, 8, 9, 10, 11,12,1, 2,  3,  4,  3,  6, 1, 9, 11, 8, 5, 3, 12 };

/* This array is the reverse of the ruler arrays:   */
/* Here, given a sign, return what planet rules it. */
char rules[NUMBER_OF_SIGNS + 1] = { 0, 5, 4, 3, 2, 1, 3, 4, 5,  6, 7, 7, 6 };
int rulesA[12 + 1] = { -1,
  oMar, oVen, oMer, oMoo, oSun, oMer, oVen, oPlu, oJup, oSat, oUra, oNep };
int rules2A[12 + 1] = { -1,
  -1, -1, -1, -1, -1, -1, -1, oMar, -1, -1, oSat, oJup };

// colour defaults

int kStar1 = kRed;
int kStar2 = kOrange;
int kStar3 = kMaroon;
int kCornerDecoration = kDkBlue;

#ifdef INTERPRETALT
byte NoPrint = 0;
byte InterpretAlt = 0;
#endif

byte NoPrintHeader = 0;

/* Influence information used by ChartInfluence() follows. The influence of */
/* a cp0.longitude in its ruling or exalting sign or house is tacked onto the last */
/* two positions of the object and house influence array, respectively.     */


  /* The inherent strength of each cp0.longitude - */
double rObjInf[cLastMoving + 3] = { 20,
	30, 25, 10, 10, 10, 10, 10, 10, 10, 10,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	20, 10, 10, 10, 10, 10, 10, 10, 10, 15, 10, 10,
	3, 3, 3, 3, 3, 3, 3, 3, 3,
	20, 10
};


double rObjInfA[90] = {
30,30,25,10,10,10,10,10,8,8,8,6,5,5,5,5,5,5,4,4,4,4,
20,10,10,10,10,10,10,10,10,15,10,10,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,20,10,10,
10,10
};

double rObjInf2[cLastMoving + 3] = { 20,
	30, 25, 10, 10, 10, 10, 10, 10, 10, 10,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	20, 10, 10, 10, 10, 10, 10, 10, 10, 15, 10, 10,
	3, 3, 3, 3, 3, 3, 3, 3, 3,
	20, 10
};

double ppower1[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

double ppower2[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

double ppowerTotal[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* The inherent strength of each house - */
double rHouseInf[NUMBER_OF_SIGNS + 3] = { 0,
	20, 0, 0, 10, 0, 0, 5, 0, 0, 15, 0, 0,
	15, 5
};

double rHouseInfA[12 + 6] = {
	0,
  20, 0, 0, 10, 0, 0, 5, 0, 0, 15, 0, 0,
  15, 5, 5, 5, 5 };

/* The inherent strength of each aspect - */
double rAspInf[cAspect + 1] = { 0.0,
	1.0, 0.8, 0.8, 0.6, 0.6, 0.4, 0.4, 0.2, 0.2,
	0.2, 0.2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
};

//double rAspInfA[25] = {
//	0.0,
//  1.0, 0.8, 0.8, 0.6, 0.6, 0.4, 0.4, 0.2, 0.2,
//  0.2, 0.2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
//  0.05, 0.05, 0.05, 0.05, 0.05, 0.05};

double rAspInfA[25] = { 0.0,
1.0,0.8,0.8,0.6,0.6,0.4,0.4,0.2,0.2,
0.2,0.2,0.1,0.1,0.1,0.1,0.1,0.1,0.1,
0.05,0.05,0.05,0.05,0.05,0.05 };

double rAspInf2[cAspect + 1] = { 0.0,
	1.0, 0.8, 0.8, 0.6, 0.6, 0.4, 0.4, 0.2, 0.2,
	0.2, 0.2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
};

/* The inherent strength of each cp0.longitude when transiting - */
double rTransitInf[cLastMoving + 3] = { 10,
	10, 4, 8, 9, 20, 30, 35, 40, 45, 50,
	30, 15, 15, 15, 15, 30, 30, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	50, 50, 50, 50, 50, 50, 50, 50, 50
};

double rTransitInf2[cLastMoving + 3] = { 10,
	10, 4, 8, 9, 20, 30, 35, 40, 45, 50,
	30, 15, 15, 15, 15, 30, 30, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	50, 50, 50, 50, 50, 50, 50, 50, 50
};

const double rObjDist[oVesta + 1] = { 149.59787, 0.0, 0.3844,
	57.91, 108.2, 227.94, 778.33, 1426.98, 2870.99, 4497.07, 5913.52,
	13.670 * 149.59787, 2.767 * 149.59787, 2.770 * 149.59787,
	2.669 * 149.59787,
	2.361 * 149.59787
};
const double rObjYear[oVesta + 1] = { 1, 0, 27.32166 / 365.25,
	87.969 / 365.25, 224.701 / 365.25, 686.98 / 365.25, 11.8623,
	29.458, 84.01, 164.79, 248.54,
	51.0, 4.60, 4.61, 4.36, 3.63
};
const double rObjDiam[oVesta + 1] = { 12756.0, 1392000.0, 3476.0,
	4878.0, 12102.0, 6786.0, 142984.0, 120536.0, 51118.0, 49528.0, 2300.0,
	320.0, 955.0, 538.0, 226.0, 503.0
};
const double rObjDay[oPlu + 1] = { 24.0, 30.0 * 24.0, 27.322 * 24.0,
	58.65 * 24.0, 243.01 * 24.0, 24.6229, 9.841, 10.233, 17.9, 19.2,
	6.3872 * 24.0
};
const double rObjMass[oPlu + 1] = { 1.0, 322946.0, 0.0123,
	0.0553, 0.8149, 0.1074, 317.938, 95.181, 14.531, 17.135, 0.0022
};
const double rObjAxis[oPlu + 1] = { 23.5, 0.0, 6.7,
	2.0, 2.7, 25.19, 3.12, 26.73, 82.14, 29.6, 57.54
};
const byte cSatellite[oPlu + 1] = { 1, 9, 0,
	0, 0, 2, 16, 18, 15, 8, 1
};


const char *szStarConName[cStar + 1] = { "",
	"alEri\0   ", "alUMi\0   ", "zeRet\0   ", "etTau\0   ", "alTau\0   ","alAur\0   ",
	"beOri\0   ", "gaOri\0   ", "beTau\0   ", "epOri\0   ", "alOri\0   ","beAur\0   ",
	"beCMa\0   ", "alCar\0   ", "gaGem\0   ", "alCMa\0   ", "epCMa\0   ","deCMa\0   ",
	"alGem\0   ", "alCMi\0   ", "beGem\0   ", "ga-2Vel\0 ", "epCar\0   ","beCar\0   ",
	"alHya\0   ", "alLeo\0   ", "alUMa\0   ", "al-1Cru\0 ", "gaCru\0   ","beCru\0   ",
	"epUMa\0   ", "alVir\0   ", "etUMa\0   ", "beCen\0   ", "alBoo\0   ","alCen\0   ",
	"alSco\0   ", "laSco\0   ", "thSco\0   ", "epSgr\0   ", "alLyr\0   ","alAql\0   ",
	"alPav\0   ", "alCyg\0   ", "alGru\0   ", "alPsA\0   ", "M31\0     ","alAnd\0   ",
	"gaPeg\0   ", "alCas\0   ", "beAnd\0   ", "alPis\0   ", "ga-1And\0 ","bePer\0   ",
	"deOri\0   ", "deGem\0   ", "alCnc\0   ", "beUMa\0   ", "epVir\0   ","zeUMa\0   ",
	"beUMi\0   ", "al-2Lib\0 ", "beLib\0   ", "alCrB\0   ", "alSer\0   ","alOph\0   ",
	"be-1Cyg\0 ", "alCep\0   ", "gaCap\0   ", "deAqr\0   ", "bePeg\0   ","alPeg     ",
	"SgrA*\0   ", "HerA*\0   ", "alAqr\0   "
};

int NParseCommandLine(char *szLine, char **argv);
int NParseCommandLineW(wchar_t *szLine, wchar_t **argv);

double StarOrb = 1.2;
double PartOrb = 1.0;
double CoeffComp = 2.5;
double CoeffPar = 6.0;
double CoeffTens = 1.0;
double PowerPar = 0.25;
double SolidAspect = 8.0;
double SConjunct = 0.0;
double SHarmon = 0.0;
double STension = 0.0;
double SSumma = 0.0;
double SChart = 0.0;
double PrimeOrb1 = 1.16666667;
double PrimeOrb2 = 1.16666667;
int StarAspects = 1;
int PartAspects = 1;
int PartAspectsShow = 5;
int TensChart = 0;
int hRevers = 0;
int InfoFlag = 0;
int PrimeRest = 0;
int MonthFormat = FALSE;
byte StarRest = 0;
byte PolarMCflip = 0;
byte fEquator = 0;
byte oscLilith = 0;
byte fNESW = 0;
BOOL fSortAspectsByOrbs = 0; // if false, sorting is done by power
byte fDisp = FALSE;

int CornerDecorationType = 0; // 0 = none, 1 = moire, 2 = spider
int CornerDecorationSize = 60;
int CornerDecorationNumberLines = 80;	// used only for spider web

CI *LastChartShown = NULL;

// This must be in English, do not translate!!
const char *szMonthEnglish[NUMBER_OF_MONTHS + 1] = { "",
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
};

const char* szSignAbbrevEnglish[NUMBER_OF_SIGNS + 1] = { "",
	"Ari", "Tau", "Gem", "Can", "Leo", "Vir", "Lib", "Sco",
	"Sag","Cap", "Aqu", "Pis"
};

const char *szObjShortNameEnglish[NUMBER_OBJECTS] = {
	"Earth", "Sun ", "Moon", "Mercury", "Venus", "Mars",	/* Planets   */
	"Jupiter", "Saturn", "Uranus", "Neptune", "Pluto",
	"Chiron", "Ceres", "Pallas", "Juno", "Vesta",	/* Asteroids */
	"NoNode", "SoNode", "Lilith", "Fortune", "Vertex", "EaPoint",	/* Others    */
	// the extra space after IC is necessary for house wheels in text
	// mode to be correctly displayed
	"Ascendant", "2nd Cusp", "3rd Cusp", "IC ",	/* Cusps     */
	"5th Cusp", "6th Cusp", "Descendant", "8th Cusp",
	"9th Cusp", "Midheaven", "11th Cusp", "12th Cusp",
	"Cupido", "Hades", "Zeus", "Kronos",	/* Uranians  */
	"Apollon", "Admetos", "Vulkanus", "Poseidon", "Proserpina",

	// Stars 

	// Star names should not be translated, because they may be overwritten
	// by names from the fixed stars catalogue, that are in English.

	"Achernar\0    ", "Polaris\0     ", "Zeta Retic.\0 ", "Alcyone\0     ",
	"Aldebaran\0   ", "Capella\0     ", "Rigel\0       ", "Bellatrix\0   ",
	"Elnath\0      ", "Alnilam\0     ", "Betelgeuse\0  ", "Menkalinan\0  ",
	"Mirzam\0      ", "Canopus\0     ", "Alhena\0      ", "Sirius\0      ",
	"Adhara\0      ", "Wezen\0       ", "Castor\0      ", "Procyon\0     ",
	"Pollux\0      ", "Suhail\0      ", "Avior\0       ", "Miaplacidus\0 ",
	"Alphard\0     ", "Regulus\0     ", "Dubhe\0       ", "Acrux\0       ",
	"Gacrux\0      ", "Mimosa\0      ", "Alioth\0      ", "Spica\0       ",
	"Alkaid\0      ", "Agena\0       ", "Arcturus\0    ", "Toliman\0     ",
	"Antares\0     ", "Shaula\0      ", "Sargas\0      ", "Kaus Austr.\0 ",
	"Vega\0        ", "Altair\0      ", "Peacock\0     ", "Deneb\0       ",
	"Alnair\0      ", "Fomalhaut\0   ", "Andromeda\0   ", "Alpheratz\0   ",
	"Algenib\0     ", "Schedar\0     ", "Mirach\0      ", "Alrischa\0    ",
	"Almac\0       ", "Algol\0       ", "Mintaka\0     ", "Wasat\0       ",
	"Acubens\0     ", "Merak\0       ", "Vindemiatrix\0", "Mizar\0       ",
	"Kochab\0      ", "Zuben Elgen.\0", "Zuben Escha.\0", "Alphecca\0    ",
	"Unuk Alhai\0  ", "Ras Alhague\0 ", "Albireo\0     ", "Alderamin\0   ",
	"Nashira\0     ", "Skat\0        ", "Scheat\0      ", "Markab\0      ",
	"Gal.Center\0  ", "Apex\0        ", "Sadalmelik\0  "
};

const char *szSwitchName[] =
{
	"Glyph",
	"CharSet",
	"DegCharacter",
	"UseInternationalAtlas",
	"OmitWizard",
	"OmitRegionalWizard",
	"DebugFont",
	"SimplifiedMode",
	"AdvancedMode",
	"DisableMacroShortcuts",
	"DisableSave",
	"EnableSave",
	""
};

/*
******************************************************************************
** Object Calculation Tables.
******************************************************************************
*/

const byte rErrorCount[oPlu - oJup + 1] = { 11, 5, 4, 4, 4 };
const byte rErrorOffset[oPlu - oJup + 1] = { 0, 72, 72 + 51, 72 + 51 + 42, 72 + 51 + 42 + 42 };

const double  rErrorData[72 + 51 + 42 * 3] = {
	-.001, -.0005, .0045, .0051, 581.7, -9.7, -.0005, 2510.7, -12.5, -.0026,
	1313.7, -61.4,
	0.0013, 2370.79, -24.6, -.0013, 3599.3, 37.7, -.001, 2574.7, 31.4,
	-.00096, 6708.2,
	-114.5, -.0006, 5499.4, -74.97, -.0013, 1419, 54.2, .0006, 6339.3, -109,
	.0007, 4824.5,
	-50.9, .0020, -.0134, .0127, -.0023, 676.2, .9, .00045, 2361.4, 174.9,
	.0015, 1427.5,
	-188.8, .0006, 2110.1, 153.6, .0014, 3606.8, -57.7, -.0017, 2540.2, 121.7,
	-.00099,
	6704.8, -22.3, -.0006, 5480.2, 24.5, .00096, 1651.3, -118.3, .0006,
	6310.8, -4.8, .0007,
	4826.6, 36.2,				/* Jupiter error */

	-.0009, .0037, 0, .0134, 1238.9, -16.4, -.00426, 3040.9, -25.2, .0064,
	1835.3, 36.1,
	-.0153, 610.8, -44.2, -.0015, 2480.5, -69.4, -.0014, .0026, 0, .0111,
	1242.2, 78.3,
	-.0045, 3034.96, 62.8, -.0066, 1829.2, -51.5, -.0078, 640.6, 24.2, -.0016,
	2363.4,
	-141.4, .0006, -.0002, 0, -.0005, 1251.1, 43.7, .0005, 622.8, 13.7, .0003,
	1824.7, -71.1,
	.0001, 2997.1, 78.2,		/* Saturn error */

	-.0021, -.0159, 0, .0299, 422.3, -17.7, -.0049, 3035.1, -31.3, -.0038,
	945.3, 60.1,
	-.0023, 1227, -4.99, .0134, -.02186, 0, .0317, 404.3, 81.9, -.00495,
	3037.9, 57.3, .004,
	993.5, -54.4, -.0018, 1249.4, 79.2, -.0003, .0005, 0, .0005, 352.5,
	-54.99, .0001, 3027.5,
	54.2, -.0001, 1150.3, -88,	/* Uranus error */

	0.1832, -.6718, .2726, -.1923, 175.7, 31.8, .0122, 542.1, 189.6, .0027,
	1219.4, 178.1,
	-.00496, 3035.6, -31.3, -.1122, .166, -.0544, -.00496, 3035.3, 58.7,
	.0961, 177.1, -68.8,
	-.0073, 630.9, 51, -.0025, 1236.6, 78, .00196, -.0119, .0111, .0001,
	3049.3, 44.2, -.0002,
	893.9, 48.5, .00007, 1416.5, -25.2,	/* Neptune error */

	-.0426, .073, -.029, .0371, 372, -331.3, -.0049, 3049.6, -39.2, -.0108,
	566.2, 318.3,
	0.0003, 1746.5, -238.3, -.0603, .5002, -.6126, .049, 273.97, 89.97,
	-.0049, 3030.6, 61.3,
	0.0027, 1075.3, -28.1, -.0007, 1402.3, 20.3, .0145, -.0928, .1195, .0117,
	302.6, -77.3,
	0.00198, 528.1, 48.6, -.0002, 1000.4, -46.1	/* Pluto error */
};

OE rgoe[oVesta - 1 + cUran] = {
	{358.4758, 35999.0498, -.0002, .01675, -.4E-4, 0, 1, 101.2208, 1.7192,
	 .00045, 0, 0, 0, 0, 0, 0}
	,							/* Earth/Sun */
	{102.2794, 149472.515, 0, .205614, .2E-4, 0, .3871, 28.7538, .3703, .0001,
	 47.1459, 1.1852, 0.0002, 7.009, .00186, 0}
	,							/* Mercury */
	{212.6032, 58517.8039, .0013, .00682, -.5E-4, 0, .7233, 54.3842, .5082,
	 -.14E-2, 75.7796, 0.8999, .4E-3, 3.3936, .1E-2, 0}
	,							/* Venus */
	{319.5294, 19139.8585, .2E-3, .09331, .9E-4, 0, 1.5237, 285.4318, 1.0698,
	 .1E-3, 48.7864, 0.77099, 0, 1.8503, -.7E-3, 0}
	,							/* Mars */
	{225.4928, 3033.6879, 0, .04838, -.2E-4, 0, 5.2029, 273.393, 1.3383, 0,
	 99.4198, 1.0583, 0, 1.3097, -.52E-2, 0}
	,							/* Jupiter */
	{174.2153, 1223.50796, 0, .05423, -.2E-3, 0, 9.5525, 338.9117, -.3167, 0,
	 112.8261, .8259, 0, 2.4908, -.0047, 0}
	,							/* Saturn */
	{74.1757, 427.2742, 0, .04682, .00042, 0, 19.2215, 95.6863, 2.0508, 0,
	 73.5222, .5242, 0, 0.7726, .1E-3, 0}
	,							/* Uranus */
	{30.13294, 240.45516, 0, .00913, -.00127, 0, 30.11375, 284.1683, -21.6329,
	 0, 130.68415,
	 1.1005, 0, 1.7794, -.0098, 0}
	,							/* Neptune */
	{229.781, 145.1781, 0, .24797, .002898, 0, 39.539, 113.5366, .2086, 0,
	 108.944, 1.3739, 0, 17.1514, -.0161, 0}
	,							/* Pluto */
	{34.6127752, 713.5756219, 0, .382270369, -.004694073, 0, 13.66975144,
	 337.407213, 2.163306646, 0, 208.1482658, 1.247724355, 0, 6.911179715, .011236955, 0}
	,							/* Chiron */
	{108.2925, 7820.36556, 0, .0794314, 0, 0, 2.7672273, 71.0794444, 0, 0,
	 80.23555556, 1.3960111, 0, 10.59694444, 0, 0}
	,							/* Ceres */
	{106.6641667, 7806.531667, 0, .2347096, 0, 0, 2.7704955, 310.166111, 0, 0,
	 172.497222, 1.39601111, 0, 34.81416667, 0, 0}
	,							/* Pallas Athena */
	{267.685, 8256.081111, 0, .2562318, 0, 0, 2.6689897, 245.3752778, 0, 0,
	 170.137777, 1.396011111, .0003083333, 13.01694444, 0, 0}
	,							/* Juno */
	{138.7733333, 9924.931111, 0, .0902807, 0, 0, 2.360723, 149.6386111, 0, 0,
	 103.2197222, 1.396011111, .000308333, 7.139444444, 0, 0}
	,							/* Vesta */
	{104.5959, 138.5369, 0, 0, 0, 0, 40.99837, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Cupido   */
	{337.4517, 101.2176, 0, 0, 0, 0, 50.667443, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Hades    */
	{104.0904, 80.4057, 0, 0, 0, 0, 59.214362, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Zeus     */
	{17.7346, 70.3863, 0, 0, 0, 0, 64.816896, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Kronos   */
	{138.0354, 62.5, 0, 0, 0, 0, 70.361652, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Apollon  */
	{-8.678, 58.3468, 0, 0, 0, 0, 73.736476, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Admetos  */
	{55.9826, 54.2986, 0, 0, 0, 0, 77.445895, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Vulkanus */
	{165.3595, 48.6486, 0, 0, 0, 0, 83.493733, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	,							/* Poseidon */
	{170.73, 51.05, 0, 0, 0, 0, 79.22663, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	/* Proserpina */
};

double  rStarBright[cStar + 1] = { 0,
	0.46, 2.02, 5.24, 5.09, 0.85, 0.08, 0.12, 1.64, 1.65, 1.70,
	0.50, 1.90, 1.98, -0.72, 1.93, -1.46, 1.50, 1.86, 1.58, 0.38,
	1.14, 1.82, 1.86, 1.68, 1.98, 1.35, 1.79, 1.58, 1.63, 1.25,
	1.77, 0.97, 1.86, 0.61, -0.04, -0.01, 0.96, 1.63, 1.87, 1.85,
	0.03, 0.77, 1.94, 1.25, 1.74, 1.16, 4.61,
	2.06, 2.83, 2.23, 2.29, 4.33, 2.25, 2.12, 2.23, 3.53, 4.25,
	2.37, 2.83, 2.27, 2.08, 2.75, 2.61, 2.23, 2.65, 2.08, 3.24,
	2.44, 3.68, 3.27, 2.42, 2.49, 9.99, 9.99, 2.96
};

const double  rStarData[cStar * 6] = {
	1, 37, 42.9, -57, 14, 12, 2, 31, 50.5, 89, 15, 51, 3, 17, 46.1, -62, 34,32,
	3, 49, 11.1, 24, 8, 12, 4, 35, 55.2, 16, 30, 33, 5, 16, 41.4, 45, 59, 53,
	5, 14, 32.3, -8, 12, 6, 5, 25, 7.9, 6, 20, 59, 5, 26, 17.5, 28, 36, 27,
	5, 36, 12.8, -1, 12, 7, 5, 55, 10.3, 7, 24, 25, 5, 59, 31.7, 44, 56, 51,
	6, 22, 42.0, -17, 57, 21, 6, 23, 57.1, -52, 41, 45, 6, 37, 42.7, 16, 23,57,
	6, 45, 8.9, -16, 42, 58, 6, 58, 37.6, -28, 58, 20, 7, 8, 23.5, -26, 23,36,
	7, 34, 36.0, 31, 53, 19, 7, 39, 18.1, 5, 13, 30, 7, 45, 18.9, 28, 1, 34,
	8, 9, 32.0, -47, 20, 12, 8, 22, 30.8, -59, 30, 35, 9, 13, 12.0, -69, 43,2,
	9, 27, 35.2, -8, 39, 31, 10, 8, 22.3, 11, 58, 2, 11, 3, 43.7, 61, 45, 3,
	12, 26, 35.9, -63, 5, 57, 12, 31, 9.9, -57, 6, 48, 12, 47, 43.2, -59, 41,19,
	12, 54, 1.7, 55, 57, 35, 13, 25, 11.6, -11, 9, 41, 13, 47, 32.4, 49, 18,48,
	14, 3, 49.4, -60, 22, 23, 14, 15, 39.7, 19, 10, 57, 14, 39, 35.9, -60, 50,7,
	16, 29, 24.4, -26, 25, 55, 17, 33, 36.5, -37, 6, 14, 17, 37, 19.2, -42,59, 52,
	18, 24, 10.3, -34, 23, 5, 18, 36, 56.3, 38, 47, 1, 19, 50, 47.0, 8, 52, 6,
	20, 25, 38.9, -56, 44, 6, 20, 41, 25.9, 45, 16, 49, 22, 8, 14.0, -46, 57,40,
	22, 57, 39.0, -29, 37, 20, 0, 42, 7.0, 41, 16, 0, 0, 8, 23.3, 29, 05, 26,
	0, 13, 14.0, 15, 11, 1, 0, 40, 30.4, 56, 32, 15, 1, 9, 43.9, 35, 37, 14,
	2, 2, 2.7, 2, 45, 51, 2, 3, 54.0, 42, 19, 47, 3, 8, 10.1, 40, 57, 20,
	5, 32, 0.4, -0, -17, 57, 7, 20, 7.4, 21, 58, 56, 8, 58, 29.2, 11, 51, 28,
	11, 1, 50.5, 56, 22, 57, 13, 2, 10.6, 10, 57, 33, 13, 23, 55.5, 54, 55,32,
	14, 50, 42.3, 74, 9, 20, 14, 50, 52.7, -16, 2, 30, 15, 17, 0.4, -9, 22,59,
	15, 34, 41.3, 26, 42, 53, 15, 44, 16.0, 6, 25, 32, 17, 34, 56.0, 12, 33,36,
	19, 30, 43.3, 27, 57, 35, 21, 18, 34.8, 62, 35, 8, 21, 40, 5.5, -16, 39,44,
	22, 54, 39.0, -15, 49, 15, 23, 3, 46.5, 28, 4, 58, 23, 4, 45.7, 15, 12,19,
	17, 45, 37.42, -29, 0, 21, 18, 4, 0.0,  30, 0,  0, 22, 5, 47.0, -0,19, 11.5
};

GS gs = {
	FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, TRUE, FALSE, FALSE,
	TRUE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE,
	DEFAULTX, DEFAULTY, ANIMATION_FACTOR_HOURS, 200, 0, 0, 0, 0.0,
	BITMAPMODE, 1, 14.0, 11.0, NULL, oCore, 1111,
	0 // ANSI_CHARSET
};

GI gi = {
	0, FALSE, -1,
	NULL, 0, NULL, NULL, 0, 0.0, FALSE,
	2, 1, 10, kWhite, kBlack, kLtGray, kDkGray, 0, 0, 0, 0, -1, -1
#ifdef PS
		, FALSE, 0, FALSE, 0, 0, 1.0
#endif
#ifdef META
		, NULL, NULL, MAXMETA, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
#endif
		,1,FALSE
};

WI wi = {
	/*NULL, (HWND)NULL, (HWND)NULL, (HWND)NULL, (HMENU)NULL, (HACCEL)NULL,
	hdcNil, hdcNil,hdcNil, (HWND)NULL, (HPEN)NULL, (HBRUSH)NULL, (HFONT)NULL,*/
	0, 0, 0, 0, 0, 0, 0, -1, -1,
	0, 0, FALSE, TRUE, FALSE, TRUE, FALSE, 1, FALSE,
	FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, kLtGray, 1, 1000, 0, 0, FALSE,
	false, /*fDisableMacroShortcuts*/
	/*NULL,//HFONT h;
	NULL,//HFONT m_hFont;
	NULL,//HFONT m_hFont2;
	*/
	0,//LANGID lid;
	FALSE,//BOOL chs;
	/*(HFONT)NULL,*/
	0,
	FALSE,
	FALSE
};



// calculations
#define EclToEqu(Z, L)  CoorXform(Z, L, is.rObliquity)
#define EclToEqu2(Z, L) CoorXform2(Z, L, Rad2Deg(is.rObliquity) )

#define EquToEcl(Z, L) CoorXform(Z, L, -is.rObliquity)
#define EquToEcl2(Z, L) CoorXform2(Z, L, -Rad2Deg(is.rObliquity))

#define EquToLocal(Z, L, T) CoorXform(Z, L, T)
#define EquToLocal2(Z, L, T) CoorXform2(Z, L, T)
#define JulianDayFromTime(t) ((t)*36525.0+2415020.0)
#define IoeFromObj(obj) \
  (obj < oMoo ? 0 : (obj <= cLastPlanet ? obj - 2 : obj - uranLo + cLastPlanet - 1))
#define Tropical(deg) (deg - is.rSid + us.rSiderealCorrection)

#define BLo(w) ((byte)(w))
#define BHi(w) ((byte)((word)(w) >> 8 & 0xFF))
#define WLo(l) ((word)(dword)(l))
#define WHi(l) ((word)((dword)(l) >> 16 & 0xFFFF))
#define WFromBB(bLo, bHi) ((word)BLo(bLo) | (word)((byte)(bHi)) << 8)
#define LFromWW(wLo, wHi) ((dword)WLo(wLo) | (dword)((word)(wHi)) << 16)
#define LFromBB(b1, b2, b3, b4) LFromWW(WFromBB(b1, b2), WFromBB(b3, b4))
#define RGBR(l) BLo(l)
#define RGBG(l) BHi(l)
#define RGBB(l) ((byte)((dword)(l) >> 16 & 0xFF))
#define ChHex(n) (char)((n) < 10 ? '0' + (n) : 'a' + (n) - 10)
#define VgaFromEga(x) NMultDiv((x), 480, 350)
#define VgaFromCga(x) NMultDiv((x), 480, 200)
#define Max(v1, v2) ((v1) > (v2) ? (v1) : (v2))
#define Min(v1, v2) ((v1) < (v2) ? (v1) : (v2))
#define NSgn(n) ((n) < 0 ? -1 : (n) > 0)
#define RSgn2(r) ((r) < 0.0 ? -1.0 : 1.0)
#define FBetween(v, v1, v2) ((v) >= (v1) && (v) <= (v2))
#define RFract(r) ((r) - floor(r))
#define ChCap(ch) ((ch) >= 'a' && (ch) <= 'z' ? (ch) - 'a' + 'A' : (ch))
#define ChCapW(ch) ((ch) >= L'a' && (ch) <= L'z' ? (ch) - L'a' + L'A' : (ch))
#define ChUncap(ch) (FCapCh(ch) ? (ch) - 'A' + 'a' : (ch))
#define FCapCh(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define FNumCh(ch) ((ch) >= '0' && (ch) <= '9')
#define NMultDiv(n1, n2, n3) ((int)((long)(n1) * (n2) / (n3)))
#define Ratio(v1, v2, v3) ((v1) + ((v2) - (v1)) * (v3))
#define Sign2Z(s) ((double)(((s)-1)*30))
#define Z2Sign(r) (((int)(r))/30+1)
#define Deg2Rad(r) ((r)/rDegRad)
#define Rad2Deg(r) ((r)*rDegRad)
#define NFloor(r) ((int)floor(r))
#define RSinD(r) sin(Deg2Rad(r))
#define RCosD(r) cos(Deg2Rad(r))
#define NSinD(nR, nD) ((int)((double)(nR)*RSinD((double)nD)))
#define NCosD(nR, nD) ((int)((double)(nR)*RCosD((double)nD)))
#define mod12(n) ((n - 1) % 12 + 1)  // returns 1-12, n must be >= 1
#define mod36(n) ((n - 1) % 36 + 1)  // returns 1-36, n must be >= 1

// object comparison
#define FItem(obj)    FBetween(obj, 0, cObj)
#define FNorm(obj)    FBetween(obj, 0, cLastMoving)
#define FCusp(obj)    FBetween(obj, cuspLo, cuspHi)
#define FAngle(obj)   (obj == oAsc || obj == oNad  || obj == oDes  || obj == oMC)
#define FMinor(obj)   (FCusp(obj) && (obj - oAsc) % 3 != 0)
#define FUranian(obj) FBetween(obj, uranLo, uranHi)
#define FStar(obj)    FBetween(obj, starLo, starHi)
#define FObject(obj)  ((obj) <= oVesta || (obj) >= uranLo)
#define FThing(obj)   ((obj) <= cThing || (obj) >= uranLo)
#define FHelio(obj)   (FNorm(obj) && FObject(obj) && (obj) != oMoo)

#define FSector(s)    FBetween(s, 1, cSector)
#define ChDst1(dst)    (dst == 0.0 ? Lang(sST) : (dst == 1.0 ? Lang(sDT) : L"AT"))
#define ChDashF(f)    (f ? '=' : '_')
#define SzNumF(f)     (f ? "1 " : "0 ")
#define DayInYearHi(yea) (365-28+DayInMonth(2, yea))
#define FChSwitch(ch) \
  ((ch) == '-' || (ch) == '/' || (ch) == '_' || (ch) == '=' || (ch) == ':')

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
#define FValidWheel(n) FBetween(n, 1, WHEELROWS)
#define FValidAstrograph(n) (n > 0 && 160%n == 0)
#define FValidPart(n) FBetween(n, 1, cPart)
#define FValidBioday(n) FBetween(n, 1, 199)
#define FValidScreen(n) FBetween(n, 20, 200)
#define FValidMacro(n) FBetween(n, 1, NUMBER_OF_MACROS)
#define FValidTextrows(n) ((n) == 25 || (n) == 43 || (n) == 50)
#define FValidGlyphs(n) FBetween(n, 0, 2223)
#define FValidGrid(n) FBetween(n, 1, cObj)
#define FValidScale(n) (FBetween(n, 100, MAXSCALE) && (n)%100 == 0)
#define FValidGraphx(x) (FBetween(x, BITMAPX1, BITMAPX) || (x) == 0)
#define FValidGraphy(y) (FBetween(y, BITMAPY1, BITMAPY) || (y) == 0)
#define FValidRotation(n) FBetween(n, 0, 360-1)
#define FValidTilt(n) FBetween(n, -90.0, 90.0)
#define FValidColor(n) FBetween(n, 0, cColor - 1)
#define FValidBmpmode(ch) \
  ((ch) == 'N' || (ch) == 'C' || (ch) == 'V' || (ch) == 'A' || (ch) == 'B')
#define FValidTimer(n) FBetween(n, 1, 32000)
#define FValidStarAspects(n) FBetween(n, 0, 18)
#define FValidStarOrb(r) FBetween(r, 0.2, 4.0)
#define FValidCoeffTens(r) FBetween(r, 0.2, 4.0)
#define FValidCoeffComp(r) FBetween(r, rSmall, 1000.0)

/* Compute Mollewide projection in pixel scale given latitude. */
#define NMollewide(y) \
  ((int)(sqrt((double)(180L*nScl*180L*nScl - 4L*(y)*nScl*(y)*nScl))+0.5))
/* Do settings indicate the current chart should have the info sidebar? */
#define fSidebar ((gi.nCurrChart == gWheel || gi.nCurrChart == gHouse || \
  gi.nCurrChart == gSector) && gs.fText && !us.fVelocity)
/* Is the current chart most properly displayed as a square graphic? */
#define fSquare \
  (gi.nCurrChart == gWheel || gi.nCurrChart == gHouse || gi.nCurrChart == gGrid || \
  (gi.nCurrChart == gHorizon && us.fPrimeVert) || gi.nCurrChart == gDisposit || \
  gi.nCurrChart == gOrbit || gi.nCurrChart == gGlobe || gi.nCurrChart == gPolar)
/* Does the current chart have to be displayed in a map rectangle? */
#define fMap \
  (gi.nCurrChart == gAstroGraph || gi.nCurrChart == gWorldMap)
/* Do settings indicate the current chart should have an outer border? */
#define fDrawBorder \
  ((gs.fBorder || gi.nCurrChart == gGrid) && gi.nCurrChart != gGlobe && \
  gi.nCurrChart != gPolar && (gi.nCurrChart != gWorldMap || !gs.fMollewide))
/* Do settings indicate current chart should have chart info at its bottom? */
#define fDrawText \
  (gs.fText && gi.nCurrChart != gEphemeris && gi.nCurrChart != gCalendar && gi.nCurrChart != gWorldMap && \
  gi.nCurrChart != gGlobe && gi.nCurrChart != gPolar && ((gi.nCurrChart != gWheel && \
  gi.nCurrChart != gHouse && gi.nCurrChart != gSector) || us.fVelocity))

#define SIZEOF_FIELD(structure, field) (sizeof(((structure *) 0)->field))
#define NUM_ELEMENTS(array)  (sizeof(array)/sizeof(*array))

//==================================================================================================

#define RSqr(r) sqrt(r)
#define RLength3(x, y, z) RSqr(Sq(x) + Sq(y) + Sq(z))
#define PtLen(pt) RLength3(pt.x, pt.y, pt.z)
double Longit = 0.0;
double Latit = 0.0;

double ObjPrime[cLastMoving + 1];
double StarPrime[cStar + 1];


wchar_t szFileName[MAX_FILE_NAME + 1];
wchar_t szFileTitle[MAX_FILE_NAME + 1];

#if 0
OPENFILENAMEW ofn = {
	sizeof(OPENFILENAMEW), (HWND)NULL, (HINSTANCE)NULL, NULL, NULL, 0, 1,
	szFileName,	MAX_FILE_NAME, szFileTitle, MAX_FILE_NAME, NULL, NULL,
	OFN_OVERWRITEPROMPT, 0, 0, NULL, 0L, NULL, NULL
};

PRINTDLGA prd = {
	sizeof(PRINTDLG),
	(HWND)NULL,
	(HGLOBAL)NULL,
	(HGLOBAL)NULL,
	hdcNil,
	PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC | PD_USEDEVMODECOPIES,
	1,
	0xFFFF,
	1,
	0xFFFF,
	1,
	(HINSTANCE)NULL,
	0L,
	NULL,
	NULL,
	(char *)NULL,
	(char *)NULL,
	(HGLOBAL)NULL,
	(HGLOBAL)NULL
};
#endif

/* Color tables for Astrolog's graphics palette. */

int ikPalette[cColor] = { -0, -1, 1, 4, 6, 3, -8, 5, -3, -2, -4, -5, -7, 2, 7, -6 };

/* These are the actual color arrays and variables used by the program.      */
/* Technically, Astrolog always assumes we are drawing on a color terminal; */
/* for B/W graphics, all the values below are filled with black or white.    */

KI kMainB[9], kRainbowB[8], kElemB[4], kAspB[cAspect + 1], kObjB[NUMBER_OBJECTS];

/*
******************************************************************************
** Graphics Table Data.
******************************************************************************
*/

#ifdef STROKE
//const char szObjectFont[cLastMoving + 2] = ";QRSTUVWXYZ     <    a  c     b  >         ";
//const char szAspectFont[cAspect + 1] = "!\"#$'&%()+-       ";
const char szObjectFont[] = ";QRSTUVWXYZ     <    a  c     b  >         ";
const char szAspectFont[] = "!\"#$'&%()+-       ";
#endif

//CharTables Glyph;
const char * szDrawSign[NUMBER_OF_SIGNS + 2] = { "",
	"ND4HU2HLGDFBR6EUHLGD2G",	/* Aries       */
	"BL3D2F2R2E2U2H2NE2L2NH2G2",	/* Taurus      */
	"BLU3LHBR7GLNL3D6NL3RFBL7ERU3",	/* Gemini      */
	"BGNDHLGDFRNEFR2EREBU3NHDGLHUENRHL2GLG",	/* Cancer      */
	"BF4H2UEU2H2L2G2D2FDGH",	/* Leo         */
	"BF4BLHNGNHEU5G2ND3U2HGND6HGND6H",	/* Virgo       */
	"BGNL3HUER2FDGR3BD2L8",		/* Libra       */
	"BH4FND6EFND6EFD6FREU",		/* Scorpio     */
	"BG4E3NH2NF2E5NL2D2",		/* Sagittarius */
	"BH3BLED4FND2EU2ENF2UFERFDGLF2D2G",	/* Capricorn #1 */
	"BG4EUEDFDEUEDFDEUEBU5GDGUHUGDGUHUGDG",	/* Aquarius    */
	"NL4NR4BH4F2D4G2BR8H2U4E2",	/* Pisces      */
	"BH4RFR2ER3G3D2GDFR2EU2HL3G2DG"	/* Capricorn #2 */
};

const char * szDrawSign2[NUMBER_OF_SIGNS + 2] = { "",
	"BD8U7HU3HU2H2L2G2D2F2BR12E2U2H2L2G2D2GD3G",	/* Aries  */
	"BH6BU2FDFRFNR4GLGDGD4FDFRFR4EREUEU4HUHLHEREUE",	/* Taurus */
	"",							/* Gemini */
	"BG5NRLH2U2E2R2F2D2G2F2R4ER2E3BH6NE2D2F2R2E2U2H2L2H2L4GL2G3",	/* Cancer */
	"",							/* Leo   */
	"",							/* Virgo */
	"",							/* Libra */
	"BH8F2ND12E2F2ND12E2F2D12F2RE2U3NGF",	/* Scorpio */
	"",							/* Sagittarius  */
	"",							/* Capricorn #1 */
	"BG8EUE2UEDFD2FDEUE2UEDFD2FDEUE2UEBU10GDG2DGUHU2HUGDG2DGUHU2HUGDG2DG",
	"NL8NR8BH8F3DFD6GDG3BR16H3UHU6EUE3",	/* Pisces */
	""					/* Capricorn #2 */
};

const char * szDrawObject[cLastMoving + 5] = {
	"ND4NL4NR4U4LGLDGD2FDRFR2ERUEU2HULHL",	/* Earth   */
	"U0BL4UM+1-2M+2-1RRM+2+1M+1+2DDM-1+2M-2+1LLM-2-1M-1-2U",	/* Sun     */
//	"U0BH3DGD2FDRFR2ERUEU2HULHL2GL",	/* Sun     */
	"BG3E2U2H2ER2M+2+1M+1+2D2M-1+2M-2+1L2H",	/* Moon    */
//	"BG3E2U2H2ER2FRDFD2GDLGL2H",	/* Moon    */
	"BD4UNL2NR2U2REU2HNEL2NHGD2FR",	/* Mercury */
	"LHU2ER2FD2GLD2NL2NR2D2",	/* Venus   */
	"HLG2DF2RE2UHE4ND2L2",		/* Mars    */
	"BH3RFDGDGDR5NDNR2U6E",		/* Jupiter */
	"BH3R2NUNR2D3ND3RERFDGDF",	/* Saturn  */
	"BD4NEHURBFULU3NUNR2L2NU2DGBU5NFBR6GD3F",	/* Uranus #1 */
	"BD4U2NL2NR2U5NUNRLBL2NUNLDF2R2E2UNRU",	/* Neptune   */
	"D2NL2NR2D2BU8GFEHBL3D2F2R2E2U2",	/* Pluto  #1 */

	"BG2LDFEULU3NURFRFBU5GLGLU2",	/* Chiron          */
	"BD4UNL3NR3U2RE2UH2L2G",	/* Ceres           */
	"BD4UNL3NR3UE2HUHNUGDGF2",	/* Pallas Athena   */
	"BD4UNL2NR2U4NL4NR4NE3NF3NG3NH3U3",	/* Juno            */
	"BU4DBG3NLFDF2E2UERBH2GDGHUH",	/* Vesta           */

	"BG2LGFEU2HU2E2R2F2D2GD2FEHL",	/* North Node      */
	"BH2LHEFD2GD2F2R2E2U2HU2EFGL",	/* South Node */
	"BG4E8BG2FD2G2L2H2U2E2R2F",	/* Lilith #1       */
	"NE2NF2NG2H2GD2F2R2E2U2H2L2G",	/* Part of Fortune */
	"U2NHNEBD4NGNFU2L2NHNGR4NEF",	/* Vertex          */
	"BH4NR3D4NR2D4R3BR2U8R2FD2GL2",	/* East Point      */

	"BG4U4NR2U3EFD7BR2NURU2HU2RDBR3ULD5RU",	/* Ascendant  */
	"BH3ER4FD2GLGLG2DR6",		/* 2nd Cusp   */
	"BH3ER4FD2GNL3FD2GL4H",		/* 3rd Cusp   */
	"BH4R2NR2D8NL2R2BR4NUL2U8R2D",	/* Nadir      */
	"BG3FR4EU2HL5U4R6",			/* 5th Cusp   */
	"BE3HL4GD6FR4EU2HL4G",		/* 6th Cusp   */
	"BH4D8REU6HLBF7DRU2HU2RDBG4NRU3NRU2R",	/* Descendant */
	"BL2GD2FR4EU2HNL4EU2HL4GD2F",	/* 8th Cusp   */
	"BG3FR4EU6HL4GD2FR4E",		/* 9th Cusp   */
	"BG4U8F2ND6E2D8BR4NUL2U8R2D",	/* Midheaven  */
	"BH3ED8NLRBR2RNRU8G",		/* 11th Cusp  */
	"BG4RNRU8GBR4ER2FD2GLG2D2R4",	/* 12th Cusp  */

	"BH4BRFDG2DR8BG3UNL2NR2U5LUEFDL",	/* Cupido    */
	"BENUNL2NR2D3ND2NR2L2H2U2E2R4",	/* Hades     */
	"BU4NG2NF2D7NDBLHLBR6LGL2GLBR6LHL",	/* Zeus      */
	"BU2D3ND3NR2L2BH2UE2R4F2D",	/* Kronos    */
	"U3NLR2NRD3NL2NR2D4NRL2NLU4L4UEUH",	/* Apollon   */
	"BUNU2NL2NR2D2ND3LHU2ENHR2NEFD2GL",	/* Admetos   */
	"G2DGR6HUH2U4NG2F2",		/* Vulcanus  */
	"ND4U4BL3DF2R2E2UBD8UH2L2G2D",	/* Poseidon  */
	"BE3L2D6R2BH3H0BH3R2D6L2",	/* Proserpina */

	// extra glyphs

	"BD2D0BU6NG2NF2D4LGD2FR2EU2HL",	/* Uranus #2 */
	"BL3R5EU2HL5D8R5",			/* Pluto  #2 */
	"UERHL2G2D2F2R2ELHU",		/* Lilith #2 no cross, hollow */
	"NUNGD5BH2R4BU2BLNL2NH4NUELNGNL3HNGNH2NL3UNL3NG2NH2UNG2NL3NHNU2ENL3NHNURHNG4LNG3LNFND6GND4GDNF3DF2", /* Lilith #3, filled-in, big cross */
};

// "NUNGD4BHR2BU2NL2NH4NUELNGNL3HNGNH2NL3UNL3NG2NH2UNG2NL3NHNU2ENL3NHNURHNG4LNG3LNFND6GND4GDNF3DF2", /* Lilith, filled-in, small cross */

const char * szDrawObject2[cLastMoving + 5] = {
	"ND8NL8NR8U8L2GLG3DGD4FDF3RFR4ERE3UEU4HUH3LHL2",	/* Earth */
	"U0BU8L2GLG3DGD4FDF3RFR4ERE3UEU4HUH3LHL2",	/* Sun   */
	"BG6E3UEU2HUH3E2R4FRF3DFD4GDG3LGL4H2",	/* Moon  */
	"",							/* Mercury */
	"",							/* Venus   */
	"BELHL4G3D4F3R4E3U4HUE7ND5L5",	/* Mars    */
	"BH6BRRF2D2GDGDGDGDR10ND2NR4U12E2",	/* Jupiter */
	"",							/* Saturn  */
	"BD4LGD2FR2EU2HLU6NU2NR4L4NU4D2G2BU10NF2BR12G2D6F2",	/* Uranus #1 */
	"BD8U4NL4NR4U10NU2NR2L2BL3LNU2NLD2FDFRFR4EREUEU2NLNRU2",	/* Neptune   */
	"D4NL4NR4D4BU16LGD2FR2EU2HLBL6D4FDFRFR4EREUEU4",	/* Pluto  #1 */
	"BG4LGD2FR2EU2HLU7RF2RF2RFBU10GLG2LG2BLU5",	/* Chiron        */
	"BD8U2NL6NR6U4R3E3U4H3L4G2",	/* Ceres         */
	"BD8U2NL6NR6U2E4HUHUHUHNUGDGDGDGF4",	/* Pallas Athena */
	"BD8U2NL4NR4U8NL7NR7NE5NF5NG5NH5U6",	/* Juno          */
	"BU8D3BG5NL3DF2DF2DFEUE2UE2UR3BH4GDG2DGHUH2UH",	/* Vesta         */
	"",							/* North Node      */
	"",							/* South Node      */
	"",							/* Lilith #1       */
	"",							/* Part of Fortune */
	"",							/* Vertex          */
	"",							/* East Point      */
	"BG8U8NR4U6E2F2D14BR4NHREU3HLHU3ERFBR6HLGD8FRE",	/* Ascendant */
	"",							/* 2nd Cusp   */
	"",							/* 3rd Cusp   */
	"BH8R4NR4D16NL4R4BR8BUNUGL3HU14ER3FD",	/* Nadir */
	"",							/* 5th Cusp   */
	"",							/* 6th Cusp   */
	"BH8D16R2E2U12H2L2BF14BGFREU3HLHU3ERFBG9NR3U5NR3U5R3",	/* Descendant */
	"",							/* 8th Cusp   */
	"",							/* 9th Cusp   */
	"BG8U16F4ND12E4D16BR8BUNUGL3HU14ER3FD",	/* Midheaven */
	"",							/* 11th Cusp  */
	"",							/* 12th Cusp  */
	"",							/* Cupido    */
	"",							/* Hades     */
	"",							/* Zeus      */
	"",							/* Kronos    */
	"",							/* Apollon   */
	"",							/* Admetos   */
	"",							/* Vulcanus  */
	"",							/* Proserpina */
	"",							/* Poseidon  */
	"",							/* Uranus #2 */
	"",							/* Pluto  #2 */
	"",							/* Lilith #2 */
	""							/* Lilith #3 */
};

const char * szDrawHouse[NUMBER_OF_SIGNS + 1] = { "",
	"BD2NLNRU4L", "BHBUR2D2L2D2R2", "BHBUR2D2NL2D2L2",
	"BHBUD2R2NU2D2", "BEBUL2D2R2D2L2", "NLRD2L2U4R2",
	"BHBUR2DG2D", "NRLU2R2D4L2U2", "NRLU2R2D4L2",
	"BH2NLD4NLRBR2U4R2D4L2", "BH2NLD4NLRBR2RNRU4L", "BH2NLD4NLRBR2NR2U2R2U2L2"
};

const char * szDrawHouse2[NUMBER_OF_SIGNS + 1] = { "",
	"BD4NL2NR2U8G2", "BH2BUER2FD2G4DR4", "BH2BUER2FD2GNLFD2GL2H",
	"BH2BU2D4R3NU4NRD4", "BE2BU2L4D4R3FD2GL2H", "NL2RFD2GL2HU6ER2F",
	"", "NRLHU2ER2FD2GFD2GL2HU2E", "NR2LHU2ER2FD6GL2H",
	"BH4NG2D8NL2R2BR5HU6ER2FD6GL2", "BH4NG2D8NL2R2BR4R2NR2U8G2",
	"BH4NG2D8NL2R2BR4NR4UE4U2HL2G"
};

const char * szDrawAspect[cAspect + 3] = { "",
	"HLG2DF2RE2UHE4",			/* Conjunction      */
	"BGL2GDFREU2E2U2ERFDGL2",	/* Opposition       */
	"BH4R8D8L8U8",				/* Square           */
	"BU4M-4+8R5R3M-4-8",	/* Trine            */
	//"BU4GDGDGDGDR8UHUHUHUH",	/* Trine            */
	"BLNH3NG3RNU4ND4RNE3F3",	/* Sextile          */
	"BG4EUEUEUEUNL4NR4BDFDFDFDF",	/* Inconjunct       */
	"BH4FDFDFDFDNL4NR4BUEUEUEUE",	/* Semisextile      */
	"BE4G8R8",					/* Semisquare       */
	"BD2L3U6R6D6L3D2R2",		/* Sesquiquadrature */
	"F4BU3U2HULHL2GLDGD2FDRFR2E3",	/* Quintile         */
	"BD2U3NR3NU3L3BD5R6",		/* Biquintile       */
	"BU2D3NR3ND3L3BU5R6",		/* Semiquintile     */
	"BH3R6G6",					/* Septile      */
	"BR3L5HUER4FD4GL4H",		/* Novile       */
	"BF2UHL2GFR3DGL3BE6LNLU2NRLBL4LNLD2NLR",	/* Binovile     */
	"BL2R4G4BE6LNLU2NRLBL4LNLD2NLR",	/* Biseptile    */
	"BL2R4G4BE6L7NLU2NLR3ND2R3ND2R",	/* Triseptile   */
	"BF2UHL2GFR3DGL3BU6LNLU2NLRBR2F2E2",	/* Quatronovile */
	"BU4BLD8BR2U8",				/* Parallel       */
	"BU4BLD8BR2U8BF3BLL6BD2R6"
};								/* Contraparallel */

const char * szDrawAspect2[cAspect + 3] = { "",
	"BELHL4G3D4F3R4E3U4HUE7",	/* Conjunction */
	"BG3HL2G2D2F2R2E2U2HE6HU2E2R2F2D2G2L2H",	/* Opposition  */
	"",							/* Square           */
//	"BU8GDGDGDGDGDGDGDGDR16UHUHUHUHUHUHUHUH",	/* Trine       */
	"BU8M+4+8M+4+8L8L8M+4-8M+4-8",	/* Trine       */
	"BU8D16BL8BU2E3RE3R2E3RE3BL16F3RF3R2F3RF3",	/* Sextile     */
	"BG8EUEUEUEUEUEUEUEUNL8NR8BDFDFDFDFDFDFDFDF",	/* Inconjunct  */
	"BH8FDFDFDFDFDFDFDFDNL8NR8BUEUEUEUEUEUEUEUE",	/* Semisextile */
	"",							/* Semisquare       */
	"",							/* Sesquiquadrature */
	"BFF7BU6U4HUH3LHL4GLG3DGD4FDF3RFR4E6",	/* Quintile */
	"",							/* Biquintile       */
	"",							/* Semiquintile     */
	"",							/* Septile      */
	"",							/* Novile       */
	"",							/* Binovile     */
	"",							/* Biseptile    */
	"",							/* Triseptile   */
	"",							/* Quatronovile */
	"",							/* Parallel        */
	""
};								/* Contraparallel */

const char * szWorldData[62 * 3] = {
	"-031+70",
	"LLRRHLLLLDULLGLLLDULGLLLGLRREDEGGLGGLGLGLLGDRLDRLFFRRERFDFRRREUEEHLUERERUERR\
FGLGLDDFRRRRREFRLGLLLLLGEFDLHGDDLGHLGLLHGLHURDLRRELLLRHUGLDFDLGLLFHGGLGLLLDLL\
LDRRFFDDGLLLLLLGDFGDDRRFRERREEUEREUEFRRERRFFFRFRDDLLLLRFRUREURULHLHHHEF",
	"5EUROPE",
	"+006+50", "RRERRRRUELLUHHLLREULLELLDGHDUFDEGRDRRLFDLLRGRRGGL",
	"5ENGLAND",
	"+008+55", "GLFGRRREUULL", "5IRELAND",
	"+023+64", "RRFRERRREHLLLLLGHLLRFLLRFL", "5ICELAND",
	"-011+80", "DDURFRERLGRRLLFRRREEFRRRLHGELLLHRRFRRRRERLLLLLLLLLLLDHGULLL",
	"5SVALBARD",
	"-014+45",
	"FRFRFDDFRDRRLLFRURFHHUERRRRRHUUEERRRRGRDERRLHLRRERRGGRFRFFGLLLLHLLLLGLLDLLLF\
GRFFRERFRERDDDGDGLLDFFEUDDFFDFFDDFFFDFDDDRRERRERRRUERRERURUEEHHLHUGGLLLUUGUHU\
HURRFFRFRRRDRRFRRRRRRRF",
	"5MIDDLE EAST",
	"-009+41", "DDRUULEUGD", "5SARDINIA",
	"-024+35", "RRLL", "5CRETE",
	"-032+35", "RRLL", "5CYPRUS",
	"-052+37", "LLHUURHUHUHERERRRDDLLLFFDDURFLLDFDDL", "0CASPAIN SEA",
	"-060+44", "LLUEERDFLDL", "0ARAL SEA",
	"-068+24",
	"FRGFRREDDDDDFDFDDFDDFERUEUUUUEEEEEREURRREFDFRDDDDRREFDDFDDGDDRFDDFDFFRUHUUHH\
HULUEUUURDRFDFRDEEREUUUHHHUUEERRDDEURRERREREEEUEULLREUHUHLEERRHLGLULUREERDLDR\
ERRFGRFDGRRREUHHUREUE",
	"6ASIA S",
	"-140+36",
	"DEUUEUHURREREEGLLHHDDGLDRGDDGGLGLLLGGLDLRDFEUHRRGEERDLLRGLRERRERRE",
	"6JAPAN",
	"-121+25", "GDFUEUL", "6TAIWAN",
	"-080+10", "DDDDREUHH", "6SRI LANKA",
	"-121+18", "LDDDRDDRHRRFFDDDLLEHDULRHDFDDGERDDREUUULUUHHLHEUUL",
	"2PHILIPPINES",
	"-131+43",
	"EFREEREEEUUUEUHLLUDLULEERERERRRRRRERRFLRRRRLUERERRRDRERURRGDLGLGLGLGGDDFDFEU\
RRUERUURULEEREDERRFRERERRRERRHLHLRRRREURDRRFRFRUURRHLLLDHHLLHLLHLLLLLLLDLLHRL\
LLLLLLGHULLLLLLLLLLULLLGL",
	"6SIBERIA",
	"-145+71",
	"RELLRHLLLLGDHGHLLLLGLLHUHLLLLLDLLLLHLLLLLDULUDLGLLLLRRERERRRELHLLLLLLLELLLLG\
DLLLLLUDLLLLLGLLLDLLLLLLLDFRDDHELLLLLLDRRLLHUDLGFGRRRRFRLHLLDGLGLLHRRREUHUUUL\
LGGLDRFGHLLLHLLLLRFGHLGLLLULGLLLGLLHRHLDDDLLLLDLLLFLLHUHLRRFRRRREHLLHLLLHLLL",
	"6RUSSIA",
	"-143+54", "GDDDDDDDEDUUURUUHUU", "6SAKHALIN",
	"-180+72", "GRRRRULLL", "6WRANGEL I.",
	"-137+76", "DRRRRRRRELLLLLLLL", "6SIBERIAN I.",
	"-091+80", "FERDRRRRRRULLLLLRRULLLLGL", "6SEVERNAYA",
	"-101+79", "GRRRRELLLL", "6ZEMLYA",
	"-068+77", "LLGLLLLLLGLLGGLGLRFRRRRLHERERERRRERRRREL", "6NOVAYA",
	"+123+49",
	"FGULLFDDDGFDDDFFDFRFRFDFFFDLFFRDFFEHHHHUHHUFRDFFFRDFFFDFGFRFRFRRFRRRRFFRRFRF\
FDRFFRFEUUGLHHUUEUHLLLLLEUUEULLLGDLLGLHHUHUUUEHEERERRFRRHRREFRRFDFDFEUUHUUUEE\
RERUUUHFDEUHFEURRRELUERRE",
	"4NORTH AMERICA S",
	"+113+42", "FH", "0SALT LAKE",
	"+156+20", "DRULHLHL", "4HAWAII",
	"+085+22", "RERFRRFRGRRRRHLHLHLLLLLG", "4CUBA",
	"+070+18", "RRHHLLLFLLLFRRRRRR", "4HAITI",
	"+078+18", "RRHLLF", "4JAMAICA",
	"+066+18", "ELLDR", "4PUERTO RICO",
	"+078+24", "UD", "4NASSAU",
	"+067+45",
	"REFLGDERERREHDLLLHUELLLGLGLREEERRRRRRREERRGGDGRRRFEFUUHLLLEUUHHGLRELLHHUHHHD\
GLGHHULLHLLLLLDFGFDDGLLFDDGHHUULLLLHLLHLLLUHUUEREEREERRRREUUHLLLDDGHULLLHLUHL\
GDRFGGULLLLLLLLLHLLGFLHLLLLLRHLLLLLHLLLLLLHGLLLLGUGLLLHLL",
	"4CANADA",
	"+088+49",
	"LGLGRRRRRRRFLLLGRGDDREUURUFRGRFGFERERREEREERLGGLGLLLGRLLGLEUERHLLLHULHL",
	"0GREAT LAKES",
	"+117+61", "REHRFRRERGLGLLLL", "0SLAVE LAKE",
	"+125+66", "RRERRRGREDLFHGLLLERLLLL", "0BEAR LAKE",
	"+097+50", "UULHURFDFG", "0LAKE WINNIPEG",
	"+090+72",
	"FRRLLFRRRRRRRRRRFRRGLLGRREEFRFLGLFLLLLFRERFRFRRFRRHLHFRRRUHLHRRFRURELLHLLLHR\
RHLHLHGHLHLLGLLEHFRRRHLLLLLLGLDFHLUELLGG",
	"4BAFFIN I.",
	"+125+72",
	"RFRREERRRLLGFFRRRRRLLLLLFRRRRRRRREFRRRRHRRLHLHHLRRULGLFLHLDLLULLLLHLLLLLLLDG",
	"4VICTORIA I.",
	"+141+70",
	"LLLLLLLLHGLHLLLHGLLGLLGLLDRRFRRDLLLULGLLFRRRRRRDLGLLGFDRRRDRRRRRGGGLLGLLGGLL\
RRERERRRERREERRELEERRRLLGDRERRURRFRRRRRFRRFUDRUDDHFDURDURLURDDLFRULURDHFFRGFE\
GRFFRFRFLHLHLFFRFE",
	"4ALASKA",
	"+045+60",
	"REUEREUERRRRERERRRERRRRERLLLLLLHRRRGERHFRRRRHLUDLLHLRERFRERLEUHRRHLEERLLURRR\
RRRRRELLLLLLLLLLGLLLRERHGLRELLLLLLLELLLLLLLLLLGLLLLLLGLLLLLLGLULLLLLLLFRLLLLL\
GLRRRGLLLLLLLGRRRRRRRGLLLLRRFRRRRRRRRRRFDFDLFREFRDLLLDERRFGLLGFFDRFFFRRRF",
	"4GREENLAND",
	"+080+10",
	"DRFDFDDGGGDDGRDGDDFFDFDFFDFFRFFFDDDDDDGDDDDGDDDDGDGFGDDDEUDDDGUDDLDRGDDDFDFR\
FRRFERRLHLUHUURUEELHEREURULURREURREREUHUUDFRREEEEEUEUUEERERRREUEUEUUUUUEEEEUU\
UHLHLHLLLLHLHLGEHLGEUHUUHLHLLLHHLHULEDLLELLGHLLHLGDDHUELLGLGDGHHL",
	"3SOUTH AMERICA",
	"+060-51", "LDRRELL", "3FALKLAND ISLANDS",
	"+092+00", "FUL", "3GALAPAGOS I.",
	"-032+32",
	"LLGLHLLLLHLGDGHLLHHLLHLEUULLLLLLLLLGLGLLLLHDGLGDGDGGLDGGGDGDFDDDDGDDFFFFDFRF\
FRRRRRRRRERERRFFRRFFDDDGDFFFDFDDDFDGDGDDDFDFDFDDDFDFDFDDFFERRRRREEEEEEEUUEREU\
UHUEEEREEUUUUHUUUHUEUEEEEEREEUEUEEUUULLLLGLLHUHHLHUHHUUHHUUHUHHUU",
	"1AFRICA",
	"-049-12", "DGGGLGDDDDGDDFFREUEUEUUUEUUUUH", "1MADAGASCAR",
	"-032+00", "DDDREUELLL", "0LAKE VICTORIA",
	"-014+14", "LRFLU", "0LAKE CHAD",
	"-124-16",
	"LGDGGLGLLGLDDDGFDDFDFDGFRRRERRRRURERRRRRRRFFFEEDDRFDFRFREFRERRUUEUEEUUUUUUUH\
HHHHHHUUHHHUULDDDDGDGHLHLHEUELLLHLFLLULDRGDDLLHLGG",
	"2AUSTRALIA",
	"-173-35", "FFDGFDREURULHHHL", "2NEW ZEALAND N",
	"-174-41", "LLDGLGLGGRFREEUREEU", "2NEW ZEALAND S",
	"-145-41", "DFRRUUUDLLL", "2TASMANIA",
	"-178-17", "GRRURUGDH", "2FIJI",
	"-130+00", "FRFRLGFEFRFRFDGRRFRRUERFFFRRRLHHHHRHLHHLHLLHGGLHUHLGH",
	"2NEW GUINEA",
	"-115-04", "RUUEEURHUUEHHGGGGLLDDHLDDFDDRRDERF", "2BORNEO",
	"-095+06", "DFFFFFFDFFFFRUUUHFRHLHLUHHHHHLLH", "2SUMATRA",
	"-106-06", "GRFRRRRRRFRRHLHLLLLLHL", "2JAVA",
	"-120+00", "DGDDRDFHUEDFRHUHREFHLGHURRRRELLLLG", "2CELEBES",
	"+000-70",
	"ULDLLLLLLLLGLLGLLLGLLGLLLLGLGLLGLLLLGLLLLLHLGLLLLLHLLLLLHLLLLHLLUERLEUUUUUUE\
ERRRULLGLLLLGLGGLLLDRUDRDLGHLLGLLFGRRFLLLLLLLDHLLLLHLLLLLGLLLLHLLLLLLLGRFDLLL\
ULLLGHLLLLLLLLLLHGHLLGLLLLLLLGLLLLLLLLLLLGLLLGLLLLLLLLGLLLLLLLLLLLLLLLLLLLLL",
	"7ANTARCTICA W",
	"+180-78",
	"LLLLLLLHLLGHLLGHLUEERRERREHLLLLHLLLLLLHLLLLLLLLLLLHLHLLLLLHLLULDLLLLLDLLHLLL\
LGHFLLLLLHLLLLLLGLHLLHLGLLLLHLGLLGLLLULLLGLLHDFLLLGLGLLLELLLLHLLLLLLLLLLHLLLH\
LLLLGGHGHGLLLGLDLLLLHLLGHGLLLLLLLLLLLLLLHLGLLLLLLLLLLLLLL",
	"7ANTARCTICA E",
	"", "", ""
};

#ifdef CONSTEL
const char * szDrawConstel[cCnstl + 1] = { "",
	"550210+51DDd3r8d2Rr7d2Rr3Dd5l2d3r10uru6rUu2Rr2ur4u2RrUUu3Ll7d2l3DdLl5d2Lu2l4\
Uul8Dd2Ll3Uul7",				/* Andromeda */
	"660913-25d2Ll5Dl5d2l4d4LlDRRr8Uu5l6",	/* Antila */
	"561804-68DDd3RRRRRr2Uu9LLLLl3Uu2Ll4",	/* Apus */
	"362213+02Dd3Ll14DDd5RRrUUur7Dd4Rr6UUu2Ll9ul3dLl13",	/* Aquarius */
	"562003+16Ddl3d7l3Dd9r7Dd2RRUu6r5Uu2l4u4r3Uu2l3u7Lld2l13dLl3",	/* Aquila */
	"641803-45Dd7Rr5Dd8Ru3rur2u3r3UUu5LLl3",	/* Ara */
	"560307+31DDrd9RRr3Uu6Llu2l7UuLl7",	/* Aries */
	"650604+56d2l6Dl4d6Ll7Dd5Rr5Dd2RuRr2Ur4u6l3UUu3Ll3u3Ll4",	/* Auriga */
	"431504+55d2l8DDdr4d7r4Dd5rDDd2RRr6UUu8LlUlUUu5Ll4",	/* Bootes */
	"560501-27DDd3Rr2d3r5d3r4Uu3l5Ul2u3Ll",	/* Caelum */
	"751407+86DdRr6d3RRr6u3Rr2UuRr9Dd7Rr12DDRr12u2Rr11Dd4Rr12d3RRr8u2r2u2rUu8l6Uu\
7lULLl6u5LLLl5uLLLLLLl7",		/* Camelopardalis */
	"550906+33DDDd3Rr14URrULu8l2Uu3Ll6",	/* Cancer */
	"551309+52Dd2Ll2DDrd2Rr9UuRr8u2r5Uu4lUu2Ll9",	/* Canes Venatici */
	"550707-11DDd3Rr12UUu9Ll7",	/* Canis Major */
	"660714+13DLld3l2DRr10ur3Uu2l7ul5",	/* Canis Minor */
	"562114-09DDd5r7d2Rr13UUul7Dd4Ll7Uul7",	/* Capricornus */
	"360804-51d2l4d2l5d2LLLl5DDd5RRRUu6RRr2Uu2r4u3r6u2r2u2LLl4",	/* Carina */
	"440310+77Dd2r6Dd3Rr6u2Rdr3d3r4d4r4DdRd2r10u2Rr5Uur3u2RrULl4u4l6u3Ll6Uu7LLLl1\
0",								/* Cassiopeia */
	"551501-30Dd3Rr11Dd5l6Dd4RRrUu5RrDd4r9Uu3r4UUu4Ll5u2l5ULLLl",	/* Centaurus */
	"850805+88d3RRRr9DRRr4d3RRRr9Dd3Rr5d3r6DRrd3r7dr3d3r3u2RRr5UurdRu2l7u5l3Uu5r9\
Uul12u5LLl14u2LLLLLLLLLLl5",	/* Cepheus */
	"560306+11DDdRr4DDd4Rr4dRRrUUu4Ll7Uu3LLl2UuLl6",	/* Cetus */
	"561313-75Dd3RRRRRRr6Uu5LLLLLLl13",	/* Chamaeleon */
	"341507-55Ddr2d3r3d4RrDdRr2Uu5r2uLl10Uu5Ll7",	/* Circinus */
	"660603-27Dd3l7Dd3Rr14UUu3Ll3",	/* Columba */
	"561207+33d2Ll6Dd2l3Dd6RrdRUu8l2Uu3l5",	/* Coma Berenices */
	"561905-37Dd5RRUu3Ll5",		/* Corona Australis */
	"451606+39Dd4r2dRr11Uu2l4u7Ll6",	/* Corona Borealis */
	"551214-12Dd3r4d2RrUu8Ll14",	/* Corvus */
	"551114-07DDd5RrUrUu3Ll14",	/* Crater */
	"561214-55Dd4RrUu5Ll14",	/* Crux */
	"552010+61Dd5LLlDd5RdrDd4r2Dd2RuRr4d2r6Ulu7lUu4r4u4rUu6l5u2l5ULl9ul",
	/* Cygnus */
		"542010+21DLl2d8RrDd4rd4r7u7r3Uu6l2Uul5",	/* Delphinus */
		"570408-49Dd4Lld3l7DdLd3l8DRRr6Uur3u3r5u3Rr2u2Ll2Uul6",	/* Dorado */
		"352013+86d5r12Dd5l9Dd3r3d5RDr3d2r5d2r5Dd2Rr10UuRr14uRRr3ur8u2Rr8Uu2Rr7uRr14u\
3Rr8Uu3RRr9UuLl13DdLl9d3LLlDdLd4Ll10ULl8u5Ll7Ul7u6LLLl13",	/* Draco */
	"562107+13Dd8Rr2u4lUu2Ll3ul4",	/* Equuleus */
	"430411+00d4Ll3DdRd3rDd7r3Dr2d7r5DRrd4r7d2r6d3Rr4Ddr4d3r4d4Rr8u5l3u2l4Uu2Ll6U\
Llul7u4l4Uu6Rr4UUu9Ll10ULl11",	/* Eridanus */
	"550312-24Dd5r4d4r7DRRr4Uu6LLl12",	/* Fornax */
	"560713+35d2Ll2Dd2RDrd7r5dr7Dru2Rr9u6rUu2Ru6l10Uu5Ll13",	/* Gemini */
	"442307-36DDd6Rr13UuRr8Uu4LLl7",	/* Grus */
	"551805+51Dd2rDDl3d4l7Dd8r8u2Rr9dRr2Dd6r10Uu6RUu2l2u3l2ul2Uu9Rr3UUu2LLldLl5",
	/* Hercules */
		"770404-40d9r2DdRr2d2r5d4r4Dd7Rr12Uu6l4u3l4UuLlu3l6u2l7ULl4",	/* Horologium */
		"760910+07DDd2Ll13Dld5LLl10u2LLl6d2LlDRRRr5d4r5d2RRru4r4Ur5u3Rr2u2r10URr5u2r3\
u7r4UUu7Ll10",					/* Hydra */
	"560203-58Dd7LLl9Dd5Rr8Dd2RRRr14Uu6l12dLl6UUu2Ll3",	/* Hydrus */
	"742107-45d4Ll2DDd7Ll7Dd4RRr8UURr8Uu5Ll7",	/* Indus */
	"562214+57DDd5r13uRrUu4LulUu3l3u3l3ul7",	/* Lacerta */
	"551200+28DDr6Dd7Rr2Uu7Rr9UUUu3LDd2l9d5l4u2Ll2u3L",	/* Leo */
	"451004+41Ddl9d6Ll2Dd5Rr2d2r4u5RUu3r9u6l5UuLl4",	/* Leo Minor */
	"550603-11Dd7RRrUu6Lu3Ll3",	/* Lepus */
	"551600-04DDdr3Dr11u5Rr9UUu2l6u7Ll3d3L",	/* Libra */
	"331602-30Dd2Rr3d6r5Dd4r4dRr11Uu7LlUULl2",	/* Lupus */
	"640703+62DLl9Dd4Ll5d5l6Ddr5d6RRr2u2r6Uu4RrUr4u4r6Uu2Ll3",	/* Lynx */
	"551903+48d4l4Dd3rDrd5Rrur7Ur3Uu8Ll3",	/* Lyra */
	"560608-70d5Ll9Dd5RRRRr8Uu5Ll9ULLl8",	/* Mensa */
	"552107-27DDd5Rr8UUu3Ll7",	/* Microscopium */
	"470701+12Dld9l3DLl3DdRRRrUu6Ll5UUlu2Ll",	/* Monoceros */
	"561311-64dl2Dd5RRr10Uu6LLl11",	/* Musca */
	"561609-42DDRr8u5r4ul4Uu2l5u6Ll9",	/* Norma */
	"270000-74lDd2LLLl7d3LLLLl9u2LLLLLLLLLLLl4Uu6LLLLLL",	/* Octans */
	"641806+14d2l5Dd4r5d2l2dr2DRrd4LlDRr3d2rUr7d6l7DDRr2u5r7UlurUu2Ru4l6Uu4l7Uu3L\
l6uLl6",						/* Ophiuchus */
	"560600+23dl5Dd2lDrDd4RrDdr11Uu6Rr4UrUu5Llul5dl4d3l3u6rUu3L",	/* Orion */
	"552007-57DLl7Dd4RRRr11Uu2Rr5Uu3LLLl7",	/* Pavo */
	"552201+36dLl9d2l4dLl2Ddld6lDd7r2d2RrDd2Rr2d6Rr3ur3dr2Uu3r4drUl3u4l3u4l4Uu6Ll\
",								/* Pegasus */
	"460209+59d2Ll5d2l2d2Ll12DDd4r3d5RRr2u3r2u3rUUur7Dd2Rr3Uur4u4l4u3Lul9",
	/* Perseus */
		"650206-40d8RrDdr4d2r3d5RRr8UULLLl6",	/* Phoenix */
		"640601-43Dd3l2d2l6d3l4Dd4Ru3r7Uu3r7u3Rr7Uu4l5u3LLl",	/* Pictor */
		"450108+33Ddl4Dd9Ll2Dd7RRr8Dd6Rru3Rr2Uu8Ll14UuLl2u2l2Uu2l10u3r2Uu3Ll8",
	/* Pisces */
		"562302-25Dd6RRr8Uu5LLl2",	/* Piscis Austrinus */
		"570807-11DDDd3r6DdRRr14Uu7l9Uu7Ll7UUu9Ll7",	/* Puppis */
		"560810-18DLl3d5l4Dd7Rr8UUu2l3",	/* Pyxis */
		"560401-53d3l5d3l3Dd7Rr11Uu3l4u4Ll",	/* Reticulum */
		"452005+22Dd4Rrur13u2RrUu2Ll5Dl9u2Ll5",	/* Sagitta */
		"552002-12Dd7l5DDd5Rr10Uu3RRUr4Uu4LLu4Ll2",	/* Sagittarius */
		"471606-08Dd9lDrd5l7DLLDd5Rr6u3r7UURr3u9LUUu2l6",	/* Scorpius */
		"560111-25DDRRr8u4r5Uu5LLl11",	/* Sculptor */
		"551900-04Dd6r9Uu6L",		/* Scutum */
		"861814+06d4r4Dd4r4Dd6Rr11Ul7d2lu2Llu6RrULl6u3l2ur2u2l8bRbRbRbUbUd4l3Dd6l3Dd4\
Rr12UUUu5Ll2d3RD",				/* Serpens */
	"551013+07DDd2Rr5UUu7Ll13",	/* Sextans */
	"640600+29d6r3Dd2ld6r3u3r4ur5dRr5DDRr5dr4UUUlUuLl9Dl4dLL",	/* Taurus */
	"552007-45Dd7RRr12Uu5LLl7",	/* Telescopium */
	"560211+37d3l2d3r5Dd2r7d2Rr3u3r4Uu5Ll2u2l9",	/* Triangulum */
	"561609-60dl3d3l2dLd3l3DdRRRrUu2Ll2u4l3u3l2ULl9",	/* Triangulum Australe */
	"360106-58DDd5Rr2uRr8Uu3Rr13Uu4Ll7d2LLl6",	/* Tucana */
	"641107+73Dd4Lld3Ll8dLl7Dd5r5Dd2Rr6Uu2Rr12Dd6rDDd2Rr13Uu3Rr2u6r9UuRr10u5Rr6UU\
r6Uu3LLLl7",					/* Ursa Major */
	"342200+86RRRRrDr7d5Rr7DRr5d5RRu4r14Uu6l8u3Ll7Uu6RRRRRRr10u2RRRRRRRRRRrd2R",
	/* Ursa Minor */
		"560907-37DLLlDd7RRRr2u2r5u2r4u2r3Uu7l6Uu3Ll7",	/* Vela */
		"551309+14Dd2LLl3DdRr3d7r6DDd3RRrUu8RrUu3r5UULu3l14uLl9",	/* Virgo */
		"560900-64Dd5RRr7Uu6LLL",	/* Volans */
		"462100+29dl8d4r3DRr5ur5uRrDr9u2Rru3Ll5u2l6u2LL"
};								/* Vulpecula */
#endif /* CONSTEL */
#if 0
void InitTurtleFont()
{
	Glyph.ParseAdd(L"", L'|');
	Glyph.AddString(0, L"BR2D4BD2D0");
	Glyph.AddString(0, L"BRD2BR2U2");
	Glyph.AddString(0, L"BD2R4BD2L4BFU4BR2D4");
	Glyph.AddString(0, L"BR2D6BENL3EHL2HER3");
	Glyph.AddString(0, L"RDLNUBR4G4BR4DLUR");
	Glyph.AddString(0, L"BD2NF4UEFDG2DFRE2");
	Glyph.AddString(0, L"BR2DG");
	Glyph.AddString(0, L"BR3G2D2F2");
	Glyph.AddString(0, L"BRF2D2G2");
	Glyph.AddString(0, L"BD2FNGRNU2ND2RNEF");
	Glyph.AddString(0, L"BD3R2NU2ND2R2");
	Glyph.AddString(0, L"BD5BR2DG");
	Glyph.AddString(0, L"BD3R4");
	Glyph.AddString(0, L"BD6BRRULD");
	Glyph.AddString(0, L"BD5E4");
	Glyph.AddString(0, L"BDD4NE4FR2EU4HL2G");
	Glyph.AddString(0, L"BFED6NLR");
	Glyph.AddString(0, L"BDER2FDG4R4");
	Glyph.AddString(0, L"BDER2FDGNLFDGL2H");
	Glyph.AddString(0, L"D3R3NU3ND3R");
	Glyph.AddString(0, L"NR4D3R3FDGL2H");
	Glyph.AddString(0, L"BR3NFL2GD4FR2EUHL3");
	Glyph.AddString(0, L"R4DG4D");
	Glyph.AddString(0, L"BDDFNR2GDFR2EUHEUHL2G");
	Glyph.AddString(0, L"BD5FR2EU4HL2GDFR3");
	Glyph.AddString(0, L"BR2BD2D0BD2D0");
	Glyph.AddString(0, L"BR2BD2D0BD2G");
	Glyph.AddString(0, L"BR3G3F3");
	Glyph.AddString(0, L"BD2R4BD2L4");
	Glyph.AddString(0, L"BRF3G3");
	Glyph.AddString(0, L"BDER2FDGLDBD2D0");
	Glyph.AddString(0, L"BF2DFEU2HL2GD4FR2");
	Glyph.AddString(0, L"BD6U4E2F2D2NL4D2");
	Glyph.AddString(0, L"D6R3EUHNL3EUHL3");
	Glyph.AddString(0, L"BR3NFL2GD4FR2E");
	Glyph.AddString(0, L"D6R2E2U2H2L2");
	Glyph.AddString(0, L"NR4D3NR3D3R4");
	Glyph.AddString(0, L"NR4D3NR3D3");
	Glyph.AddString(0, L"BR3NFL2GD4FR2EU2L2");
	Glyph.AddString(0, L"D3ND3R4NU3D3");
	Glyph.AddString(0, L"BRRNRD6NLR");
	Glyph.AddString(0, L"BD4DFR2EU5");
	Glyph.AddString(0, L"D3ND3RNE3F3");
	Glyph.AddString(0, L"D6R4");
	Glyph.AddString(0, L"ND6F2NDE2D6");
	Glyph.AddString(0, L"ND6F4ND2U4");
	Glyph.AddString(0, L"BDD4FR2EU4HL2G");
	Glyph.AddString(0, L"R3FDGL3NU3D3");
	Glyph.AddString(0, L"BDD4FRENHNFEU3HL2G");
	Glyph.AddString(0, L"ND6R3FDGL2NLF3");
	Glyph.AddString(0, L"BR3NFL2GDFR2FDGL2H");
	Glyph.AddString(0, L"R2NR2D6");
	Glyph.AddString(0, L"D5FR2EU5");
	Glyph.AddString(0, L"D2FDFNDEUEU2");
	Glyph.AddString(0, L"D6E2NUF2U6");
	Glyph.AddString(0, L"DF4DBL4UE4U");
	Glyph.AddString(0, L"D2FRND3REU2");
	Glyph.AddString(0, L"R4DG4DR4");
	Glyph.AddString(0, L"BR3L2D6R2");
	Glyph.AddString(0, L"BDF4");
	Glyph.AddString(0, L"BRR2D6L2");
	Glyph.AddString(0, L"BD2E2F2");
	Glyph.AddString(0, L"BD6R4");
	Glyph.AddString(0, L"BR2DF");
	Glyph.AddString(0, L"BF4G2LHU2ER2FD3");
	Glyph.AddString(0, L"D5NDFR2EU2HL2G");
	Glyph.AddString(0, L"BF4BUHL2GD2FR2E");
	Glyph.AddString(0, L"BR4D5NDGL2HU2ER2F");
	Glyph.AddString(0, L"BD4R4UHL2GD2FR3");
	Glyph.AddString(0, L"BD3RNR3ND3U2ERF");
	Glyph.AddString(0, L"BD8R3EU4HL2GD2FR2E");
	Glyph.AddString(0, L"D3ND3ER2FD3");
	Glyph.AddString(0, L"BR2D0BD2D4");
	Glyph.AddString(0, L"BR2D0BD2D5GLH");
	Glyph.AddString(0, L"D4ND2REREBD4HLH");
	Glyph.AddString(0, L"BR2D6");
	Glyph.AddString(0, L"BD2DND3EFNDEFD3");
	Glyph.AddString(0, L"BD2DND3ER2FD3");
	Glyph.AddString(0, L"BD3D2FR2EU2HL2G");
	Glyph.AddString(0, L"BD2DND5ER2FD2GL2H");
	Glyph.AddString(0, L"BR4BD8U5HL2GD2FR2E");
	Glyph.AddString(0, L"BD2DND3ER2F");
	Glyph.AddString(0, L"BD6R3EHL2HER3");
	Glyph.AddString(0, L"BR2D2NL2NR2D4");
	Glyph.AddString(0, L"BD2D3FRE2NU2D2");
	Glyph.AddString(0, L"BD2DFDFEUEU");
	Glyph.AddString(0, L"BD2D3FENUFEU3");
	Glyph.AddString(0, L"BD2F2NG2NE2F2");
	Glyph.AddString(0, L"BD2D3FR2ENU3D2GL3");
	Glyph.AddString(0, L"BD2R4G4R4");
	Glyph.AddString(0, L"BR3GDGFDF");
	Glyph.AddString(0, L"BR2FGHE");  //Glyph.AddString(0, L"BR2D2BD2D2");
	Glyph.AddString(0, L"BRFDFGDG");
	Glyph.AddString(0, L"BFEFE");
	Glyph.AddString(0, L"BD6R4");
	Glyph.AddString(0, L"BR4BDHL2GD3FR2ED3L3");
	Glyph.AddString(0, L"BD2D3FRE2NU2D2BH3BU3R0BR2R0");
	Glyph.AddString(0, L"BD4R4UHL2GD2FR3BU6BL2E");
	Glyph.AddString(0, L"BF4G2LHU2ER2FD3BU6BLHG");
	Glyph.AddString(0, L"BF4G2LHU2ER2FD3BH3BU3R0BR2R0");
	Glyph.AddString(0, L"BF4G2LHU2ER2FD3BU6BL2H");
	Glyph.AddString(0, L"BF4G2LHU2ER2FD3BU6BLGHEF");
	Glyph.AddString(0, L"BR4BD2L3GDFR2ED3L2");
	Glyph.AddString(0, L"BD4R4UHL2GD2FR3BU6BLHG");
	Glyph.AddString(0, L"BD4R4UHL2GD2FR3BU6BLL0BL2L0");
	Glyph.AddString(0, L"BD4R4UHL2GD2FR3BU6BL2H");
	Glyph.AddString(0, L"BRR0BR2R0BGBDD4");
	Glyph.AddString(0, L"BREFBGBDD4");
	Glyph.AddString(0, L"BEF2BGBDD4");
	Glyph.AddString(0, L"BD6U3E2F2DNL4D2BU7BLL0BL2L0");
	Glyph.AddString(0, L"BD6U3E2F2DNL4D2BU6BLGHEF");
	Glyph.AddString(0, L"BFE2BFBD2L4D2NR4D2R4");
	Glyph.AddString(0, L"BD2RD4LU2R4U2LD4R");
	Glyph.AddString(0, L"BD6U4E2R2BD6LU3NRNL3U3");
	Glyph.AddString(0, L"BREFBD2FD2GL2HU2ER2");
	Glyph.AddString(0, L"BD3D2FR2EU2HL2GBU3BRR0BR2R0");
	Glyph.AddString(0, L"BD3D2FR2EU2HL2NGBU3F");
	Glyph.AddString(0, L"BD2D3FRE2ND2U2BU2BLHG");
	Glyph.AddString(0, L"BD2D3FRE2ND2U2BU2BL2H");
	Glyph.AddString(0, L"BD2D3FR2ENU3D2GL3BU8BRR0BR2R0");
	Glyph.AddString(0, L"BD2D3FR2EU3HL2GBU3BRR0BR2R0");
	Glyph.AddString(0, L"BDD4FR2EU4BUBHL0BL2L0");
	Glyph.AddString(0, L"BD3D2FR2EU2HL2GBD3E4");
	Glyph.AddString(0, L"BR4BDHLGD2NLNR2D2GR4");
	Glyph.AddString(0, L"BDD4FR2EU4HL2GBD5EUE2UE");
	Glyph.AddString(0, L"BD3BRF2BU2G2");
	Glyph.AddString(0, L"BR3BFHGD2NLNRD2GH");
	Glyph.AddString(0, L"BF4G2LHU2ER2FD3BU6BL2E");
	Glyph.AddString(0, L"BEBR2G2BFD4");
	Glyph.AddString(0, L"BD3D2FR2EU2HL2NGBUBEE");
	Glyph.AddString(0, L"BD2D3FRE2ND2U2BH2E");
	Glyph.AddString(0, L"BD2DND3ER2FD3BU7GHG");
	Glyph.AddString(0, L"BDND5F4NDU4BU2GHG");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BR2D0BD2DLGDFR2E");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BR2BUD0BD2D5");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BD6U3E2F2DNL4ND2BU4BL3E");
	Glyph.AddString(0, L"BD6U3ER2FDNL4ND2BU4BLHG");
	Glyph.AddString(0, L"BD6U3E2F2DNL4ND2BU4BLH");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BF3BRHLNU2LGD2FRND2RE");
	Glyph.AddString(0, L"F2NE2DNLNRD2NLNRD");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BF4G2LHU2ER2FD3BU7GHG");
	Glyph.AddString(0, L"BD6U3ER2FDNL4ND2BU5GHG");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BDFR2NEFD2GNFL2NGHU2E");
	Glyph.AddString(0, L"F4DGL2HUER2");
	Glyph.AddString(0, L"R3FD4GL2NlU3NLNRU3");
	Glyph.AddString(0, L"BREFBFBDL4D2NR4D2R4");
	Glyph.AddString(0, L"BRR0BR2R0BFBDL4D2NR4D2R4");
	Glyph.AddString(0, L"BEF2BFL4D2NR4D2R4");
	Glyph.AddString(0, L"BD2BRRD4NLR");
	Glyph.AddString(0, L"BR2EBD2LNLD5NLR");
	Glyph.AddString(0, L"BREFBDLNLD5NLR");
	Glyph.AddString(0, L"BER0BR2R0BD2LNLD5NLR");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BR2D2BD2D2");
	Glyph.AddString(0, L"BEFBDNLNRD5NLR");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BD2D3FR2EU3HL2GBE2E");
	Glyph.AddString(0, L"BD7U6ERFDGNL2F2DG");
	Glyph.AddString(0, L"BD2D3FR2EU3HL2GBEBUEF");
	Glyph.AddString(0, L"BD2D3FR2EU3HL2GBE2H");
	Glyph.AddString(0, L"BD3D2FR2EU2HL2GBU2BREFE");
	Glyph.AddString(0, L"BD2D3FR2EU3HL2GBU2BREFE");
	Glyph.AddString(0, L"BFBDD3NFD2GBE2REU3");
	Glyph.AddString(0, L"BRNRD7NLNRBU2R2EUHL2");
	Glyph.AddString(0, L"BURNRD8NLNRBU2R2EU2HL2");
	Glyph.AddString(0, L"BDD4FR2EU4BHBLE");
	Glyph.AddString(0, L"BD2D3FR2EU3BUBHHG");
	Glyph.AddString(0, L"BDD4FR2EU4BHBLH");
	Glyph.AddString(0, L"BD2D3FR2ENU3D2GL3BU8BR2E");
	Glyph.AddString(0, L"D2FRND3REU2BL2E");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BF4BD2U6L2ND6LGDFR");
	Glyph.AddString(0, L"BR4HLGF3G2H2E2BD4FGLH");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"BR2FGHE");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
	Glyph.AddString(0, L"");
}

#endif
#define PrintL() is.szFileScreen? PrintSzW(L"\n"):PrintSzW(L"\n")

extern void CoorXform(double* azi, double* alt, double tilt);

#define chSig3(A) tSignName[A][0], tSignName[A][1], tSignName[A][2]
#define chSig3C(A) tSignName[A][0], tSignName[A][1], tSignName[A][2]
#define chSig2C(A) tSignName[A][0], tSignName[A][1]

#define chObj3(A) tObjShortName[A][0], tObjShortName[A][1], tObjShortName[A][2]
#define chObj3C(A) tObjShortName[A][0], tObjShortName[A][1]
#define chMon3(A) tMonth[A][0], tMonth[A][1], tMonth[A][2]
#define chMon3C(A) tMonth[A][0], tMonth[A][1], tMonth[A][2], tMonth[A][3]
#define chDay3(A) tDay[A][0], tDay[A][1], tDay[A][2]
#define chDay3C(A) tDay[A][0], tDay[A][1], tDay[A][2],tDay[A][3]
#define chDay2(A) tDay[A][0], tDay[A][1]

#define kSignA(s) kElemA[(s)-1 & 3]
#define kSignB(s) kElemB[(s)-1 & 3]
#define kModeA(m) kElemA[m <= 1 ? m : eWat]

#define FIgnore(i)  (ignore1[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)
#define FIgnore2(i) (ignore2[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)
#define FIgnore3(i) (ignore3[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)

#define notV(V) V = !(V)
#define negV(V) V = -(V)

#define PrintL() is.szFileScreen? PrintSzW(L"\n"):PrintSzW(L"\n")
#define PrintL2() is.szFileScreen? PrintSzW(L"\n\n"):PrintSzW(L"\n\n")

#define SwapN(n1, n2) {int  tmp = n1; n1 = n2; n2 = tmp;}
#define SwapR(r1, r2) {double tmp = r1; r1 = r2; r2 = tmp;}

#define FSwitchF(f) ((((f) | fOr) & !fAnd) ^ fNot)
#define SwitchF(f) f = FSwitchF(f)
#define SwitchF2(f) f = (((f) | (fOr || fNot)) & !fAnd)

/* Should an object in the outer wheel be restricted? */
#define FProper2(i) ( !( us.nRel == rcProgress ? ignore3[i] : ( ( (us.nRel == rcTransit || isSolarReturn || isLunarReturn) || us.nRel == rcProgTran) ? ignore2[i] : ignore1[i])) && i != us.objCenter)
#define FProper3(i)  ( !ignore3[i] && i != us.objCenter)
#define FProper21(i) ( !ignore2[i] && i != us.objCenter)

#define FProper22(i) ( !(IsComparison ? ignore3[i] : (IsTransitAndNatal ? ignore2[i] : ignore1[i])) && i != us.objCenter)    

/* Are particular coordinates on the chart? */
#define FInRect(x, y, x1, y1, x2, y2) \
  ((x) >= (x1) && (x) < (x2) && (y) >= (y1) && (y) < (y2))
#define FOnWin(X, Y) FInRect((X), (Y), 0, 0, gs.xWin, gs.yWin)
/* Get a coordinate based on chart radius, a fraction, and (co)sin value. */
#define POINT1(U, R, S) ((int)(((U)+1.4)*(R)*(S)))
#define POINT2(U, R, S) ((int)(((U)-0.3)*(R)*(S)))
/* Determine (co)sin factors based on zodiac angle and chart orientation. */
#define PX(A) RCosD(A)
#define PY(A) RSinD(A)
#define PZ(A) PlaceInX(A)
/* Compute Mollewide projection in pixel scale given latitude. */
#define NMollewide(y) \
  ((int)(sqrt((double)(180L*nScl*180L*nScl - 4L*(y)*nScl*(y)*nScl))+0.5))
/* Do settings indicate the current chart should have the info sidebar? */
#define fSidebar ((gi.nCurrChart == gWheel || gi.nCurrChart == gHouse || \
  gi.nCurrChart == gSector) && gs.fText && !us.fVelocity)
/* Is the current chart most properly displayed as a square graphic? */
#define fSquare \
  (gi.nCurrChart == gWheel || gi.nCurrChart == gHouse || gi.nCurrChart == gGrid || \
  (gi.nCurrChart == gHorizon && us.fPrimeVert) || gi.nCurrChart == gDisposit || \
  gi.nCurrChart == gOrbit || gi.nCurrChart == gGlobe || gi.nCurrChart == gPolar)
/* Does the current chart have to be displayed in a map rectangle? */
#define fMap \
  (gi.nCurrChart == gAstroGraph || gi.nCurrChart == gWorldMap)
/* Do settings indicate the current chart should have an outer border? */
#define fDrawBorder \
  ((gs.fBorder || gi.nCurrChart == gGrid) && gi.nCurrChart != gGlobe && \
  gi.nCurrChart != gPolar && (gi.nCurrChart != gWorldMap || !gs.fMollewide))
/* Do settings indicate current chart should have chart info at its bottom? */
#define fDrawText \
  (gs.fText && gi.nCurrChart != gEphemeris && gi.nCurrChart != gCalendar && gi.nCurrChart != gWorldMap && \
  gi.nCurrChart != gGlobe && gi.nCurrChart != gPolar && ((gi.nCurrChart != gWheel && \
  gi.nCurrChart != gHouse && gi.nCurrChart != gSector) || us.fVelocity))

#define SIZEOF_FIELD(structure, field) (sizeof(((structure *) 0)->field))
#define NUM_ELEMENTS(array)  (sizeof(array)/sizeof(*array))


#define PtZero(pt) pt.x = pt.y = pt.z = 0.0;
#define PtSet(pt, a, b, c) pt.x = a; pt.y = b; pt.z = c;
#define PtLen(pt) RLength3(pt.x, pt.y, pt.z)
#define PtMul(pt, r) pt.x *= (r); pt.y *= (r); pt.z *= (r);
#define PtDiv(pt, r) pt.x /= (r); pt.y /= (r); pt.z /= (r);
#define PtAdd2(pt, pt2) pt.x += pt2.x; pt.y += pt2.y; pt.z += pt2.z;
#define PtSub2(pt, pt2) pt.x -= pt2.x; pt.y -= pt2.y; pt.z -= pt2.z;
#define PtNeg2(pt, pt2) pt.x = -pt2.x; pt.y = -pt2.y; pt.z = -pt2.z;
#define PtVec(pt, pt1, pt2) pt = pt2; PtSub2(pt, pt1);
#define PtDot(pt1, pt2) (pt1.x*pt2.x + pt1.y*pt2.y + pt1.z*pt2.z)
#define PtCross(pt, pt1, pt2) pt.x = pt1.y*pt2.z - pt1.z*pt2.y; \
  pt.y = pt1.z*pt2.x - pt1.x*pt2.z; pt.z = pt1.x*pt2.y - pt1.y*pt2.x;




int kElemA[4] = { 9,3,13,2 };
int kAspA[cAspect + 1] = { 15,8,5,9,10,14,8,8,7,7,7,7,7,7,7,7,7,7,7 };
int kRainbowA[8] = { 15, 9, 3, 11, 10, 14, 12, 5 };
int kMainA[9] = { 0,15,7,8,1,2,6,3,13 };

//==================================================================================================

#define EquToLocal(Z, L, T) CoorXform(Z, L, T)
#define JulianDayFromTime(t) ((t)*36525.0+2415020.0)
#define Untropical(deg) ((deg) + is.rSid - us.rZodiacOffset)
#define Mon ciMain.mon
#define Day1 ciMain.day
#define Yea ciMain.yea
#define Tim ciMain.tim
#define Zon ciMain.zon
#define Dst ciMain.dst
#define Lon ciMain.lon
#define Lat ciMain.lat

#define cObjInt    uranHi
#define objMax     (NUMBER_OBJECTS)
#define cCnstl     88
#define cSector    36
#define cPart      177
#define cWeek      7
#define cColor     16
#define cRainbow   7
#define cRay       7
#define xFont      6
#define xFontT     (xFont * gi.nScaleText * gi.nScaleT)
#define yFontT     (yFont * gi.nScaleText * gi.nScaleT)
#define xSideT     (SIDESIZE * gi.nScaleText * gi.nScaleT)

#define nDegMax    360
#define nDegHalf   180
#define nLarge     9999999
#define zonLMT     24.0

#define kBlackB   kMainB[0]
#define kWhiteB   kMainB[1]
#define kLtGrayB  kMainB[2]
#define kDkGrayB  kMainB[3]
#define kMaroonB  kMainB[4]
#define kDkGreenB kMainB[5]
#define kDkCyanB  kMainB[6]
#define kDkBlueB  kMainB[7]
#define kMagentaB kMainB[8]

#define kRedB    kRainbowB[1]
#define kOrangeB kRainbowB[2]
#define kYellowB kRainbowB[3]
#define kGreenB  kRainbowB[4]
#define kCyanB   kRainbowB[5]
#define kBlueB   kRainbowB[6]
#define kPurpleB kRainbowB[7]

#define Max(v1, v2) ((v1) > (v2) ? (v1) : (v2))
#define Min(v1, v2) ((v1) < (v2) ? (v1) : (v2))
#define NSgn(n) ((n) < 0 ? -1 : (n) > 0)
#define RSgn2(r) ((r) < 0.0 ? -1.0 : 1.0)
#define FOdd(n) ((n) & 1)
#define FBetween(v, v1, v2) ((v) >= (v1) && (v) <= (v2))
#define ChCap(ch) ((ch) >= 'a' && (ch) <= 'z' ? (ch) - 'a' + 'A' : (ch))
#define ChUncap(ch) (FCapCh(ch) ? (ch) - 'A' + 'a' : (ch))
#define FCapCh(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define FUncapCh(ch) ((ch) >= 'a' && (ch) <= 'z')
#define FNumCh(ch) ((ch) >= '0' && (ch) <= '9')
#define NHex(ch) ((int)((ch) <= '9' ? (ch) - '0' : (ch) - 'a' + 10) & 15)
#define NMultDiv(n1, n2, n3) ((int)((long)(n1) * (n2) / (n3)))
#define Ratio(v1, v2, v3) ((v1) + ((v2) - (v1)) * (v3))
#define ZFromS(s) ((double)(((s)-1)*30))
#define SFromZ(r) (((int)(r))/30+1)
#define RFromD(r) ((r)/rDegRad)
#define DFromR(r) ((r)*rDegRad)
#define GFromO(o) ((rDegMax - (o))/10.0)
//#define GFromO(o) ((rDegMax - cp0.longitude[o])/10.0)
//#define RAbs(r) fabs(r)
#define NAbs(n) abs(n)
#define RFloor(r) floor(r)
#define Sq(n) ((n)*(n))
#define RSqr(r) sqrt(r)
#define RLog(r) log(r)
#define RLog10(r) (RLog(r) / rLog10)
#define RSin(r) sin(r)
#define RCos(r) cos(r)
#define RTan(r) tan(r)
#define RAtn(r) atan(r)
#define RAsin(r) asin(r)
#define RAcos(r) acos(r)
#define RTanD(r) RTan(RFromD(r))

typedef struct _ObjDraw {
	int obj;  /* The object to draw                  */
	int x;    /* Horizontal pixel position of object */
	int y;    /* Vertical pixel position of object   */
	int yg;   /* Vertical pixel position of glyph    */
	unsigned long kv;    /* Color to use when drawing object    */
	int f;   /* Whether to draw object at all       */
} ObjDraw;

#define cPlanet 18
#define cuspLo  (oCore+1)
#define cuspHi  (cuspLo+cSign-1)
#define cobLo   (moonsHi+1)
#define moonsLo (dwarfHi+1)
#define custLo  uranLo
#define custHi  cobHi
#define cobHi   (cobLo+cCOB-1)
#define moonsHi (moonsLo+cMoons-1)
#define dwarfLo (uranHi+1)
#define cMoons  27
#define cDwarf  9
#define cCOB    5
#define cMoons2 (cMoons + cCOB)
#define dwarfHi (dwarfLo+cDwarf-1)
#define oNorm   cobHi

#define FMoons(obj)   FBetween(obj, moonsLo, moonsHi)
int oNod = 16;
#define FNodal(obj)   FBetween(obj, oNod, oLil)
#define FGeo(obj)     ((obj) == oMoo || FNodal(obj))
#define FCust(obj)    FBetween(obj, custLo, custHi)
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

#define rSqr2      1.41421356237309504880
#define rSqr3      1.73205080756887729353
#define rPhi       1.61803398874989484820
#define rLog10     2.30258509299404568402
#define rLog101    4.61512051684125945088
#define rPi        3.14159265358979323846
#define rPi2       (rPi*2.0)
#define rPiHalf    (rPi/2.0)
#define rDegMax    360.0
#define rDegHalf   180.0
#define rDegQuad   90.0
#define rDegRad    (rDegHalf/rPi)
#define rEpoch2000 (-24.736467)
#define rMiToKm    1.609344
#define rFtToM     0.3048
#define rInToCm    2.54
#define rAUToKm    149597870.7
#define rLYToAU    63241.07708427
#define rPCToAU    206264.8062471
#define rDayInYear 365.24219
#define rEarthDist 149.59787
#define rJD2000    2451545.0
#define rAxis      23.44578889
#define rSmall     (1.7453E-09)
#define rLarge     10000.0
#define rInvalid   (1.23456789E-09)
#define rRound     0.5
#define RAsinD(r) DFromR(RAsin(r))
#define RAngleD(x, y) DFromR(RAngle(x, y))
#define RAtnD(r) DFromR(RAtn(r))

// Return the minimum great circle distance between two sets of spherical
// coordinates. This is like MinDistance() but takes latitude into account.
#define RAbs(r) fabs(r)
#define RAcosD(r) DFromR(RAcos(r))

#define RGBVal(r, g, b) \
  ((unsigned long)(0xff000000 | ((unsigned long)(r) << 16) | ((unsigned long)(g) << 8) | (unsigned long)(b)))

int cSign = 12;
char* szWheelX[4 + 1] = { NULL, NULL, NULL, NULL, NULL };
const CP* rgpcp[5] = { &cp0, &cp1, &cp2, &cp3, &cp4 };
//tSignName 原来位配置文件取 默认从 1 开始  改为数组后 下标从 0 开始，代码保持一致，定义数组时默认  0 位置不使用为空
const char* tObjName[] = {"Earth", "Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", "Chiron", "Ceres", "Pallas Athena", "Juno", "Vesta", "North Node", "South Node", "Lilith", "Part Fortune", "Vertex", "East Point", "Ascendant", "2nd Cusp", "3rd Cusp", "IC ", "5th Cusp", "6th Cusp", "Descendant", "8th Cusp", "9th Cusp", "Midheaven", "11th Cusp", "12th Cusp", "Cupido", "Hades", "Zeus", "Kronos", "Apollon", "Admetos", "Vulkanus", "Poseidon", "Proserpina", "Achernar", "Polaris", "Zeta Retic.", "Alcyone", "Aldebaran", "Capella", "Rigel", "Bellatrix", "Elnath", "Alnilam", "Betelgeuse", "Menkalinan", "Mirzam", "Canopus", "Alhena", "Sirius", "Adhara", "Wezen", "Castor", "Procyon", "Pollux", "Suhail", "Avior", "Miaplacidus", "Alphard", "Regulus", "Dubhe", "Acrux", "Gacrux", "Mimosa", "Alioth", "Spica", "Alkaid", "Agena", "Arcturus", "Toliman", "Antares", "Shaula", "Sargas", "Kaus Austr.", "Vega", "Altair", "Peacock", "Deneb", "Alnair", "Fomalhaut", "Andromeda", "Alpheratz", "Algenib", "Schedar", "Mirach", "Alrischa", "Almac", "Algol", "Mintaka", "Wasat", "Acubens", "Merak", "Vindemiatrix", "Mizar", "Kochab", "Zuben Elgen.", "Zuben Escha.", "Alphecca", "Unuk Alhai", "Ras Alhague", "Albireo", "Alderamin", "Nashira", "Skat", "Scheat", "Markab", "Gal.Center", "Apex", "Sadalmelik" };
const char* tObjShortName[]= {"Earth","Sun","Moon","Mercury","Venus","Mars","Jupiter","Saturn","Uranus","Neptune","Pluto","Chiron","Ceres","Pallas","Juno","Vesta","NoNode","SoNode","Lilith","Fortune","Vertex","EaPoint","Ascendant","2nd Cusp","3rd Cusp","IC ","5th Cusp","6th Cusp","Descendant","8th Cusp","9th Cusp","Midheaven","11th Cusp","12th Cusp","Cupido","Hades","Zeus","Kronos","Apollon","Admetos","Vulkanus","Poseidon","Proserpina","Achernar","Polaris","Zeta Retic.","Alcyone","Aldebaran","Capella","Rigel","Bellatrix","Elnath","Alnilam","Betelgeuse","Menkalinan","Mirzam","Canopus","Alhena","Sirius","Adhara","Wezen","Castor","Procyon","Pollux","Suhail","Avior","Miaplacidus","Alphard","Regulus","Dubhe","Acrux","Gacrux","Mimosa","Alioth","Spica","Alkaid","Agena","Arcturus","Toliman","Antares","Shaula","Sargas","Kaus Austr.","Vega","Altair","Peacock","Deneb","Alnair","Fomalhaut","Andromeda","Alpheratz","Algenib","Schedar","Mirach","Alrischa","Almac","Algol","Mintaka","Wasat","Acubens","Merak","Vindemiatrix","Mizar","Kochab","Zuben Elgen.","Zuben Escha.","Alphecca","Unuk Alhai","Ras Alhague","Albireo","Alderamin","Nashira","Skat","Scheat","Markab","4","7","10"};
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
const char* szPerson[]={"This person"};
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
const char* sefstarsName = "sefstars.txt";
const char* tHouseSystem[] = { "","Placidus", "Koch", "Equal(Asc)", "Campanus", "Meridian", "Regiomontanus", "Porphyry", "Morinus", "Topocentric", "Alcabitius", "Equal(MC)", "Neo-Porphyry", "Whole", "Vedic", "Null", "Shripati" };
const char* tColor[] = {"Color  1", "Color 11", "Color 12", "Color 14", "Color 13", "Color 15", "Color 16", "Color 10", "Color  9", "Color  3", "Color  4", "Color  6", "Color  7", "Color  5", "Color  8", "Color  2"};
const char* sNoAspects = "No Aspects in list!";
const char* sTotalPower = "Total Power: %.2f - Average Power: %.2f\n";
//==================================================================================================

//==================================================================================================

const char* szHouseSystem[NUMBER_OF_HOUSE_SYSTEMS] = {
	"Placidus", "Koch", "Equal(Asc)", "Campanus", "Meridian",
	"Regiomontanus", "Porphyry", "Morinus", "Topocentric", "Alcabitius",
	"Equal(MC)", "Neo-Porphyry", "Whole", "Vedic", "Null", "Shripati"
};

const char* szAspectName[cAspect + 1] = { "",
	"Conjunct", "Opposite", "Square", "Trine", "Sextile",
	"Inconjunct", "Semisextile", "Semisquare", "Sesquiquadrate",
	"Quintile", "Biquintile",
	"Semiquintile", "Septile", "Novile",
	"Binovile", "Biseptile", "Triseptile", "Quatronovile"
};

wchar_t szAspectAbbrev[cAspect + 1][10] = { L"",
	L"Con", L"Opp", L"Squ", L"Tri", L"Sex",
	L"Inc", L"SSx", L"SSq", L"Ses", L"Qui", L"BQn",
	L"SQn", L"Sep", L"Nov", L"BNv", L"BSp", L"TSp", L"QNv"
};

// //char as[19][5]={"","合","冲","刑","拱","六分","梅花","四合","半刑","补八","五分","倍五","十分","七分","九分","九二","七二","三分","九四"};
wchar_t szAspectAbbrevC[cAspect + 1][10] = { L"",
	L"合", L"冲", L"刑", L"拱", L"六分",
	L"梅花", L"四合", L"半刑", L"补八",
	L"五分", L"倍五",
	L"十分", L"七分", L"九分",
	L"九二", L"七二", L"三分", L"九四"
};

const char* szAspectAbbrevCon = "Con";
const char* szAspectAbbrevOpp = "Opp";

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

const char* ayanamsha_short_name[LAST_SIDEREAL_MODE + 1] =
{
	"Fagan/Bradley",
	"Lahiri",
	"De Luce",
	"Raman",
	"Ushashashi",
	"Krishnamurti",
	"Djwhal Khul",
	"Yukteshwar",
	"J. N. Bhasin",
	"Babyl. (Kugler 1)",
	"Babyl. (Kugler 2)",
	"Babyl. (Kugler 3)",
	"Babyl. (Huber)",
	"Babyl. (Eta Piscium)",
	"Babyl. (Ald. 15 Tau)",
	"Hipparchos",
	"Sassanian",
	"Galac. Center 0 Sag",
};

// text that will be displayed in the About dialogue box
// Note: we need carriage return line feed sequences ("\n") for Windows to display
// correctly a new line (just '\n' is not enough).

#ifdef ARABIC
AI ai[cPart] = {
	{L"    02 01F ", L"Fortune"},
	{L"    01 02F ", L"Spirit"},
	{L"    06 S F ", L"Victory"},
	{L"    F  05F ", L"Valor & Bravery"},
	{L"    05 03Fh", L"Mind & Administrators"},
	{L"   h02r02Fh", L"Property & Goods"},
	{L"    06 07 h", L"Siblings"},
	{L"   j03 01Fh", L"Death of Siblings"},
	{L"    06 07Fh", L"Death of Parents"},
	{L"    07h02Fh", L"Grandparents"},
	{L"    06 03Fh", L"Real Estate"},
	{L"    07 06Fh", L"Children & Life"},
	{L"   R02 02 h", L"Expected Birth"},
	{L"    05 03 h", L"Disease & Defects (1)"},
	{L"    05 07 h", L"Disease & Defects (2)"},
	{L"   R07 07 h", L"Captivity"},
	{L"    02 03 h", L"Servants"},
	{L"   h07 04 h", L"Partners"},
	{L"   h08 02 h", L"Death"},
	{L"    05 07Fh", L"Sickness & Murder"},
	{L"    03 07Fh", L"Danger, Violence & Debt"},
	{L"   h09r09 h", L"Journeys"},
	{L"   105 07Fh", L"Travel by Water"},
	{L"    03 02Fh", L"Faith, Trust & Belief"},
	{L"    02 07Fh", L"Deep Reflection"},
	{L"    01 07Fh", L"Understanding & Wisdom"},
	{L"    06 01Fh", L"Fame & Recognition"},
	{L"    02 05Fh", L"Rulers & Disassociation"},
	{L"    07 01Fh", L"Father, Fate & Karma"},	/* Combust */
	{L"    F  07Fh", L"Sudden Advancement"},
	{L"    01 07 h", L"Celebrity of Rank"},
	{L"    07 05Fh", L"Surgery & Accident"},
	{L"    04 03Fh", L"Merchants & Their Work"},
	{L"    F  S Fh", L"Merchandise (Exchange)"},	/* Moon */
	{L"    02 04Fh", L"Mother"},
	{L"    S  F Fh", L"Glory & Constancy"},
	{L"    01 F Fh", L"Honorable Acquaintances"},
	{L"    06 F Fh", L"Success"},
	{L"    04 F Fh", L"Worldliness"},
	{L"    03 02 h", L"Acquaintances"},
	{L"    03 S  h", L"Violence"},
	{L"    01 03Fh", L"Liberty of Person"},
	{L"    04 06Fh", L"Praise & Acceptance"},
	{L"   h12r12 h", L"Enmity"},
	{L"    F  S  h", L"Bad Luck"},
	{L"    05 F F ", L"Debilitated Bodies"},
	{L"    02D  F ", L"Boldness & Violence"},
	{L"    S  03F ", L"Trickery & Deceit"},
	{L"   h03 05  ", L"Necessities"},
	{L"    03 F   ", L"Realization of Needs"},
	{L"    01 05F ", L"Retribution"},
	{L"    06 02  ", L"Children (Male)"},
	{L"    04 02  ", L"Children (Female)"},
	{L"    05 04  ", L"Play & Variety"},	/* Change */
	{L"    07 03 h", L"Stability"},
	{L"   h05 06Fh", L"Speculation"},
	{L"    03 04Fh", L"Art"},
	{L"   h05r05 h", L"Sexual Attraction"},
	{L"    10 04 h", L"Sex Drive & Stimulation"},
	{L"    05 01 h", L"Passion"},
	{L"    05 04 h", L"Emotion & Affection"},
	{L"   r08 07 h", L"Most Perilous Year"},
	{L"   h08 07 h", L"Peril"},
	{L"    09 08 h", L"Occultism"},
	{L"    03 01 h", L"Commerce"},
	{L"h09h03 04 h", L"Marriage Contracts"},
	{L"   h09r09 h", L"Travel by Land"},
	{L"    08H08 h", L"Travel by Air"},
	{L" 30 01 02Fh", L"Destiny"},
	{L" 30 02 01Fh", L"Vocation & Status"},
	{L"   019 01 h", L"Honor, Nobility (Day)"},
	{L"   033 02 h", L"Honor, Nobility (Night)"},
	{L"    10 01 h", L"Organization"},
	{L"    04h07 h", L"Divorce"},
	{L"    08 01 h", L"Ostracism & Loss"},
	{L"    02 08Fh", L"Friends"},
	{L"    07 01 h", L"Tragedy & Brethren"},
	{L"    02 10Fh", L"Race (Consciousness)"},
	{L"    02D02Fh", L"Bondage & Slavery"},
	{L"    F  09 h", L"Imprisonment & Sorrow"},
	{L"    04 08 h", L"Perversion"},
	{L"   h12 09 h", L"Self-Undoing"},
	{L"    09 01 h", L"Treachery & Entrapment"},
	{L"h12r12 09 h", L"Bereavement"},
	{L"    06h12 h", L"Suicide (Yang)"},
	{L"   h08 09 h", L"Suicide (Yin)"},
	{L"    06 09 h", L"Depression"},
	{L" 05 09 08 h", L"Assassination (Yang)"},
	{L"   r12 09 h", L"Assassination (Yin)"},
	{L"    09 06  ", L"Cancer (Disease)"},
	{L"    08 07  ", L"Catastrophe"},
	{L"    07 08  ", L"Foolhardiness"},
	{L"    03 05  ", L"Release & Luck"},
	{L"    06 03  ", L"Benevolence & Assurance"},
	{L"    03 06  ", L"Hope & Sensitivity"},
	{L"    03 07  ", L"Aptness & Aloofness"},
	{L"    08 09  ", L"Charm & Personality"},
	{L"    02 03F ", L"Faith & Good Manners"},
	{L"    01 03  ", L"Temperament"},
	{L"    04 03  ", L"Security & Treasure"},
	{L"    08 03  ", L"Originality"},
	{L"    03 08  ", L"Eccentricity, Astrology"},
	{L"    09 03  ", L"Divination"},
	{L"    03 09  ", L"Intrusion"},
	{L"    05 06  ", L"Negotiation"},
	{L"    06 05  ", L"Discord & Controversy"},
	{L"    05 08  ", L"Coincidence"},
	{L"    08 05  ", L"Unpreparedness"},
	{L"    05 09  ", L"Popularity"},
	{L"    09 05  ", L"Misunderstanding"},
	{L"    04 06  ", L"Sentiment & Marriage"},
	{L"    06 04  ", L"Loneliness"},
	{L"    04 07  ", L"Success in Investment"},
	{L"    07 04  ", L"Frugality & Labor"},
	{L"    08 04  ", L"Wastefulness"},
	{L"    04 09  ", L"Vanity"},
	{L"    09 04  ", L"Corruptness"},
	{L"    01 05  ", L"Initiative"},
	{L"    05 02F ", L"Memory"},
	{L"    04 01  ", L"Love, Beauty & Peace"},
	{L"    01 04  ", L"Disinterest & Boredom"},
	{L"    01 06  ", L"Accomplishment"},
	{L"    07 02F ", L"Influence"},
	{L"    06 01  ", L"Increase & Impression"},
	{L"    09 07  ", L"Caution"},
	{L"    07 09  ", L"Timidity"},
	{L"    08 06  ", L"Entertainment"},
	{L"    06 08  ", L"Bequest"},
	{L"    01 09  ", L"Genius"},
	{L"    02 09F ", L"Revelation"},
	{L"    09 02F ", L"Delusion"},
	{L"    08 02F ", L"Misinterpretation"},
	{L"    01 08  ", L"Intellectuality"},
	{L"    06 07 E", L"Earth"},
	{L"    04 02 E", L"Water"},
	{L"   r04 03 E", L"Air & Wind"},
	{L"    05 01 E", L"Fire"},
	{L"    07 05FE", L"Clouds"},
	{L"    04 02FE", L"Rains"},
	{L"    07 03FE", L"Cold"},
	{L"    06 01FC", L"Wheat"},
	{L"    06 02FC", L"Barley & Meats"},
	{L"    04 06FC", L"Rice & Millet"},
	{L"    07 06FC", L"Corn"},
	{L"    07 05FC", L"Lentils, Iron, Pungents"},
	{L"    05 07FC", L"Beans & Onions"},
	{L"    01 03FC", L"Chick Peas"},
	{L"    04 07FC", L"Sesame & Grapes"},
	{L"    03 04FC", L"Sugar & Legumes"},
	{L"    01 02FC", L"Honey"},
	{L"    02 05FC", L"Oils"},
	{L"    04 05FC", L"Nuts & Flax"},
	{L"    02 03FC", L"Olives"},
	{L"    05 07FC", L"Apricots & Peaches"},
	{L"    03 06FC", L"Melons"},
	{L"    05 02FC", L"Salt"},
	{L"    03 01FC", L"Sweets"},
	{L"    07 03FC", L"Astrigents & Purgatives"},
	{L"    04 03FC", L"Silk & Cotton"},
	{L"    05 07FC", L"Purgatives (Bitter)"},
	{L"    06 07FC", L"Purgatives (Acid)"},
	{L"    30D   H", L"Secrets"},
	{L"    02 03FH", L"Information True/False"},
	{L"    F D  FH", L"Injury to Business"},
	{L" 03 07 06 H", L"Freedmen & Servants"},
	{L" 02 07 06 H", L"Employers"},
	{L"   h07 04 H", L"Marriage"},
	{L"    06 01 H", L"Time for Action/Success"},
	{L"    07 01 H", L"Time Occupied in Action"},
	{L" 07 06 01 H", L"Dismissal & Resignation"},
	{L"    05 02 H", L"Life/Death of Absentee"},
	{L"    05 01 H", L"Lost Animal (Light)"},
	{L"    05 07 H", L"Lost Animal (Dark)"},
	{L"    03 05 H", L"Lawsuit"},
	{L"h08 05 02 H", L"Decapitation"},
	{L" 30 07 02 H", L"Torture"},
	{L" 02h04D   H", L"Lost Objects"}
};
#endif /* ARABIC */




typedef struct 
{
	char	t1[20];
	char	t2[20];
	char	t3[20];
	double   zone;
}timezoneArr ;

timezoneArr ta[86]={
{"AHST","AHT","",+10.00},
{"AHDT","AHT/S","",+09.00},
{"AHWT","AHT/S","",+09.00},
{"AST","AST","",+04.00},
{"ADT","ASTS/S","",+03.00},
//{"AWT","AST/D","",+03.00},
//{"ACST","ACST","",-9.50},
{"ACDT","ACST/S","",-10.50},
//{"AEST","AEST","",-10.00},
{"AEDT","AEST/S","",-11.00},
//{"ASST","ASST","",-10.50},
//{"ASDT","ASST/S","",-11.50},
{"AWST","AWST","",-08.00},
{"AWDT","AWST/S","",-09.00},
{"AZT","AZT","",+02.00},
{"AZDT","AZT/S","",+01.00},
{"AZWT","AZT/S","",+01.00},
{"BGT","BGT","",-03.00},
{"BGDT","BGT/S","",-04.00},
{"BZT","BZT","",+03.00},
{"BZDT","BZT/S","",+02.00},
//{"CST","CST","",+06.00},
//{"CDT","CST/S","",+05.00},
{"CWT","CST/S","",+05.00},
{"EET","EET'","",-02.00},
{"EEDT","EET/S","",-03.00},
{"EEWT","EET/S","",-03.00},
{"EST","EST","",+05.00},
{"EDT","EST/S","",+04.00},
{"EWT","EST/S","",+04.00},
{"GMT/UT","WET/UT","",+00.00},
{"GDT","WET/S","",-01.00},
{"GDT","WET/S","",-01.00},
{"GDWT","WET/DS","",-02.00},
//{"HST","HST","",+10.50},
{"HDT","HST/S","",+09.50},
{"HWT","HST/S","",+09.50},
//{"IST","IST","",-5.50},
//{"IDT","IST/S","",-6.50},
{"IWT","IST/S","",-6.50},
{"IDLE","IDLE'","",-12.00},
{"IDLW","IDLW","",+12.00},
{"IRT","IRT","",-3.50},
{"IRDT","IRT/S","",-4.50},
{"JST","JST","",-09.00},
{"JDT","JST/S","",-10.00},
{"JWT","JST/S","",-10.00},
{"JVT","JVT","",-7.50},
{"JVDT","JVT/S","",-8.50},
//{"KET","KET","",-03.00},
//{"MET","MET","",-01.00},
{"MEDT","MET/S","",-02.00},
{"MEWT","MET/S","",-02.00},
{"MDST","MET/DS","",-03.00},
{"MST","MST","",+07.00},
//{"MDT","MST/S","",+06.00},
{"MWT","MST/S","",+06.00},
{"NZT","NZT","",-12.00},
{"NZDT","NZT/S","",-13.00},
{"NFT","NFT","",+03.50},
{"NFDT","NFT/S","",+02.50},
{"NFWT","NFT/S","",+02.50},
{"NAT","NAT","",+11.00},
//{"NST","NST","",-6.50},
{"NSDT","NST/S","",-7.50},
{"ONZT","OZT","",-11.50},
{"ONZD","OZT/S","",-12.50},
//{"PST","PST","",+08.00},
{"PDT","PST/S","",+07.00},
{"PWT","PST/S","",+07.00},
{"R3T","R3T","",-04.00},
{"R3DT","R3T/S","",-05.00},
{"R3WT","R3T/S","",-05.00},
{"R4T","R4T","",-05.00},
{"R4DT","R4T/S","",-06.00},
{"R4WT","R4T/S","",-06.00},
{"R5T","R5T","",-06.00},
{"R5DT","R5T/S","",-07.00},
{"R5WT","R5T/S","",-07.00},
{"R11S","Z11","",-11.00},
{"R11D","Z11/S","",-12.00},
{"R11W","Z11/S","",-12.00},
{"SST","SST","",-07.00},
{"SSDT","SST/S","",-08.00},
//{"WAT","WAT","",+01.00},
//{"WADT","WAT/S","",+01.00},
{"YST","YST","",+09.00},
{"YDT","YST/S","",+08.00},
{"YWT","YST/S","",+08.00},
//{"CST","MET","MET",-01.00},
//{"CDT","MEDT","MET/S",-02.00},
{"CWT","MEWT","MET/S",-02.00},
{"CCT","AWST","CCT",-08.00},
{"CCDT","AWDT","CCT/S",-09.00},
{"GMST","AEST","",-10.00},
{"RZ1","EET","EET",-02.00},
{"RZ2","BGT","BGT",-03.00},
{"RZ6","SST","SST",-07.00},
//{"SAT","ACST","ACST",-9.50},
{"SDT","ACDT","ACDT",-10.50},
{"WET","GMT","WET",00.00},
{"WEDT","GDT","WET/S",-01.00},
{"WEWT","GWT","WET/S",-01.00},
{"WDST","GDWT","WET/DS",-02.00},
{"NZST","","",-12.00}
};

//#include "shlobj.h"		// for SHBrowseForFolder
BOOL FCreateGridA(BOOL fFlip);
void ComputeInfluence();
double GetOrbA(int obj1, int obj2, int asp);
void PrintInDayEvent(int source, int aspect, int dest, int nVoid);

int rgobjList[118];
#define RObjInf(i) rObjInf[Min(i, cLastMoving)]
#define RObjInfA(i) rObjInfA[Min(i, cLastMoving)]
#define FCmSwissAny() (us.fEphemFiles && !us.fPlacalcPla)
double power1[118], power2[118];

enum _displayformat {
  dfZod = 0,  // Zodiac Position
  dfHM  = 1,  // Hours & Minutes
  df360 = 2,  // 360 Degrees
  dfNak = 3,  // 27 Nakshatras
};


class Options
{
public:
	int FIXSTARS;
	int ANTIS;
	int CANTIS;

	int DODECATEMORIA;

	int MOON;
	int ABOVEHOR;
	int ABOVEHORNATAL;

	BOOL	aspects;
	BOOL	aspect[11];
	BOOL	symbols;
	BOOL	traditionalaspects;
	BOOL	houses;
	BOOL	positions;
	BOOL	intables;
	BOOL	bw;
	int	theme;
	int	ascmcsize;
	double	tablesize;
	BOOL	planetarydayhour;
	BOOL	housesystem;
	BOOL	transcendental[3];
	BOOL	shownodes;
	BOOL	aspectstonodes;
	BOOL	showlof;
	BOOL	showaspectstolof;
	BOOL	showterms;
	BOOL	showdecans;
	int showfixstars;
	BOOL	showfixstarsnodes;
	BOOL    showfixstarshcs;
	BOOL showfixstarslof;
	BOOL topocentric;
	BOOL usetradfixstarnamespdlist;
	BOOL netbook;

	//#AppearanceII
	BOOL	speculums[2][14];
	BOOL	intime;

	//#Symbols
	BOOL uranus;
	int pluto;
	BOOL signs;

	//#Dignities(planets, domicile, exaltatio)
						//#Sun
	BOOL dignities[10][2][12];

	//#Minor dignities
	//#Triplicities
	int	seltrip;
	int	trips[3][4][3];

	//#Terms
	int	selterm;

	int	terms[2][12][5][2];

	//#Decans
	int	seldecan;
	int	decans[2][12][3];

	//#ChartAlmuten
	BOOL oneruler;
	BOOL usedaynightorb;
	int dignityscores[5];
	BOOL useaccidental;
	int housescores[12];
	int sunphases[3];
	int dayhourscores[2];
	BOOL  useexaltationmercury;

	//#TopicalAlmuten && Parts
	int topicals;
	//#Arabic Parts
	int	arabicpartsref;
	int	daynightorbdeg;
	int	daynightorbmin;
	int	arabicparts;

	//#Ayanamsha
//int	ayanamsha;

	//#Colors


	//#Housesystem
	int hsys;

	//#Nodes
	BOOL meannode;

	//#Orbis
	double	orbis[11][11];
	double	orbisplanetspar[11][2];

	//# Houses 
	double	orbisH[11];
	double	orbisparH[2];//#parallel/contraparallel
	double	orbiscuspH;

	//# Asc,MC
	double	orbisAscMC[11];
	double	orbisparAscMC[2];
	double	orbiscuspAscMC;

	double	exact;

	//#Primary Dirs
	int primarydir;// = PLACIDIANSEMIARC
	int subprimarydir;// = MUNDANE
	int subzodiacal;// = SZNEITHER
	BOOL bianchini;

	BOOL	sigascmc[2];
	BOOL	sighouses;
	BOOL	sigplanets[12];
	BOOL	promplanets[16];
	BOOL	pdaspects[11];
	BOOL	pdmidpoints;
	BOOL	pdparallels[2];
	BOOL	pdsecmotion;
	int	pdsecmotioniter;

	BOOL	zodpromsigasps[2];
	BOOL	ascmchcsasproms;// Asc, MC as promissors

	BOOL	pdfixstars;
	BOOL	pdfixstarssel[30];
	BOOL	pdlof[2];

	BOOL	pdsyzygy;

	BOOL	pdterms;

	BOOL	pdantiscia;
	BOOL pdcustomer;
	int	pdcustomerlon[3];
	int		pdcustomerlat[3];
	BOOL pdcustomersouthern;

	BOOL pdcustomer2;
	int	pdcustomer2lon[3];
	int		pdcustomer2lat[3];
	BOOL pdcustomer2southern;

	//#PD-keys
	BOOL	pdkeydyn;
	int	pdkeyd;
	int	pdkeys;
	int	pdkeydeg;
	int	pdkeymin;
	int	pdkeysec;
	BOOL	useregressive;

	//#Lot of Fortune
	int	lotoffortune;

	//#Syzygy
	int syzmoon;

	//#'''total30     昂宿六       毕宿五       大陵五      天大将军一     心宿二       大角星       鬼宿四      鬼宿三       摇光        参宿四      南门二       老人星      北河二      五帝座一     北落师门      贯索四      室宿一      奎宿九      北极星       北河三      鬼(宿)星团  南河三      轩辕十四     参宿七      天狼星        角宿一      天市右垣七   织女一      氐宿一         氐宿四'''
	char fixstars[30][10];// = {'etTau':1.5, 'alTau':1.5, 'bePer':1.5, 'ga-1And':1.5, 'alSco':1.5, 'alBoo':1.5, 'deCnc':1.5, 'gaCnc':1.5, 'etUMa':1.5, 'alOri':1.5, 'alCen':1.5, 'alCar':1.5, 'alGem':1.5, 'beLeo':1.5, 'alPsA':1.5, 'alCrB':1.5, 'alPeg':1.5, 'beAnd':1.5, 'alUMi':1.5, 'beGem':1.5, 'M44':1.5, 'alCMi':1.5, 'alLeo':1.5, 'beOri':1.5, 'alCMa':1.5, 'alVir':1.5, 'alSer':1.5, 'alLyr':1.5, 'al-2Lib':1.5, 'beLib':1.5}


	//#Profections
	BOOL zodprof;
	BOOL usezodprojsprof;

	BOOL isfirbonatti;

	char deflocname[260];
	BOOL deflocplus;
	int defloczhour;
	int defloczminute;
	BOOL deflocdst;
	int  defloclondeg;
	int  defloclonmin;
	int  defloclatdeg;
	int  defloclatmin;
	BOOL defloceast;
	BOOL deflocnorth;
	int 	 deflocalt;

	//##PDsInChart
	int  pdincharttyp;
	BOOL pdinchartsecmotion;

	BOOL pdinchartterrsecmotion;

	//#Languages
	int  langid;

	BOOL	autosave;
	long rgbbmp[16];
	int kAspA[19];
	int kElemA[4];

	char myBuff[64];
	int size;
	int cWeight;
	unsigned long  bItalic;

	char	optionsfilestxt[25][120];
	char	optsdirtxt[120];// = 'Opts'
	Options() {};
	BOOL step12;
	BOOL mainsigs;
	BOOL FROMPDSSETTING;
	BOOL USEBIRTH;
	BOOL fEquator;
	BOOL hasToolbar;
	BOOL bStatusBar;
	BOOL fInverse;
	BOOL UseAspectGrid;
	BOOL HarmonicChartsYearsMode;
	double ayanamsha;
	BOOL NoNutation;
	BOOL NoPrecession;
	BOOL AddNutation;
	BOOL IsChaldean;
	int Dignities;
	BOOL IsPlanetModern;
	BOOL IsPlanetTraditional;
	BOOL IsPlanetLuminaries;
	int orbs;
	int progdirrestric;
	int UseTheme;
	BOOL zregyptian;
	BOOL zr27cap;
	int ZodRelsign;
	int ZodRelperiod;
	int PlanetOrder;
	int StartWith;
	BOOL sectptolemy;
	void OptionsInit()
	{
		USEBIRTH = TRUE;
		FROMPDSSETTING = FALSE;
		step12 = TRUE;
		mainsigs = FALSE;
		FIXSTARS = 1;
		ANTIS = 2;
		CANTIS = 3;
		DODECATEMORIA = 4;
		MOON = 0;
		ABOVEHOR = 1;
		ABOVEHORNATAL = 2;
		aspects = TRUE;

		BOOL aspect1[11] = { TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE };
		memcpy(aspect, aspect1, sizeof(aspect1));
		symbols = TRUE;
		traditionalaspects = FALSE;
		houses = TRUE;
		positions = FALSE;
		intables = FALSE;
		bw = FALSE;
		theme = 0;
		ascmcsize = 5;
		tablesize = 0.75;
		planetarydayhour = TRUE;
		housesystem = TRUE;
		transcendental[0] = transcendental[1] = transcendental[2] = TRUE;

		shownodes = TRUE;
		aspectstonodes = FALSE;
		showlof = TRUE;
		showaspectstolof = FALSE;
		showterms = FALSE;
		showdecans = FALSE;
		showfixstars = 0;
		showfixstarsnodes = FALSE;
		showfixstarshcs = FALSE;
		showfixstarslof = FALSE;
		topocentric = FALSE;
		usetradfixstarnamespdlist = FALSE;
		netbook = FALSE;

		BOOL	speculums1[2][14] = {
			{TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE},
			{TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE}
		};
		memcpy(speculums, speculums1, sizeof(speculums1));

		intime = FALSE;
		intime;
		uranus = TRUE;
		pluto = 0;
		signs = TRUE;
		BOOL dignities1[10][2][12] = {
			{{FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}, {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}},
			{{FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}, {FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}},
			{{FALSE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}, {FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}},
			{{FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE}, {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE}},
			{{TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE}, {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE}},
			{{FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE}, {FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}},
			{{FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE}, {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE}},
			{{FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}, {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}},
			{{FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}, {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}},
			{{FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}, {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}} };

		memcpy(dignities, dignities1, sizeof(dignities1));

		seltrip = 0;

		int trips1[3][4][3] = { {{0, 5, 6},{6, 2, 5},{3, 4, 1},{3, 1, 4}},
						{{0, 5, 7},{6, 2, 7},{4, 4, 7},{3, 1, 7}},
						{{0, 4, 5},{6, 3, 2},{5, 1, 4},{2, 6, 3}} };
		memcpy(trips, trips1, sizeof(trips1));

		selterm = 0;

		int terms1[2][12][5][2] = { {{{5, 6}, {3, 6}, {2, 8}, {4, 5}, {6, 5}},
					{{3, 8}, {2, 6}, {5, 8}, {6, 5}, {4, 3}},
					{{2, 6}, {5, 6}, {3, 5}, {4, 7}, {6, 6}},
					{{4, 7}, {3, 6}, {2, 6}, {5, 7}, {6, 4}},
					{{5, 6}, {3, 5}, {6, 7}, {2, 6}, {4, 6}},
					{{2, 7}, {3, 10}, {5, 4}, {4, 7}, {6, 2}},
					{{6, 6}, {2, 8}, {5, 7}, {3, 7}, {4, 2}},
					{{4, 7}, {3, 4}, {2, 8}, {5, 5}, {6, 6}},
					{{5, 12}, {3, 5}, {2, 4}, {6, 5}, {4, 4}},
					{{2, 7}, {5, 7}, {3, 8}, {6, 4}, {4, 4}},
					{{2, 7}, {3, 6}, {5, 7}, {4, 5}, {6, 5}},
					{{3, 12}, {5, 4}, {2, 3}, {4, 9}, {6, 2}}},
					{{{5, 6}, {3, 8}, {2, 7}, {4, 5}, {6, 4}},
					{{3, 8}, {2, 7}, {5, 7}, {6, 2}, {4, 6}},
					{{2, 7}, {5, 6}, {3, 7}, {4, 6}, {6, 4}},
					{{4, 6}, {5, 7}, {2, 7}, {3, 7}, {6, 3}},
					{{5, 6}, {2, 7}, {6, 6}, {3, 6}, {4, 5}},
					{{2, 7}, {3, 6}, {5, 5}, {6, 6}, {4, 6}},
					{{6, 6}, {3, 5}, {2, 5}, {5, 8}, {4, 6}},
					{{4, 6}, {3, 7}, {5, 8}, {2, 6}, {6, 3}},
					{{5, 8}, {3, 6}, {2, 5}, {6, 6}, {4, 5}},
					{{3, 6}, {2, 6}, {5, 7}, {6, 6}, {4, 5}},
					{{6, 6}, {2, 6}, {3, 8}, {5, 5}, {4, 5}},
					{{3, 8}, {5, 6}, {2, 6}, {4, 5}, {6, 5}}} };
		memcpy(terms, terms1, sizeof(terms1));

		seldecan = 0;

		int decans1[2][12][3] = { {{4, 0, 3},
						{2, 1, 6},
						{5, 4, 0},
						{3, 2, 1},
						{6, 5, 4},
						{0, 3, 2},
						{1, 6, 5},
						{4, 0, 3},
						{2, 1, 6},
						{5, 4, 0},
						{3, 2, 1},
						{6, 5, 4}},
						{{4, 0, 5},
						{3, 2, 6},
						{2, 3, 6},
						{1, 4, 5},
						{0, 5, 4},
						{2, 6, 3},
						{3, 6, 2},
						{4, 5, 1},
						{5, 4, 0},
						{6, 3, 2},
						{6, 2, 3},
						{5, 1, 4}} };
		memcpy(decans, decans1, sizeof(decans1));

		oneruler = TRUE;
		usedaynightorb = FALSE;

		int dignityscores1[5] = { 5, 4, 3, 2, 1 };
		memcpy(dignityscores, dignityscores1, sizeof(dignityscores1));

		useaccidental = TRUE;

		int housescores1[12] = { 12, 6, 3, 9, 7, 1, 10, 5, 4, 11, 8, 2 };
		memcpy(housescores, housescores1, sizeof(housescores1));

		int sunphases1[3] = { 3, 2, 1 };
		memcpy(sunphases, sunphases1, sizeof(sunphases1));

		int dayhourscores1[2] = { 7, 6 };
		memcpy(dayhourscores, dayhourscores1, sizeof(dayhourscores1));

		useexaltationmercury = FALSE;
		topicals = -1;
		arabicpartsref = 0;
		daynightorbdeg = 0;
		daynightorbmin = 0;
		arabicparts = -1;
		//ayanamsha = 0;

		//#Housesystem
		hsys = 'P';

		//#Nodes
		meannode = TRUE;

		double orbis1[11][11] = {
			{5.0, 1.75, 1.75, 3.0, 1.75, 4.0, 4.0, 1.75, 1.75, 1.75, 5.0},//sun
			{5.0, 1.75, 1.75, 3.0, 1.75, 4.0, 4.0, 1.75, 1.75, 1.75, 5.0},//moon
			{3.5, 1.5, 1.5, 2.5, 1.5, 3.0, 3.0, 1.5, 1.5, 1.5, 3.5},//mercury 
			{3.5, 1.5, 1.5, 2.5, 1.5, 3.0, 3.0, 1.5, 1.5, 1.5, 3.5},//venus
			{3.5, 1.5, 1.5, 2.5, 1.5, 3.0, 3.0, 1.5, 1.5, 1.5, 3.5},//mars
			{4.0, 1.5, 1.5, 3.0, 1.5, 3.5, 3.5, 1.5, 1.5, 1.5, 4.0},//jupiter
			{4.0, 1.5, 1.5, 3.0, 1.5, 3.5, 3.5, 1.5, 1.5, 1.5, 4.0},//saturn
			{3.0, 1.0, 1.0, 2.0, 1.0, 2.5, 2.5, 1.0, 1.0, 1.0, 3.0},//uranus
			{3.0, 1.0, 1.0, 2.0, 1.0, 2.5, 2.5, 1.0, 1.0, 1.0, 3.0},//pluto
			{3.0, 1.0, 1.0, 2.0, 1.0, 2.5, 2.5, 1.0, 1.0, 1.0, 3.0},//nodes
			{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5} };//ASCMC
		////house

		memcpy(orbis, orbis1, sizeof(orbis1));


		double orbisplanetspar1[11][2] = { {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0} };
		memcpy(orbisplanetspar, orbisplanetspar1, sizeof(orbisplanetspar1));


		double orbisH1[11] = { 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25 };
		memcpy(orbisH, orbisH1, sizeof(orbisH1));

		orbisparH[0] = orbisparH[1] = 0.25;
		orbiscuspH = 3.0;

		double orbisAscMC1[11] = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
		memcpy(orbisAscMC, orbisAscMC1, sizeof(orbisAscMC1));


		orbisparAscMC[0] = orbisparAscMC[1] = 0.5;

		orbiscuspAscMC = 5.0;
		exact = 1.0;

		//#Primary Dirs
		primarydir = 0;
		subprimarydir = 2;
		subzodiacal = 0;
		bianchini = FALSE;

		sigascmc[0] = sigascmc[1] = TRUE;

		sighouses = FALSE;

		BOOL sigplanets1[12] = { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE };
		//BOOL sigplanets2[16]={FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE};
		BOOL sigplanets2[16] = { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE };
		memcpy(sigplanets, sigplanets1, sizeof(sigplanets1));
		memcpy(promplanets, sigplanets2, sizeof(sigplanets2));

		BOOL pdaspects1[11] = { TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE };
		memcpy(pdaspects, pdaspects1, sizeof(pdaspects1));

		pdmidpoints = FALSE;

		pdparallels[0] = pdparallels[1] = 1;

		pdsecmotion = TRUE;
		pdsecmotioniter = 2;//#3rd iter is the default

		zodpromsigasps[0] = TRUE, zodpromsigasps[1] = FALSE;
		ascmchcsasproms = FALSE;

		pdfixstars = FALSE;

		BOOL pdfixstarssel1[30] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };
		memcpy(pdfixstarssel, pdfixstarssel1, sizeof(pdfixstarssel1));

		pdlof[0] = pdlof[1] = FALSE;

		pdsyzygy = FALSE;

		pdterms = TRUE;

		pdantiscia = FALSE;

		pdcustomer = FALSE;
		memset(pdcustomerlon, 0, 3);
		memset(pdcustomerlat, 0, 3);

		pdcustomersouthern = FALSE;
		pdcustomer2 = FALSE;

		memset(pdcustomer2lon, 0, 3);

		memset(pdcustomer2lat, 0, 3);

		pdcustomer2southern = FALSE;
		pdkeydyn = FALSE;
		pdkeyd = 0;
		pdkeys = 2;
		pdkeydeg = 0;
		pdkeymin = 0;
		pdkeysec = 0;

		useregressive = FALSE;

		us.nArabicNight = 0;
		lotoffortune = 2;

		//#Syzygy
		syzmoon = MOON;
		const char* fixstars1[30] = {
			"etTau", "alTau", "bePer", "ga-1And", "alSco", "alBoo", "deCnc", "gaCnc",
				"etUMa", "alOri", "alCen", "alCar", "alGem", "beLeo", "alPsA", "alCrB",
				"alPeg", "beAnd", "alUMi", "beGem", "M44", "alCMi", "alLeo", "beOri",
				"alCMa", "alVir", "alSer", "alLyr", "al-2Lib", "beLib" };

		memcpy(fixstars, fixstars1, sizeof(fixstars1));

		//#Profections
		zodprof = TRUE;
		usezodprojsprof = FALSE;

		isfirbonatti = TRUE;
		strcpy(deflocname, "");
		deflocplus = TRUE;
		defloczhour = 0;
		defloczminute = 0;
		deflocdst = FALSE;
		defloclondeg = 0;
		defloclonmin = 0;
		defloclatdeg = 0;
		defloclatmin = 0;
		defloceast = TRUE;
		deflocnorth = TRUE;
		deflocalt = 0;

		//##PDsInChart
		pdincharttyp = 2;
		pdinchartsecmotion = FALSE;

		pdinchartterrsecmotion = TRUE;

		//#Languages
		langid = 0;
		autosave = FALSE;

		unsigned long  rgbbmp1[cColor] = {
			0x000000L,// kBlack           0
				0x00007FL,// kMaroon          1
				RGBVal(0,172,0),// kDkGreen      2
				RGBVal(206,134,0),// kOrange     3
				0x7F0000L,// kDkBlue          4
				RGBVal(255,30,255),// kPurple          5
				0x7F7F00L,// kDkCyan          6
				0xBFBFBFL,// kLtGray          7
				0x7F7F7FL,// kDkGray          8
				0x0000FFL,// kRed             9
				RGBVal(0,213,0),//0x00FF00L,// kGreen           10
				0x00FFFFL,// kYellow          11
				RGBVal(255,157,174)/*0xFF0000L*/,// kBlue            12
				RGBVal(51,133,255)/*0xFF00FFL*/,// kMagenta           13
				RGBVal(28,174,240),//0xFFFF00L,// kCyan                                  14
				RGBVal(255,255,255)//RGBVal(254,255,251)//RGBVal(250,250,250)/*0xFFFFFFL*/// kWhite             15    FFA500
		};

		int kElemA1[4] = { 9,3,2,13 };
		int kAspA1[18 + 1] = { 15, 4, 12, 9, 10, 14, 13, 13, 4, 4, 11, 6, 6, 1, 5, 5, 1, 1, 5 };

		memcpy(&this->rgbbmp, &rgbbmp1, 16 * sizeof(long));
		memcpy(&this->kAspA, &kAspA1, 19 * sizeof(int));
		memcpy(&this->kElemA, &kElemA1, 4 * sizeof(int));

		strcpy(myBuff, "MS Shell Dlg");

		if (wi.chs)
			size = 12;
		else
			size = 14;

		cWeight = 300;//FW_LIGHT FW_BLACK,             cWeight
		bItalic = 0;//   bItalic
		hasToolbar = FALSE;
		bStatusBar = FALSE;
		fInverse = TRUE;
		UseAspectGrid = FALSE;
		HarmonicChartsYearsMode = TRUE;
		NoNutation = FALSE;
		NoPrecession = FALSE;
		AddNutation = FALSE;
		IsChaldean = TRUE;
		Dignities = 1;   // 1 for tradtion astrolog, 0 for modern astrolog
		IsPlanetModern = FALSE;
		IsPlanetTraditional = FALSE;
		IsPlanetLuminaries = FALSE;
		orbs = -1;
		progdirrestric = 8;
		UseTheme = 1;
		zregyptian = TRUE;
		zr27cap = TRUE;
		ZodRelsign = 0;
		ZodRelperiod = 6;
		PlanetOrder = 0;  //  Chaldean
		StartWith = 0;    //  起运方法
		sectptolemy = FALSE;
	}
};

Options options;

//==================================================================================================

/*
******************************************************************************
** Other Calculations.
******************************************************************************
*/

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

int FSphereLocal(double azi, double alt, const CIRC *pcr, int *xp, int *yp)
{
  if (gs.fEcliptic)
  {
    azi = Mod(azi - rDegQuad); 
	negV(alt);

    CoorXform2(&azi, &alt, is.latMC - rDegQuad);
    azi = Mod(is.lonMC - azi + rDegQuad);
    EquToEcl2(&azi, &alt);

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
  CoorXform2(&azi, &alt, rDegQuad - Lat);
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

int FSphereZodiac(double lon, double lat, const CIRC *pcr, int *xp, int *yp)
{
  double lonT, latT;

  lonT = Tropical(lon); latT = lat;
  EclToEqu2(&lonT, &latT);
  lonT = Mod(is.lonMC - lonT + rDegQuad);
  EquToLocal2(&lonT, &latT, rDegQuad - is.latMC);
  return FSphereLocal(lonT + rDegQuad, -latT, pcr, xp, yp);
}


//==================================================================================================


double rOne = 0.999999999;
double rFractal = 0.0;

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


#include <string.h>

const char* get_weekday_name(int day) {
	static const char* weekdays[] = {
		"Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday", "Sunday"
	};
	return (day >= 1 && day <= 7) ? weekdays[day - 1] : "";
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
				wprintf(L"%ls", szInput);
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
					wprintf(L"%ls", szInput);
				}
			}
		}
		else
		{
			putc(*pch, is.S);
		}
#if 0
		if (*pch == '\n' && is.S == stdout && wi.hdcPrint != hdcNil && is.cchRow >= us.nScrollRow)
		{
			/* If writing to the printer, start a new page when appropriate. */

			is.cchRow = 0;
			EndPage(wi.hdcPrint);
			StartPage(wi.hdcPrint);

			/* StartPage clobbers all the DC settings for Windows 95, 98, etc,   */
			/* however for NT, 2000 and later it doesn't. Here we cater for both */
			SetMapMode(wi.hdcPrint, MM_ANISOTROPIC);	/* For SetViewportExtEx */
			SetViewportOrgEx(wi.hdcPrint, 0, 0, NULL);
			SetViewportExtEx(wi.hdcPrint, wi.xPrint, wi.yPrint, NULL);
			SetWindowOrgEx(wi.hdcPrint, 0, 0, NULL);
			SetWindowExtEx(wi.hdcPrint, wi.xClient, wi.yClient, NULL);
			SetBkMode(wi.hdcPrint, TRANSPARENT);
			SelectObject(wi.hdcPrint, wi.hfont);
		}
#endif
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



/* Like above but return a fractional Julian time given the extra info. */
double MdytszToJulian(int mon, int day, int yea, double tim, double dst, double zon)
{
	return useswe.getMdyToJulian() + (DegMin2DecDeg(tim) + DegMin2DecDeg(zon) - DegMin2DecDeg(dst)) / 24.0;
}

/* Return the day of the week (Sunday is 0) of the specified given date. */
int DayOfWeek(int month, int day, int year)
{
	int d;

	d = (int)(((long)useswe.getMdyToJulian() + 1) % 7);
	return d < 0 ? d + 7 : d;
}

wchar_t* chSig3CW(int A)
{
	static wchar_t ret[2];
	ret[0] = tSignName[A][0];
	ret[1] = tSignName[A][1];
	return ret;
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
			swprintf(szZod, sizeof(szZod) / sizeof(wchar_t), L" %2d%ls%02d", d, tSignName[sign+1], m);
		}
		else
		{
			wchar_t tstmp[MAX_STRING_NAME_LEN];
			swprintf(tstmp,sizeof(tstmp)/sizeof(wchar_t), L"%ls",char_to_wchar(tSignName[sign+1]).c_str());
			swprintf(szZod, sizeof(szZod) / sizeof(wchar_t), L" %2d%lc%lc%lc%02d", d, tstmp[0], tstmp[1], tstmp[2] , m);
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

/* Here we return a string simply expressing the given value as degrees */
/* and minutes (and sometimes seconds) in the 0 to 360 degree circle.   */
wchar_t *SzDegree(double deg)
{
	static wchar_t szPos[11];
	int d, m;
	double s;

	deg = fabs(deg) + (is.fSeconds ? 0.5 / 60.0 / 60.0 : 0.5 / 60.0);
	d = (int)deg;
	m = (int)(RFract(deg) * 60.0);
	swprintf(szPos, sizeof(szPos) / sizeof(wchar_t), L"%3d%lc%02d'", d, btowc(chDeg1), m);
	if (is.fSeconds)
	{
		s = RFract(deg) * 60.0;
		s = RFract(s) * 60.0;
		swprintf(&szPos[7], sizeof(szPos[7]) / sizeof(wchar_t), L"%02d\"", (int)s);
	}
	return szPos;
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
	static char szTim[12];

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
			sprintf(szTim, "%2d:%02d", hr, min);
		else
			sprintf(szTim, "%2d:%02d:%02d", hr, min, sec);
	}
	else
	{
		if (sec == -1)
			//sprintf(szTim, "%2d:%02d%ls", Mod12(hr), min, hr < 12 ? L"AM" : L"PM");
			sprintf(szTim, "%2d:%02d%s", Mod12(hr), min, hr < 12 ? AmPm[0] : AmPm[1]);
		else
			sprintf(szTim, "%2d:%02d:%02d%s", Mod12(hr), min, sec, hr < 12 ? AmPm[0] : AmPm[1]);
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
	int hr, min, sec;
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
	int hr, min, sec;
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

wchar_t *SzTim2(double tim)
{
	/* Corrected by Michael Rideout on February 15, 2004: */
	int hr, min, sec;
	int d, m, s;
	double rMin;

	hr = NFloor(tim);
	rMin = (tim - hr) * 100.0;
	min = (int)(rMin + 0.5 / 600.0);
	rFractal = RFract(rMin);
	if (rFractal > rOne)
		rFractal = rSmall;
	sec = (int)(60.0 * rFractal + 0.5);

	decToDeg(DegMin2DecDeg(tim) + 0.50 / 3600.0, &d, &m, &s);
	return SzTime2(d, m, s);
}

/* Return a string containing the given time zone, given as a real value     */
/* having the hours before GMT in the integer part and minutes fractionally. */
char *SzZone(double zon)
{
	static char szZon[10];
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

	sprintf(szZon, "%c%d:%02d:%02d", zon > 0.0 ? '-' : '+', (int)fabs(zon),
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

/* Nicely format the given longitude and latitude locations and return    */
/* them in a string. Various parts of the program display a chart header, */
/* and this allows the similar computations to be coded only once.        */
char *SzLocation(double lon, double lat)
{
	static char szLoc[32];
	int mini, minj, seci, secj;
	double rMin;
	unsigned char ch1, ch2, ch3;

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

	//ch1 = chDeg1;
	ch1 = us.fAnsiChar > 1 ? chDeg2 : chDeg1;
	ch2 = us.fAnsiChar > 1 ? 39 : chMin1;
	ch3 = us.fAnsiChar > 1 ? 34 : chSec1;

	if (0)
	{
		if (us.fAnsiChar)
		{
			if (us.fAnsiChar != 3)
			{
				sprintf(szLoc, "%3.0f%c%02d%c%02d%c%c%3.0f%c%02d%c%02d%c%c",
					floor(fabs(lon)), ch1, mini, ch2, seci, ch3, lon < 0.0 ? 'E' : 'W',
					floor(fabs(lat)), ch1, minj, ch2, secj, ch3, lat < 0.0 ? 'S' : 'N');
			}
			else
			{
				sprintf(szLoc, "%3.0f%c%02d%c%02d%3.0f%c%02d%c%02d",
					floor(fabs(lon)), lon < 0.0 ? 'E' : 'W', mini, ch2, seci,
					floor(fabs(lat)), lat < 0.0 ? 'S' : 'N', minj, ch2, secj);
			}
		}
		else
		{
			sprintf(szLoc, "%3.0f%c%02d%c%02d%c%3.0f%c%02d%c%02d%c",
				floor(fabs(lon)), ch1, mini, ch2, seci, lon < 0.0 ? 'E' : 'W',
				floor(fabs(lat)), ch1, minj, ch2, secj, lat < 0.0 ? 'S' : 'N');
		}
	}
	else
	{
		if (us.fAnsiChar)
		{
			const char* deg = ":";
			if (us.fAnsiChar != 3)
			{
				sprintf(szLoc, "%3.0f%s%02d%c%02d%c%c%3.0f%s%02d%c%02d%c%c",
					floor(fabs(lon)), deg, mini, ch2, seci, ch3, lon < 0.0 ? 'E' : 'W',
					floor(fabs(lat)), deg, minj, ch2, secj, ch3, lat < 0.0 ? 'S' : 'N');
			}
			else
			{
				sprintf(szLoc, "%3.0f%c%02d%c%02d%3.0f%c%02d%c%02d",
					floor(fabs(lon)), lon < 0.0 ? 'E' : 'W', mini, ch2, seci,
					floor(fabs(lat)), lat < 0.0 ? 'S' : 'N', minj, ch2, secj);
			}
		}
		else
		{
			const char* deg =  ":" ;
			sprintf(szLoc, "%3.0f%s%02d%c%02d%c%3.0f%s%02d%c%02d%c",
				floor(fabs(lon)), deg, mini, ch2, seci, lon < 0.0 ? 'E' : 'W',
				floor(fabs(lat)), deg, minj, ch2, secj, lat < 0.0 ? 'S' : 'N');
		}

	}
	return szLoc;
}

char *SzLocationF(double lon, double lat)
{
	static char szLoc[64];
	int mini, minj, seci, secj;
	double rMin;
	unsigned char ch1, ch2, ch3;

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

	//ch1 = chDeg1;
	ch1 = ':';
	ch2 = us.fAnsiChar > 1 ? 39 : chMin1;
	ch3 = us.fAnsiChar > 1 ? 34 : chSec1;

	if (0)
	{
		if (us.fAnsiChar)
		{
			if (us.fAnsiChar != 3)
			{
				sprintf(szLoc, "%3.0f%c%02d%c%02d%c%c%3.0f%c%02d%c%02d%c%c",
					floor(fabs(lon)), ch1, mini, ch2, seci, ch3, lon < 0.0 ? 'E' : 'W',
					floor(fabs(lat)), ch1, minj, ch2, secj, ch3, lat < 0.0 ? 'S' : 'N');
			}
			else
			{
				sprintf(szLoc, "%3.0f%c%02d%c%02d%3.0f%c%02d%c%02d",
					floor(fabs(lon)), lon < 0.0 ? 'E' : 'W', mini, ch2, seci,
					floor(fabs(lat)), lat < 0.0 ? 'S' : 'N', minj, ch2, secj);
			}
		}
		else
		{
			sprintf(szLoc, "%3.0f%c%02d%c%02d%c%3.0f%c%02d%c%02d%c",
				floor(fabs(lon)), ch1, mini, ch2, seci, lon < 0.0 ? 'E' : 'W',
				floor(fabs(lat)), ch1, minj, ch2, secj, lat < 0.0 ? 'S' : 'N');
		}
	}
	else
	{
		if (us.fAnsiChar)
		{
			const char* deg = ":";
			if (us.fAnsiChar != 3)
			{
				sprintf(szLoc, "%3.0f%s%02d%c%02d%c%c%3.0f%s%02d%c%02d%c%c",
					floor(fabs(lon)), deg, mini, ch2, seci, ch3, lon < 0.0 ? 'E' : 'W',
					floor(fabs(lat)), deg, minj, ch2, secj, ch3, lat < 0.0 ? 'S' : 'N');
			}
			else
			{
				sprintf(szLoc, "%3.0f%c%02d%c%02d%3.0f%c%02d%c%02d",
					floor(fabs(lon)), lon < 0.0 ? 'E' : 'W', mini, ch2, seci,
					floor(fabs(lat)), lat < 0.0 ? 'S' : 'N', minj, ch2, secj);
			}
		}
		else
		{
			const char* deg = ":";
			sprintf(szLoc, "%3.0f%s%02d%c%02d%c%3.0f%s%02d%c%02d%c",
				floor(fabs(lon)), deg, mini, ch2, seci, lon < 0.0 ? 'E' : 'W',
				floor(fabs(lat)), deg, minj, ch2, secj, lat < 0.0 ? 'S' : 'N');
		}

	}
	return szLoc;
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

	ch2 = us.fAnsiChar > 1 ? 39 : chMin1;
	ch3 = us.fAnsiChar > 1 ? 34 : chSec1;
	if (us.fAnsiChar)
	{
		int a = floor(fabs(lat));
		if (us.fAnsiChar != 3)
		{
			swprintf(szLoc, sizeof(szLoc) / sizeof(wchar_t), L"%3d%c%02d%c%02d%c%c%3d%c%02d%c%02d%c%c",
				(int)floor(fabs(lon)), ch1, mini, ch2, seci, ch3,
				lon < 0.0 ? L'E' : L'W', a, ch1, minj, ch2, secj, ch3, lat < 0.0 ? L'S' : L'N');
		}
		else
		{
			swprintf(szLoc, sizeof(szLoc) / sizeof(wchar_t), L"%3d%lc%02d%lc%02d%3d%lc%02d%lc%02d",
				(int)floor(fabs(lon)), lon < 0.0 ? L'E' : L'W', mini, ch2, seci,
				a, lat < 0.0 ? L'S' : L'N', minj, ch2, secj);
		}
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

void SetCI(CI* ci, int mon, int day, int yea, double tim, double dst, double zon, double lon,
	double lat, wchar_t* loc)
{
	ci->mon = mon;
	ci->day = day;
	ci->yea = yea;
	ci->tim = tim;
	ci->dst = dst;
	ci->zon = zon;
	ci->lon = lon;
	ci->lat = lat;
	wcscpy(ci->loc, loc);
}


#if 0
/* Given a string representing the complete pathname to a file, strip off    */
/* all the path information leaving just the filename itself. This is called */
/* by the main program to determine the name of the Astrolog executable.     */
char *ProcessProgname(char* szPath)
{
	char *b, *c, *e;

	b = c = szPath;
	while (*c)
	{
		*c = ChUncap(*c);		/* Because DOS filenames are case insensitive. */
		c++;
	}
	e = c;
	while (c > b && *c != '.')
		c--;
	if (c > b)
		*c = 0;
	else
		c = e;
	while (c > b && *c != chDirSep)
		c--;
	if (c > b)
		szPath = c + 1;
	return szPath;
}

/* This is Astrolog's memory allocation routine, returning a pointer given  */
/* a size, and if the allocation fails prints a warning, including the user */
/* passed as parameter.														*/
void *allocate(long length, char *user)
{
	void *p;

	p = malloc(length);

	if (p == NULL && user)
	{
		PrintWarning("Can't allocate memory for %ls (%d bytes)", user, length);
	}

	return p;
}
#endif

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

/* Given a cp0.longitude and sign, determine whether: The cp0.longitude rules the sign, */
/* the cp0.longitude has its fall in the sign, the cp0.longitude exalts in the sign, or */
/* is debilitated in the sign; and return an appropriate character.       */
wchar_t Dignify(int obj, int sign)
{
	// sRFED = "RFed"      R=擢升   F擢升  e陷    d落  //入庙  同入庙  失势  同失势  耀升   落陷
	// 
	if (obj > cLastMoving)
		return L' ';
	if (ruler1[obj] == sign || ruler2[obj] == sign)
		if (wi.chs)
			return L'庙';
		else
			return L'R';
	if (ruler1[obj] == Mod12(sign + 6) || ruler2[obj] == Mod12(sign + 6))
		if (wi.chs)
			return L'旺';
		else
			return L'F';
	if (exalt[obj] == sign)
		if (wi.chs)
			return L'陷';
		else
			return L'e';
	if (exalt[obj] == Mod12(sign + 6))
		if (wi.chs)
			return L'落';
		else
			return L'd';

	if (!wi.chs)
		return L'-';
	else
		return L'—';
}

#define PrintAltitude(deg) PrintSzW(SzAltitude(deg))



#if 0
/* Another important procedure: Display any of the types of (text) charts    */
/* that the user specified they wanted, by calling the appropriate routines. */
void PrintChart(BOOL fProg)
{
	int fCall = FALSE;

	X_MEMSET(bigstr, 0, 302400 * 2);
	if (options.hasToolbar && !us.fPrimDirs)
	{
		PrintL();
	}

	if (us.fListing)
	{
		if (is.fMult)
			PrintL2();
		if (us.nRel < rcDifference)
		{
			ChartListing();
		}
		else if (us.nRel == rcDifference)
		{
			DisplayTimeDifference();
		}
		else
		{
			DisplayBiorhythm();
		}
		is.fMult = TRUE;
	}
	if (us.fWheel)
	{
		if (is.fMult)
			PrintL2();
		ChartWheel();
		is.fMult = TRUE;
	}
	if (us.fGrid)
	{
		if (is.fMult)
			PrintL2();
		if (us.nRel > rcDual)
		{
			fCall = us.fSmartAspects;
			us.fSmartAspects = FALSE;
			if (!FCreateGrid(FALSE))
				return;
			us.fSmartAspects = fCall;
			not(fCall);
			ChartGrid();
			if (us.fGridConfig)
			{					/* If -g0 switch in effect, then  */
				PrintL();		/* display aspect configurations. */
				if (!fCall)
					FCreateGrid(FALSE);
				DisplayGrands();
			}
		}
		else
		{

			/* Do a relationship aspect grid between two charts if -r0 in effect. */
			fCall = us.fSmartAspects;
			us.fSmartAspects = FALSE;
			if (!FCreateGridRelation(us.fGridConfig))
				return;
			us.fSmartAspects = fCall;
			ChartGridRelation();
		}
		is.fMult = TRUE;
	}
	if (us.fAspList)
	{
		if (is.fMult)
			PrintL2();
		if (us.nRel > rcDual)
		{
			/*
			  rcNone       = 0,
			  rcSynastry   = 1,
			  rcComposite  = 2,
			  rcMidpoint   = 3,
			  rcDifference = 4,
			  rcBiorhythm  = 5,
			  rcDual       = -1
			*/
			if (!fCall)
			{
				fCall = TRUE;
#ifdef TESTNEW
				if (!FCreateGridA(FALSE))
#else
				if (!FCreateGrid(FALSE))
#endif
					return;
			}
#ifdef TESTNEW
			ChartAspectA();
#else
			ChartAspect();
#endif
		}
		else
		{
			/*
  rcNone       = 0,
  rcSynastry   = 1,
  rcComposite  = 2,
  rcMidpoint   = 3,
  rcDifference = 4,
  rcBiorhythm  = 5,
  rcDual       = -1,
  rcTriWheel   = -2,
  rcQuadWheel  = -3,
  rcQuinWheel  = -4,
  rcHexaWheel  = -5,
  rcTransit    = -6,
  rcProgress   = -7,
		*/
			if (!FCreateGridRelation(FALSE))
				return;
			ChartAspectRelation();
		}
		is.fMult = TRUE;
	}

	if (us.fJyotish)
	{
		DisplayJyotish();
		return;
	}

	if (us.fAlmtsPoint)
	{
		DisplayAlmutensP();
		return;
	}
	if (us.fAlmtsChart)
	{
		DisplayAlmutensC();
		return;
	}
	if (us.fPrimDirs)
	{
		DisplayPrimDirs();
		return;
	}
	if (us.fPrimDirs2)
	{
		PrintPDsInChart();
		return;
	}

	if (us.fMidpoint)
	{
		if (is.fMult)
			PrintL2();
		if (us.nRel > rcDual)
		{
			if (!fCall)
			{
				if (!FCreateGrid(FALSE))
					return;
			}
			ChartMidpoint();
		}
		else
		{
			if (!FCreateGridRelation(TRUE))
				return;
			ChartMidpointRelation();
		}
		is.fMult = TRUE;
	}
	if (us.fHorizon)
	{
		if (is.fMult)
			PrintL2();
		if (us.fHorizonSearch)
		{
			ChartInDayHorizon();
		}
		else
			ChartHorizon();
		is.fMult = TRUE;
	}
	if (us.fOrbit)
	{
		if (is.fMult)
			PrintL2();
		ChartOrbit();
		is.fMult = TRUE;
	}
	if (us.fSector)
	{
		if (is.fMult)
			PrintL2();
		else
			PrintHeader();		/* Print chart header if it hasn't been done yet. */
		ChartSector();
		is.fMult = TRUE;
	}
	if (us.fInfluence)
	{
		if (is.fMult)
			PrintL2();
#ifdef TESTNEW
		ChartInfluenceA();
#else
		ChartInfluence();
#endif
		is.fMult = TRUE;
	}
	if (us.fAstroGraph)
	{
		if (is.fMult)
			PrintL2();
		ChartAstroGraph();
		is.fMult = TRUE;
	}
	if (us.fCalendar)
	{
		if (is.fMult)
			PrintL2();
		if (us.fCalendarYear)
			ChartCalendarYear();
		else
			ChartCalendarMonth();
		is.fMult = TRUE;
	}
	if (us.fInDay)
	{
		if (is.fMult)
			PrintL2();
		ChartInDaySearch(fProg);
		is.fMult = TRUE;
	}
	if (us.fInDayInf)
	{
		if (is.fMult)
			PrintL2();
		ChartInDayInfluence();
		is.fMult = TRUE;
	}
	if (us.fEphemeris)
	{
		if (is.fMult)
			PrintL2();
		ChartEphemeris();
		is.fMult = TRUE;
	}
	if (us.fTransit)
	{
		if (is.fMult)
			PrintL2();
		ChartTransitSearch(fProg);
		is.fMult = TRUE;
	}
	if (us.fTransitInf)
	{
		if (is.fMult)
			PrintL2();
		ChartTransitInfluence(fProg);
		is.fMult = TRUE;
	}
#ifdef ARABIC
	if (us.nArabic)
	{
		if (is.fMult)
			PrintL2();
		DisplayArabic();
		is.fMult = TRUE;
	}
#endif
	if (us.fFirdaria)
	{
		ChartFirdaria();
		is.fMult = TRUE;
	}
	if (us.fZodRel)
	{
		ChartZodRel();
		is.fMult = TRUE;
	}
	if (us.fDecennials)
	{
		ChartDecennials();
		is.fMult = TRUE;
	}

	if (us.fHouseLord)
	{
		ChartHouseLord();
		is.fMult = TRUE;
	}
	if (us.fEmptyASP)
	{
		ChartEmptyASP();
		is.fMult = TRUE;
	}

	if (us.fPlanetaryHours)
	{
		ChartPlanetaryHours();
		is.fMult = TRUE;
	}

	if (!is.fMult)
	{							/* Assume the -v chart if user */
		us.fListing = TRUE;	/* didn't indicate anything.   */
		PrintChart(fProg);
		is.fMult = TRUE;
	}
}
#endif

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
	if (IsAgeHarm)
	{
		PrintSzW(L"\n");
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", szAppNameW);
		PrintSzW(sz);
		PrintSzW(L" ");
	}
	else if (us.nRel == rcSynastry)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", szAppNameW);
		PrintSzW(sz);
	}
	else if (isSolarReturn)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", szAppNameW);
		PrintSzW(sz);
	}
	else if (isLunarReturn)
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
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%s %ls %ls (%lcT %ls GMT)", day3str, SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), SzTimW(ciMain.tim), ChDstW(ciMain.dst), SzZoneW(ciMain.zon));
		PrintSzW(sz);
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%lc%ls%ls%ls\n", fLoc && !fNam ? L'\n' : L' ',
			ciMain.loc, fLoc ? L" " : L"", SzLocation2(ciMain.lon, ciMain.lat));
		PrintSzW(sz);

		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", L"sNatalHouses3");
		PrintSzW(sz);
	}
	else if (IsSyzygy)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", szAppNameW);
		PrintSzW(sz);
	}
	else if (IsDraconic)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", szAppNameW);
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
	else if (IsProfections)
	{
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", szAppNameW);
		PrintSzW(sz);
		PrintSzW(L" ");
		if (options.zodprof)
		{
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls",szAppNameW);
			PrintSzW(sz);
			PrintSzW(L" ");
		}
		else
		{
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls",szAppNameW);
			PrintSzW(sz);
			PrintSzW(L" ");
		}
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
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%s %ls %ls (%lcT %ls GMT)", day3str, SzDate(ciTwin.mon, ciTwin.day, ciTwin.yea, 3), SzTimW(ciTwin.tim), ChDstW(ciTwin.dst), SzZoneW(ciTwin.zon));
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
		|| us.nRel == rcSynastry || us.nRel == rcProgress || us.fTransitInf || us.nRel == rcProgTran || isSolarReturn || isLunarReturn)
		/*|| us.nRel == rcSynastry || us.nRel == rcProgress || us.fTransitInf)*/
	{
		if (us.fProgressUS && !us.nRel)
		{
			if (us.fSolarArc == 1)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Chart directed (Degree per Year/Month) to\n");
			else if (us.fSolarArc == 2)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Chart directed (Solar Arc) to\n");
			else if (us.fSolarArc == 3)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Primary directed to\n");
			else if (us.fSolarArc == 4)
				swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Chart directed (Lunar Arc) to\n");
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
					else if (us.fSolarArc == 2)
						swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Natal houses, planets directed (Solar Arc) to\n");
					else if (us.fSolarArc == 3)
						swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Natal houses, planets directed (Primary) to\n");
					else if (us.fSolarArc == 4)
						swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"Natal houses, planets directed (Lunar Arc) to\n");
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
			else if (us.nRel == rcTransit || isSolarReturn || isLunarReturn)
			{
				if (!us.fInterpret && (us.fInfluence || us.fListing) || isSolarReturn || isLunarReturn)
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
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%s %ls %ls (%lcT %ls GMT)", daystr, SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), SzTimW(ciMain.tim), ChDstW(ciMain.dst), SzZoneW(ciMain.zon));
		else
			swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%s %ls %ls (%lcT %ls GMT)", day3str, SzDate(ciMain.mon, ciMain.day, ciMain.yea, 3), SzTimW(ciMain.tim), ChDstW(ciMain.dst), SzZoneW(ciMain.zon));
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

	is.szFileScreen = NULL;


	//Undo ...
	CreateElemTable(&et);

	PrintHeader();
	wprintf(L"\n");

#if 0
	if (IsSyzygy)
	{
		Syzygy syzygy;
		ignore16 = ignore1[16];
		ignore17 = ignore1[17];
		ignore1[16] = 0;
		ignore1[17] = 0;
		isSolarReturn = FALSE;
		isLunarReturn = FALSE;
		us.nRel = 0;
		wi.fCast = TRUE;
		memcpy(&ciSyzygy, &ciCore, sizeof(CI));
		ciSyzygy.yea = syzygy.time.year;
		ciSyzygy.mon = syzygy.time.month;
		ciSyzygy.day = syzygy.time.day;
		double t1 = syzygy.time.hour + syzygy.time.minute / 60.0 + syzygy.time.second / 3600.0 - ciCore.zon + ciCore.dst;
		ciSyzygy.tim = DecDeg2DegMin(t1);
		CI ciT = ciCore;
		ciCore = ciSyzygy;
		CastChart(TRUE);
		ciCore = ciT;
	}

	if (Is12Part /*us.f12parts*/)
	{
		Is12Part = FALSE;
		CastChart(TRUE);
		for (i = 0; i <= cObj; i++)
		{
			cp0.longitude[i] = calcDodecatemoria(cp0.longitude[i]);
		}
	}

	if (us.fNavamsas2)
	{
		for (i = 0; i <= cObj; i++)
		{
			cp0.longitude[i] = Navamsa(cp1.longitude[i]);
			cp0.latitude[i] = 0.0;
		}
	}

	if (us.fAnti) // 映点计算
	{
		Place place;
		double jut = DegMin2DecDeg(ciCore.tim) + 0.5 / 3600;
		int jhour = (int)jut;
		int jmin = (int)fmod(jut * 60, 60);
		int jsec = (int)fmod(jut * 3600, 60);
		timeP.time(ciCore.yea, ciCore.mon, ciCore.day, jhour, jmin, jsec, 0, FALSE, ciCore.zon, ciCore.dst, place);

		double d = swe_deltat(timeP.jd);
		double obl[6];
		char serr[256];

		swe_calc(timeP.jd + d, SE_ECL_NUT, 0, obl, serr);

		for (i = 0; i <= cObj; i++)
		{
			if (ignore1[i])
				continue;

			double ant, cant;
			calc(cp1.longitude[i], &ant, &cant);
			cp0.longitude[i] = ant;
			cp0.latitude[i] = 0.0;
		}
	}

	if (us.fAnti2) // 反映点计算
	{
		Place place;
		double jut = DegMin2DecDeg(ciCore.tim) + 0.5 / 3600;
		int jhour = (int)jut;
		int jmin = (int)fmod(jut * 60, 60);
		int jsec = (int)fmod(jut * 3600, 60);
		timeP.time(ciCore.yea, ciCore.mon, ciCore.day, jhour, jmin, jsec, 0, FALSE, ciCore.zon, ciCore.dst, place);
		double d = swe_deltat(timeP.jd);
		double obl[6];
		char serr[256];

		swe_calc(timeP.jd + d, SE_ECL_NUT, 0, obl, serr);

		for (i = 0; i <= cObj; i++)
		{
			if (ignore1[i])
				continue;

			double ant, cant;
			calc(cp1.longitude[i], &ant, &cant);
			cp0.longitude[i] = cant;
			cp0.latitude[i] = 0.0;
		}
	}

	memcpy(&ignoreT, &ignore1, NUMBER_OBJECTS);
	if (us.nRel == rcProgTran)
	{
		memcpy(&ignore1, &ignore3, NUMBER_OBJECTS);
		ignore1[18] = 1;
		ignore1[20] = 1;
		ignore1[22] = 0;
		ignore1[23] = 1;
		ignore1[24] = 1;
		ignore1[25] = 1;
		ignore1[26] = 1;
		ignore1[27] = 1;
		ignore1[28] = 1;
		ignore1[29] = 1;
		ignore1[30] = 1;
		ignore1[31] = 0;
		ignore1[32] = 1;
		ignore1[33] = 1;
	}
	CreateElemTable(&et);
	PrintHeader();				/* Show time and date of the chart being displayed. */

#ifdef INTERPRET
	if (us.fInterpret)
	{							/* Print an interpretation if -I in effect. */
		if (us.nRel == rcSynastry)
		{
			InterpretSynastry();	/* Print synastry interpretaion for -r -I.  */
		}
		else
		{
#ifdef INTERPRETALT
			//if (InterpretAlt && (!us.nRel || us.nRel == rcTransit))
			//{
			//	if (is.fMult)
			//		PrintL2();
//
//				if (us.nRel == rcTransit)
//					TransitInterpret();
//				else
//					NatalInterpret();
//
//				is.fMult = TRUE;
//			}
//			else
//			{
			if (InterpretAlt && (!us.nRel || us.nRel == rcTransit || isSolarReturn || isLunarReturn || rcProgress || us.nRel == rcProgTran))
			{
				if (is.fMult)
					PrintL2();

				if (us.nRel == rcProgress || us.nRel == rcProgTran)
				{
					IsPDsData = TRUE;
					ProgressInterpret();
					IsPDsData = FALSE;
				}
				else if (us.nRel == rcTransit || isSolarReturn || isLunarReturn)
				{
					TransitInterpret();
				}
				else
				{
					NatalInterpret();
				}

				is.fMult = TRUE;
			}
			else
			{

#endif
				InterpretLocation();	/* Do normal interpretation for just -v -I. */
#ifdef INTERPRETALT
			}
#endif
		}
		return;
	}
#endif
#endif
	
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
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" %lc ", cpPDs.vel_longitude[obj_left] >= 0.0 ? L' ' : chRet);
			else
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L" %lc ", cp0.vel_longitude[obj_left] >= 0.0 ? L' ' : chRet);
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
				a1 = Dignify(obj_left, Z2Sign(cpPDs.longitude[obj_left]));// 		a1	8212 L'—'	wchar_t
			else
				a1 = Dignify(obj_left, Z2Sign(cp0.longitude[obj_left]));

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
			a2[0] = Dignify(obj_left, k);
			swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"[%ls", a2);

			PrintSzW(FCusp(obj_left) ? L"  " : sz);

			a2[0] = Dignify(obj_left, l);
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
				swprintf(sz, sizeof(sz) / sizeof(wchar_t), L"%ls %2d:", L"House cusp", obj_right);
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
						tElement[k],
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
			is.fSeconds = FALSE;
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
	//memcpy(&ignore1, &ignoreT, NUMBER_OBJECTS);
}


static int gregflag = SE_GREG_CAL;

/* This is a subprocedure of CastChart(). Once we have the chart parameters, */
/* calculate a few important things related to the date, i.e. the Greenwich  */
/* time, the Julian day and fractional part of the day, the offset to the    */
/* sidereal, and a couple of other things.                                   */
double ProcessInput(BOOL fDate)
{
	double Off;
	double x[6], jd, eph_time, ayanamsa;
	char serr[AS_MAXCH];

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
		is.JD = useswe.getMdyToJulian();
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
		gregflag = FALSE;
	else
		gregflag = TRUE;

	//jut = ciCorejhour + jmin / 60.0 + jsec / 3600.0;

	if (us.fSidereal)
	{
		useswe.setMod(us.nSiderealMode);
	}

	useswe.setDeltat(jd);
	eph_time = jd + useswe.getDeltat();
	useswe.setAyanamsa(eph_time);
	ayanamsa = useswe.getAyanamsa();

	if (ayanamsa > 180.0)
	{
		// swe_get_ayanamsa flips Ayanamsha from 0 to 360 degrees around year 200, 
		// Astrolog doesn't
		ayanamsa -= 360.0;
	}

	// Swiss Ephemeris returns a positive value for ayanamsa
	Off = -ayanamsa;

	// Compute angle that the ecliptic is inclined to the Celestial Equator 计算黄道向天赤道倾斜的角度 eph_time=2435033.5142473509
	useswe.sweCalc(eph_time, SE_ECL_NUT, 1, x, serr);

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
	if (options.AddNutation == TRUE && us.fSidereal)
		is.rSid = is.rSid + us.Nutation;

	return Off;
}

/* Convert polar to rectangular coordinates. */
void PolToRec(double A, double R, double* X, double* Y) {
	if (A == 0.0)
		A = rSmall;
	*X = R * cos(A);
	*Y = R * sin(A);
}

/* Convert rectangular to polar coordinates. */
void RecToPol(double X, double Y, double* A, double* R) {
	if (Y == 0.0)
		Y = rSmall;
	*R = sqrt(X * X + Y * Y);
	*A = Angle(X, Y);
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



/* This is another subprocedure of CastChart(). Calculate a few variables */
/* corresponding to the chart parameters that are used later on. The      */
/* astrological vertex (object number nineteen) is also calculated here.  */
void ComputeVariables(double* vtx)
{
	double R, R2, B, L, O, G, X, Y, A;

	is.RAa = Deg2Rad(Mod((6.6460656 + 2400.0513 * is.T + 2.58E-5 * is.T * is.T + ciCore.tim) * 15.0 - ciCore.lon));
	R2 = is.RAa;
	O = -is.rObliquity;
	B = ciCore.lat;
	A = R2;
	R = 1.0;
	PolToRec(A, R, &X, &Y);
	X *= cos(O);
	RecToPol(X, Y, &A, &R);
	is.MC = Mod(is.rSid + Rad2Deg(A));	/* Midheaven */

	L = R2 + rPi;
	B = rPiHalf - fabs(B);
	if (ciCore.lat < 0.0)
		B = -B;
	G = RecToSph(B, L, O) + rPiHalf;

	// keep the vertex always on the western hemisphere in tropical latitudes 
	if (fabs(ciCore.lat) <= is.rObliquity && ModRad(A - G) > rPi)
	{
		G += rPi;
	}

	*vtx = Mod(is.rSid + Rad2Deg(G));	/* Vertex */
}


/*
******************************************************************************
** Planetary Position Calculations.
******************************************************************************
*/

/* Given three values, return them combined as the coefficients of a */
/* quadratic equation as a function of the chart time.               */

double ReadThree(double r0, double r1, double r2)
{
	return Deg2Rad(r0 + r1 * is.T + r2 * is.T * is.T);
}


/* Another coordinate transformation. This is used by the ComputePlanets() */
/* procedure to rotate rectangular coordinates by a certain amount.        */

void RecToSph2(double AP, double AN, double INu, double* X, double* Y, double* G)
{
	double R, D, A;

	RecToPol(*X, *Y, &A, &R);
	A += AP;
	PolToRec(A, R, X, Y);
	D = *X;
	*X = *Y;
	*Y = 0.0;
	RecToPol(*X, *Y, &A, &R);
	A += INu;
	PolToRec(A, R, X, Y);
	*G = *Y;
	*Y = *X;
	*X = D;
	RecToPol(*X, *Y, &A, &R);
	A += AN;
	if (A < 0.0)
		A += 2.0 * rPi;
	PolToRec(A, R, X, Y);
}

/* Calculate some harmonic delta error correction factors to add onto the */
/* coordinates of Jupiter through Pluto, for better accuracy.             */
void ErrorCorrect(int ind, double* x, double* y, double* z)
{
	double U, V, W, A, S0, T0[4], * pr;
	int IK, IJ, irError;

	irError = rErrorCount[ind - oJup];
	pr = (double*)&rErrorData[rErrorOffset[ind - oJup]];
	for (IK = 1; IK <= 3; IK++)
	{
		if (ind == oJup && IK == 3)
		{
			T0[3] = 0.0;
			break;
		}
		if (IK == 3)
			irError--;
		S0 = ReadThree(pr[0], pr[1], pr[2]);
		pr += 3;
		A = 0.0;
		for (IJ = 1; IJ <= irError; IJ++)
		{
			U = *pr++;
			V = *pr++;
			W = *pr++;
			A += Deg2Rad(U) * cos((V * is.T + W) * rPi / 180.0);
		}
		T0[IK] = Rad2Deg(S0 + A);
	}
	*x += T0[2];
	*y += T0[1];
	*z += T0[3];
}

/* Another subprocedure of the ComputePlanets() routine. Convert the final */
/* rectangular coordinates of a cp0.longitude to zodiac position and declination. */
void ProcessPlanet(int ind, double aber)
{
	double ang, rad;

	RecToPol(spacex[ind], spacey[ind], &ang, &rad);
	cp0.longitude[ind] = Mod(Rad2Deg(ang) /*+ NU */ - aber + is.rSid);
	RecToPol(rad, spacez[ind], &ang, &rad);

	if (us.objCenter == oSun && ind == oSun)
	{
		ang = 0.0;
	}

	ang = Rad2Deg(ang);
	while (ang > 90.0)		/* Ensure declination is from -90..+90 degrees. */
	{
		ang -= 180.0;
	}

	while (ang < -90.0)
	{
		ang += 180.0;
	}

	cp0.latitude[ind] = ang;
}

/* Calculate the position of each body that orbits the Sun. A heliocentric chart is  */
/* most natural; extra calculation is needed to have other central bodies. */
void ComputePlanets()
{
	double helioret[cLastMoving + 1], heliox[cLastMoving + 1];
	double helioy[cLastMoving + 1], helioz[cLastMoving + 1];
	double aber = 0.0, AU, E, EA, E1, M, XW, YW, AP, AN, INu, X, Y, G, XS, YS, ZS;
	int ind = oSun, i;
	OE* poe;

	while (ind <= cLastMoving)
	{
		if (ignore1[ind] && ind > oSun)
			goto LNextPlanet;
		poe = &rgoe[IoeFromObj(ind)];

		EA = M = ModRad(ReadThree(poe->ma0, poe->ma1, poe->ma2));
		E = Rad2Deg(ReadThree(poe->ec0, poe->ec1, poe->ec2));
		for (i = 1; i <= 5; i++)
			EA = M + E * sin(EA);	/* Solve Kepler's equation */
		AU = poe->sma;			/* Semi-major axis         */
		E1 = 0.01720209 / (pow(AU, 1.5) * (1.0 - E * cos(EA)));	/* Begin velocity coordinates */
		XW = -AU * E1 * sin(EA);	/* Perifocal coordinates      */
		YW = AU * E1 * pow(1.0 - E * E, 0.5) * cos(EA);
		AP = ReadThree(poe->ap0, poe->ap1, poe->ap2);
		AN = ReadThree(poe->an0, poe->an1, poe->an2);
		INu = ReadThree(poe->in0, poe->in1, poe->in2);	/* Calculate inclination  */
		X = XW;
		Y = YW;
		RecToSph2(AP, AN, INu, &X, &Y, &G);	/* Rotate velocity coords */
		heliox[ind] = X;
		helioy[ind] = Y;
		helioz[ind] = G;		/* Helio ecliptic rectangtular */
		X = AU * (cos(EA) - E);	/* Perifocal coordinates for        */
		Y = AU * sin(EA) * pow(1.0 - E * E, 0.5);	/* rectangular position coordinates */
		RecToSph2(AP, AN, INu, &X, &Y, &G);	/* Rotate for rectangular */
		XS = X;
		YS = Y;
		ZS = G;					/* position coordinates   */
		if (FBetween(ind, oJup, oPlu))
			ErrorCorrect(ind, &XS, &YS, &ZS);
		cp0.vel_longitude[ind] =				/* Helio daily motion */
			(XS * helioy[ind] - YS * heliox[ind]) / (XS * XS + YS * YS);
		spacex[ind] = XS;
		spacey[ind] = YS;
		spacez[ind] = ZS;
		ProcessPlanet(ind, 0.0);
	LNextPlanet:
		ind += (ind == oSun ? 2 : (ind != cLastPlanet ? 1 : uranLo - cLastPlanet));
	}

	spacex[oEar] = spacex[oSun];
	spacey[oEar] = spacey[oSun];
	spacez[oEar] = spacez[oSun];
	cp0.longitude[oEar] = cp0.longitude[oSun];
	cp0.latitude[oEar] = cp0.latitude[oSun];
	cp0.vel_longitude[oEar] = cp0.vel_longitude[oSun];
	heliox[oEar] = heliox[oSun];
	helioy[oEar] = helioy[oSun];
	//helioret[oEar] = helioret[oSun]; warning C4700: local variable 'helioret' 
	//                                  used without having been initialized
	spacex[oSun] = spacey[oSun] = spacez[oSun] = cp0.longitude[oSun] = cp0.latitude[oSun] = heliox[oSun] = helioy[oSun] = 0.0;
	if (us.objCenter == oSun)
	{
		if (us.fVelocity)
			for (i = 0; i <= cLastMoving; i++)	/* Use relative velocity */
				cp0.vel_longitude[i] = Deg2Rad(1.0);	/* if -v0 is in effect.  */
		return;
	}

	/* A second loop is needed for geocentric charts or central bodies other */
	/* than the Sun. For example, we can't find the position of Mercury in   */
	/* relation to Pluto until we know the position of Pluto in relation to  */
	/* the Sun, and since Mercury is calculated first, another pass needed.  */

	ind = us.objCenter;
	for (i = 0; i <= cLastMoving; i++)
	{
		helioret[i] = cp0.vel_longitude[i];
		if (i != oMoo && i != ind)
		{
			spacex[i] -= spacex[ind];
			spacey[i] -= spacey[ind];
			spacez[i] -= spacez[ind];
		}
	}
	for (i = oEar; i <= cLastMoving; i += (i == oSun ? 2 : (i != cLastPlanet ? 1 : uranLo - cLastPlanet)))
	{
		if ((ignore1[i] && i > oSun) || i == ind)
			continue;
		XS = spacex[i];
		YS = spacey[i];
		ZS = spacez[i];
		cp0.vel_longitude[i] = (XS * (helioy[i] - helioy[ind]) - YS * (heliox[i] - heliox[ind])) / (XS * XS + YS * YS);
		if (ind == oEar)
			aber = 0.0057756 * sqrt(XS * XS + YS * YS + ZS * ZS) * Rad2Deg(cp0.vel_longitude[i]);	/* Aberration */
		ProcessPlanet(i, aber);
		if (us.fVelocity)		/* Use relative velocity */
			cp0.vel_longitude[i] = Deg2Rad(cp0.vel_longitude[i] / helioret[i]);	/* if -v0 is in effect   */
	}
	spacex[ind] = spacey[ind] = spacez[ind] = 0.0;
}

/* Given an object index and a Julian Day ephemeris time, get ecliptic longitude and	*/
/* latitude of the object and its velocity and distance from the Earth or Sun. 			*/
#define SEFLG_EPHMASK   (SEFLG_JPLEPH|SEFLG_SWIEPH|SEFLG_MOSEPH)
// converts an astrolog object number to Swiss Ephemeris object number
// returns Swiss Ephemeris object number, or -1 if conversion not possible
static int astrolog_object_2_SE_object(int astrolog_object)
{
	int swiss_object;

	if (FBetween(astrolog_object, oSun, oPlu))
	{
		swiss_object = astrolog_object - oSun + SE_SUN;
	}
	else if (FBetween(astrolog_object, oCeres, oVesta))
	{
		swiss_object = astrolog_object - oCeres + SE_CERES;
	}
	else if (astrolog_object == oNoNode)
	{
		swiss_object = us.fTrueNode ? SE_TRUE_NODE : SE_MEAN_NODE;
	}
	else if (astrolog_object == oLil)
	{
		swiss_object = oscLilith ? SE_OSCU_APOG : SE_MEAN_APOG;
	}
	else if (astrolog_object == oChi)
	{
		swiss_object = SE_CHIRON;
	}
	else if (FBetween(astrolog_object, oCupido, oPoseidon))
	{
		swiss_object = astrolog_object - oCupido + SE_CUPIDO;
	}
	else if (astrolog_object == oProserpina)
	{
		swiss_object = SE_PROSERPINA;
	}
	else if (astrolog_object == oEar)
	{
		swiss_object = SE_EARTH;
	}
	else
	{
		swiss_object = -1;	// no equivalent, conversion not possible
	}

	return swiss_object;
}

BOOL CalculatePlanetSE(int ind, double jde, BOOL heliocentric, double SE_coordinates[6])
{
	int iobj;
	int flag = 0;
	char serr[AS_MAXCH];

	flag = (flag & ~SEFLG_EPHMASK);
	if (eepp == 0)
		flag = SEFLG_JPLEPH;
	else if (eepp == 1)
		flag = SEFLG_SWIEPH;
	else if (eepp == 2)
		flag = SEFLG_MOSEPH;

	if (us.fSidereal) // 已经包含了岁差
	{
		flag |= SEFLG_SIDEREAL;
		useswe.setMod(us.nSiderealMode);
	}
	flag |= SEFLG_SPEED;

	iobj = astrolog_object_2_SE_object(ind);

	if (iobj >= 0)
	{
		flag |= heliocentric ? SEFLG_SPEED | SEFLG_HELCTR : SEFLG_SPEED;

		if (options.NoNutation)
			flag |= SEFLG_NONUT;

		if (options.NoPrecession)
			flag |= SEFLG_J2000;

		if (options.topocentric)
		{
			double lon = -ciCore.lon;
			double lat = Rad2Deg(ciCore.lat);
			swe_set_topo(lon, lat, 0);
			flag |= SEFLG_TOPOCTR;
		}

		if (swe_calc(jde, iobj, flag, SE_coordinates, serr) >= 0)
		{
			return TRUE;	// success
		}
		else if (eepp == 1)
		{
			flag = SEFLG_MOSEPH;
			flag |= heliocentric ? SEFLG_SPEED | SEFLG_HELCTR : SEFLG_SPEED;
			if (swe_calc(jde, iobj, flag, SE_coordinates, serr) >= 0)
			{
				return TRUE;
			}
		}
	}
	return FALSE; // object not supported or other kinds of errors
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


/* Transform spherical to rectangular coordinates in x, y, z. */
void SphToRec(double r, double azi, double alt, double* rx, double* ry, double* rz)
{
	double rT;

	*rz = r * RSinD(alt);
	rT = r * RCosD(alt);
	*rx = rT * RCosD(azi);
	*ry = rT * RSinD(azi);
}

enum _housesystem {
	hsPlacidus = 0,
	hsKoch = 1,
	hsEqual = 2,
	hsCampanus = 3,
	hsMeridian = 4,
	hsRegiomontanus = 5,
	hsPorphyry = 6,
	hsMorinus = 7,
	hsTopocentric = 8,
	hsAlcabitius = 9,
	hsEqualMC = 10,
	hsSinewaveDelta = 11,         //  Neo-Porphyry
	hsWhole = 12,         //  ???
	hsVedic = 13,         //  ???
	hsNull = 14,
	hsSripati = 15,
	hsHorizon = 16,
	cSystem = 17,
};


static int32 iflag = 0, iflag2;              /* external flag: helio, geo... */
#define rDegHalf   180.0
#define rDegMax    360.0
#define rDegQuad   90.0
void SwissHouse(double jd, double lon, double lat, int housesystem, double* asc, double* mc, double* ra, double* vtx, double* ep, double* ob, double* off)
{
	double cusp[13], ascmc[10], ascmc2[10];
	int i;
	char ch;

	switch (housesystem) // hsKrusinski = 10
	{
	case hsPlacidus:
		ch = 'P'; break;
	case hsKoch:
		ch = 'K'; break;
	case hsEqual:
		ch = 'E'; break;
	case hsCampanus:
		ch = 'C'; break;
	case hsMeridian:
		ch = 'X'; break;
	case hsRegiomontanus:
		ch = 'R'; break;
	case hsPorphyry:
		ch = 'O'; break;
	case hsMorinus:
		ch = 'M'; break;
	case hsTopocentric:
		ch = 'T'; break;
	case hsAlcabitius:
		ch = 'B'; break;
	case hsVedic:
		ch = 'V'; break;
	case hsSripati:
		ch = 'S'; break;
	case hsSinewaveDelta:
		ch = 'L'; break;
	default:
		ch = 'A'; break;
	}
	jd = JulianDayFromTime(jd);
	lon = -lon;

	///* The following is largely copied from swe_houses(). */
	//double armc, eps, nutlo[2];
	//double tjde = jd + swe_deltat(jd);
	////Undo...
	////eps = swi_epsiln(tjde, 0) * RADTODEG;
	////swi_nutation(tjde, 0, nutlo);
	//
	//for (i = 0; i < 2; i++)
	//	nutlo[i] *= RADTODEG;
	//
	//us.Nutation = nutlo[0];
	//以上注释部分为新改造的，用 swe_sidtime 不包含 eps nutlo这两个参数
	double armc, eps, nutlo[2], tjde;
	double degtmp = swe_sidtime_args(jd, &eps, nutlo, &tjde) * 15;
	us.Nutation = nutlo[0];
	if (!us.fGeodetic)
		//armc = swe_degnorm(swe_sidtime0(jd, eps + nutlo[1], nutlo[0]) * 15 + lon);
		armc = swe_degnorm(degtmp + lon);
	else
		armc = lon;
	swe_houses_armc(armc, lat, eps + nutlo[1], ch, cusp, ascmc);

	iflag = 0;	/* 0 (TROPICAL) or SEFLG_SIDEREAL or SEFLG_RADIANS or SEFLG_NONUT */
	swe_houses_ex(jd, iflag, lat, lon, ch, cusp, ascmc2);

	/* Fill out return parameters for cusp array, Ascendant, etc. */
	swe_set_sid_mode(us.nSiderealMode, 0, 0);
	*off = -swe_get_ayanamsa(tjde);
	is.rSid = us.rZodiacOffset + (us.fSidereal ? *off : 0.0);

	if (options.AddNutation != TRUE && us.fSidereal)
		is.rSid = is.rSid - us.Nutation;

	*asc = Mod(ascmc[SE_ASC] + is.rSid);
	*mc = Mod(ascmc[SE_MC] + is.rSid);
	*ra = RFromD(Mod(ascmc[SE_ARMC] + is.rSid));
	*vtx = Mod(ascmc[SE_VERTEX] + is.rSid);
	*ep = Mod(ascmc[SE_EQUASC] + is.rSid);
	*ob = eps;

	if (us.fSidereal)
	{
		*asc = Mod(*asc + us.rSiderealCorrection);
		*mc = Mod(*mc + us.rSiderealCorrection);
	}

	for (i = 1; i <= cSign; i++)
	{
		if (!us.fSidereal)
			cp0.cusp_pos[i] = Mod(cusp[i] + is.rSid);
		else
			cp0.cusp_pos[i] = Mod(cusp[i] + is.rSid + us.rSiderealCorrection);
	}

	/* Want generic MC. Undo if house system flipped it 180 degrees. */
	if ((housesystem == hsCampanus || housesystem == hsRegiomontanus ||
		housesystem == hsTopocentric) && MinDifference(*mc, *asc) < 0.0)
		*mc = Mod(*mc + rDegHalf);

	/* Have Astrolog compute the houses if Swiss Ephemeris didn't do so. */
	//if (ch == 'A')
		//ComputeHouses(housesystem);
}


void ComputeWithSwissEphemeris(double t)
{
	int i;
	double SE_coordinates[6];
	double longitude, latitude, distance, speed_longitude, speed_latitude;
	BOOL	use_SE_heliocentric_mode;
	double jd, jde;


	/* We can compute the positions of Sun through Pluto, Chiron, the four	*/
	/* asteroids, Lilith, the (true or mean) North Node and the Uranians 	*/
	/* using Swiss Ephemeris. The other objects must be done elsewhere.     */

	/* Note, that with central cp0.longitude, other than Earth, Lilith isn't */
	/* calculated at all and we use South Node instead.          V.A. */

	// Calculate cp0.longitude positions with Earth as centre, or Sun if centre is another cp0.longitude
	// The reason is that SE cannot calculate positions based on other centres, therefore
	// we first calculate heliocentric, and further down in the function we convert
	// coordinates.
	use_SE_heliocentric_mode = us.objCenter != oEar;//0

	jd = JulianDayFromTime(t);
	cp0.deltaT = swe_deltat(jd);

	jde = jd + cp0.deltaT;

	for (i = oEar; i <= uranHi; i++)
	{
		if ((ignore1[i] && i > oMoo) || FBetween(i, oFor, cuspHi))
		{
			continue;
		}

		if ((i == oEar && us.objCenter == oEar) || (i == oSun && use_SE_heliocentric_mode))
		{
			// there is no point in calculation a geocentric position for the Earth,
			// also for the Sun if we use SE heliocentric positions
			continue;
		}

		if (CalculatePlanetSE(i, jde, use_SE_heliocentric_mode, SE_coordinates))
		{
			// SE_coordinates is array of 6 doubles for longitude, latitude, distance, 
			// speed in long., speed in lat., and speed in dist. 180.85993769817415

			if (us.fSidereal)
			{
				longitude = Mod(SE_coordinates[0] + us.rSiderealCorrection);
				if (options.AddNutation)
					longitude = Mod(SE_coordinates[0] + us.rSiderealCorrection + us.Nutation);
			}
			else
				longitude = SE_coordinates[0];

			latitude = SE_coordinates[1];
			distance = SE_coordinates[2];
			speed_longitude = SE_coordinates[3];
			speed_latitude = SE_coordinates[4];

			/* Note that this can't compute charts with central planets other */
			/* than the Sun or Earth or relative velocities in current state. */
			/* In this case we use previously calculated velocities, also  */
			/* Node coordinates, which aren't computed  here.        V.A.  */

			if (us.objCenter == oEar || (i != oNoNode && i != oLil))
			{
				cp0.longitude[i] = longitude;
				cp0.latitude[i] = latitude;
				cp0.vel_longitude[i] = Deg2Rad(speed_longitude);
			}

			cp0.vel_latitude[i] = Deg2Rad(speed_latitude);
			cp0.vel_distance[i] = SE_coordinates[6];

			SphToRec(SE_coordinates[2], cp0.longitude[i], cp0.latitude[i],
				&cp0.pt[i].x, &cp0.pt[i].y, &cp0.pt[i].z);

			/* Compute x1,y,z coordinates from azimuth, altitude, and distance. */
			SphToRec(distance, cp0.longitude[i], cp0.latitude[i], &spacex[i], &spacey[i], &spacez[i]);
		}
	}

	// this is needed before the transformation that follows
	if (use_SE_heliocentric_mode)
	{
		spacex[oSun] = spacey[oSun] = spacez[oSun] = cp0.longitude[oSun] = cp0.latitude[oSun] = cp0.vel_longitude[oSun] = 0.0;
	}
	else
	{
		spacex[oEar] = spacey[oEar] = spacez[oEar] = cp0.longitude[oEar] = cp0.latitude[oEar] = cp0.vel_longitude[oEar] = 0.0;
	}

	if (us.objCenter <= oSun)	// heliocentric or geocentric
	{
		return;
	}

	/* If other cp0.longitude centered, shift all positions as in Matrix formulas. */

	for (i = 0; i <= cLastPlanet; i++)
	{
		if (!FIgnore(i))
		{
			spacex[i] -= spacex[us.objCenter];
			spacey[i] -= spacey[us.objCenter];
			spacez[i] -= spacez[us.objCenter];

			// convert the rectangular coordinates of the cp0.longitude to zodiac position and declination
			ProcessPlanet(i, 0.0);
		}
	}

	// zero coordinates of central cp0.longitude

	spacex[us.objCenter] = spacey[us.objCenter] = spacez[us.objCenter] = 0.0;
	cp0.longitude[us.objCenter] = cp0.latitude[us.objCenter] = cp0.vel_longitude[us.objCenter] = 0.0;
}

/* Calculate the position and declination of the Moon, and the Moon's North  */
/* Node. This has to be done separately from the other planets, because they */
/* all orbit the Sun, while the Moon orbits the Earth.                       */
void ComputeLunar(double* moonlo, double* moonla, double* nodelo, double* nodela)
{
	double LL, G, N, G1, D, L, ML, L1, MB, T1, Y, M = 3600.0, T2;

	T2 = is.T * is.T;
	LL = 973563.0 + 1732564379.0 * is.T - 4.0 * T2;	/* Compute mean lunar longitude    */
	G = 1012395.0 + 6189.0 * is.T;					/* Sun's mean longitude of perigee */
	N = 933060.0 - 6962911.0 * is.T + 7.5 * T2;		/* Compute mean lunar node         */
	G1 = 1203586.0 + 14648523.0 * is.T - 37.0 * T2;	/* Mean longitude of lunar perigee */
	D = 1262655.0 + 1602961611.0 * is.T - 5.0 * T2;	/* Mean elongation of Moo from Sun */
	L = (LL - G1) / M;
	L1 = ((LL - D) - G) / M;						/* Some auxiliary angles           */
	T1 = (LL - N) / M;
	D = D / M;
	Y = 2.0 * D;

	/* Compute Moon's perturbations. */

	ML = 22639.6 * RSinD(L) - 4586.4 * RSinD(L - Y) + 2369.9 * RSinD(Y) +
		769.0 * RSinD(2.0 * L) - 669.0 * RSinD(L1) -
		411.6 * RSinD(2.0 * T1) - 212.0 * RSinD(2.0 * L - Y) - 206.0 * RSinD(L + L1 - Y);
	ML +=
		192.0 * RSinD(L + Y) - 165.0 * RSinD(L1 - Y) + 148.0 * RSinD(L - L1) -
		125.0 * RSinD(D) - 110.0 * RSinD(L + L1) - 55.0 * RSinD(2.0 * T1 - Y) -
		45.0 * RSinD(L + 2.0 * T1) + 40.0 * RSinD(L - 2.0 * T1);

	*moonlo = G = Mod((LL + ML) / M + is.rSid);	/* Lunar longitude */

	/* Compute lunar latitude. */

	MB = 18461.5 * RSinD(T1) + 1010.0 * RSinD(L + T1) - 999.0 * RSinD(T1 - L) -
		624.0 * RSinD(T1 - Y) + 199.0 * RSinD(T1 + Y - L) - 167.0 * RSinD(L + T1 - Y);
	MB +=
		117.0 * RSinD(T1 + Y) + 62.0 * RSinD(2.0 * L + T1) -
		33.0 * RSinD(T1 - Y - L) - 32.0 * RSinD(T1 - 2.0 * L) - 30.0 * RSinD(L1 + T1 - Y);
	*moonla = MB = RSgn(MB) * ((fabs(MB) / M) / 360.0 - floor((fabs(MB) / M) / 360.0)) * 360.0;

	/* Compute position of the North Lunar Node, either True or Mean. */

	if (us.fTrueNode)
	{
		N = N + 5392.0 * RSinD(2.0 * T1 - Y) - 541.0 * RSinD(L1) -
			442.0 * RSinD(Y) + 423.0 * RSinD(2.0 * T1) - 291.0 * RSinD(2.0 * L - 2.0 * T1);
	}
	*nodelo = Mod(N / M + is.rSid);
	*nodela = 0.0;
}

global_directories_t dirs =
{
	{ "",						"" },
	{ "Main",					"..\\main" },
	{ "Ephemeris",				"..\\ephemeris" },
	{ "Charts",					"..\\charts" },
	{ "Interpretations",		"..\\interpretations" },
	{ "Miscellaneous",			"..\\miscellaneous" },
	{ "American Atlas",			"..\\atlas\\american" },
	{ "International Atlas",	"..\\atlas\\international" },
};

void SetEphemerisPath(void)
{
	file_name_t path;

	// Build an ephemeris path in the same way as FileOpen() in io.c 
	// searchs for a file. First look in the current directory, then 
	// in the executable directory, and finally in the ephemeris directory.
	sprintf(path, "." ";" "%s" ";" "%s",
		dirs.executable.dir, dirs.ephemeris.dir);

	// now set the directory
	swe_set_ephe_path(path);
}

/* This is used by the chart calculation routine to calculate the positions */
/* of the fixed stars. Since the stars don't move in the sky over time, 	*/
/* getting their positions is mostly just reading info from an array and	*/
/* converting it to the correct reference frame. However, we have to add	*/
/* in the correct precession for the tropical zodiac, and sort the final	*/
/* index list based on what order the stars are supposed to be printed in.	*/
BOOL FileFind(const char* szFile, char* szDir, char* path_found)
{
	file_name_t file_name;
	const char* mode = "r";
	FILE* file;


	// First look for file in the current directory
	strcpy(file_name, szFile);
	file = fopen(file_name, mode);

	if (!file)
	{
		// Next look in the executable directory
		sprintf(file_name, "%s\\%s", dirs.executable.dir, szFile);
		file = fopen(file_name, mode);

		if (!file)
		{
			// Finally look in the directory passed as parameter
			sprintf(file_name, "%s\\%s", szDir, szFile);
			file = fopen(file_name, mode);

			if (!file)
			{
				return FALSE; // no luck finding file
			}
		}
	}

	fclose(file);
	if (path_found)
	{
		strcpy(path_found, file_name);
	}

	return TRUE;
}
const char* st[75] = {
"Achernar",
"Polaris",
"ZetaRetic",
"Alcyone",
"Aldebaran",
"Capella",
"Rigel",
"Bellatrix",
"Elnath",
"Alnilam",
"Betelgeuse",
"Menkalinan",
"Mirzam",
"Canopus",
"Alhena",
"Sirius",
"Adhara",
"Wezen",
"Castor",
"Procyon",
"Pollux",
"Suhail",
"Avior",
"Miaplacidus",
"Alphard",
"Regulus",
"Dubhe",
"Acrux",
"Gacrux",
"Mimosa",
"Alioth",
"Spica",
"Alkaid",
"Agena",
"Arcturus",
"Toliman",
"Antares",
"Shaula",
"Sargas",
"Kaus",
"Vega",
"Altair",
"Peacock",
"Deneb",
"Alnair",
"Fomalhaut",
"Andromeda",
"Alpheratz",
"Algenib",
"Schedar",
"Mirach",
"Alrischa",
"Almac",
"Algol",
"Mintaka",
"Wasat",
"Acubens",
"Merak",
"Vindemiatrix",
"Mizar",
"Kochab",
"Zubenelgenubi",
"Zubeneshamali",
"Alphecca",
"Unukalhai",
"Rasalhague",
"Albireo",
"Alderamin",
"Nashira",
"Skat",
"Scheat",
"Markab",
"Gal. Center",
"Apex",
"Sadalmelik"
};

// 5.41G used SE of 02/12/1998
BOOL CalculateStarSE(int ind, double jd, double* longitude, double* latitude)
{
	long flag;
	char serr[AS_MAXCH];
	char iobj[SE_MAX_STNAME * 2];
	char namebuf[SE_MAX_STNAME * 2];
	double jde, SE_coordinates[6];
	int pos;
	char* nomname;


	//sprintf(iobj, "%i", ind);
	flag = SEFLG_SWIEPH;
	jde = jd + swe_deltat(jd);
	strcpy(iobj, st[ind - 1]);

	if (swe_fixstar(iobj, jde, flag, SE_coordinates, serr) >= 0)
	{
		*longitude = SE_coordinates[0];
		*latitude = SE_coordinates[1];
		//rStarBright[ind] = SE_coordinates[2];
		nomname = (char*)MEM_CCPY(namebuf, iobj, ',', SE_MAX_STNAME + 1);
		pos = nomname - namebuf;
		if (pos > starMaxName)
			namebuf[starMaxName] = chNull;
		else
			namebuf[pos - 1] = chNull;
		//szObjName[ind + starLo - 1] = STR_DUP(namebuf);
		//Lang.SetTableItem(tObjName, ind + starLo - 1, char_to_wchar(namebuf).c_str());
		strcpy(namebuf, iobj);
		namebuf[pos + starMaxNName] = chNull;
		szStarConName[ind] = STR_DUP(nomname);

		return TRUE;
	}

	if (us.fSeconds)
	{
		//		ErrorEphem(SE_STARFILE, ind);  
	}
	return FALSE;
}

/* Compare two strings. Return 0 if they are equal, a positive value if  */
/* the first string is greater, and a negative if the second is greater. */
int NCompareSz(const char* s1, const char* s2)
{
	while (*s1 && *s1 == *s2)
		s1++, s2++;
	return *s1 - *s2;
}

int NCompareSzW(const wchar_t* s1, const wchar_t* s2)
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

void ComputeStars(double SD)
{
	int i, j;
	double x, y, z;
	double lon, lat, lonMC, latMC, lonz, latz, azi, alt;

	BOOL HaveStarFile;
	double t_plac, longitude, latitude;

	HaveStarFile = TRUE;
	t_plac = JulianDayFromTime(is.T);

	if (!FileFind(sefstarsName, dirs.main.dir, NULL))
	{
		HaveStarFile = FALSE;
		//		if (us.fSeconds)
		//			ErrorEphem(L"sefstars.txt", -1);
	}

	/* If need, prepare to calculate Prime Vertical of stars. */

	if (us.nStar == 'p' || PrimeRest > 0)
	{
		lon = Deg2Rad(Mod(Longit));
		lat = Deg2Rad(Latit);
		if (us.fEquator)
		{
			lonMC = Deg2Rad(cp0.longitude[oMC]);
			latMC = Deg2Rad(cp0.latitude[oMC]);
		}
		else
		{
			lonMC = Deg2Rad(Tropical(cp0.longitude[oMC]));
			latMC = Deg2Rad(cp0.latitude[oMC]);
			EclToEqu(&lonMC, &latMC);
		}
		if (PolarMCflip && hRevers)
			lonMC = Mod(lonMC + rPi);
	}

	/* Read in star positions. */

	for (i = 1; i <= cStar; i++)
	{
		x = rStarData[i * 6 - 6];
		y = rStarData[i * 6 - 5];
		z = rStarData[i * 6 - 4];
		cp0.longitude[cLastMoving + i] = Deg2Rad(x * 360.0 / 24.0 + y * 15.0 / 60.0 + z * 0.25 / 60.0);
		x = rStarData[i * 6 - 3];
		y = rStarData[i * 6 - 2];
		z = rStarData[i * 6 - 1];
		if (x < 0.0)
		{
			negV(y);
			negV(z);
		}
		else if (fabs(x) == 0.0 && y < 0.0)
		{
			negV(z);
		}
		cp0.latitude[cLastMoving + i] = Deg2Rad(x + y / 60.0 + z / 60.0 / 60.0);
		/* Convert to ecliptic zodiac coordinates. */
		EquToEcl(&cp0.longitude[cLastMoving + i], &cp0.latitude[cLastMoving + i]);
		cp0.longitude[cLastMoving + i] = Mod(Rad2Deg(cp0.longitude[cLastMoving + i]) + rEpoch2000 + SD);
		cp0.latitude[cLastMoving + i] = Rad2Deg(cp0.latitude[cLastMoving + i]);

		if (HaveStarFile)
		{
			if (CalculateStarSE(i, t_plac, &longitude, &latitude))
			{
				cp0.longitude[cLastMoving + i] = Mod(longitude + is.rSid);
				cp0.latitude[cLastMoving + i] = latitude;
			}
		}
		cp0.vel_longitude[cLastMoving + i] = Deg2Rad(360.0 / 26000.0 / 365.25);
		cp0.vel_latitude[cLastMoving + i] = 0.0;
		starname[i] = i;

		/* If sorting by prime vertical needed, compute it. */

		if (us.nStar == 'p' || PrimeRest > 0)
		{
			if (us.fEquator)
			{
				lonz = Deg2Rad(cp0.longitude[cLastMoving + i]);
				latz = Deg2Rad(cp0.latitude[cLastMoving + i]);
			}
			else
			{
				lonz = Deg2Rad(Tropical(cp0.longitude[cLastMoving + i]));
				latz = Deg2Rad(cp0.latitude[cLastMoving + i]);
				EclToEqu(&lonz, &latz);
			}
			lonz = Deg2Rad(Mod(Rad2Deg(lonMC - lonz + lon)));
			lonz = Deg2Rad(Mod(Rad2Deg(lonz - lon + rPiHalf)));
			if (!fEquator)
				EquToLocal(&lonz, &latz, rPiHalf - lat);
			azi = 360.0 - Rad2Deg(lonz);
			alt = Rad2Deg(latz);
			azi = Deg2Rad(azi);
			alt = Deg2Rad(alt);
			CoorXform(&azi, &alt, rPiHalf);
			azi = Rad2Deg(azi);
			alt = Rad2Deg(alt);
			StarPrime[i] = azi;
		}
	}
	if (PrimeRest > 0)
	{
		for (i = 1; i <= cLastMoving; i++)
		{
			if (us.fEquator)
			{
				lonz = Deg2Rad(cp0.longitude[i]);
				latz = Deg2Rad(cp0.latitude[i]);
			}
			else
			{
				lonz = Deg2Rad(Tropical(cp0.longitude[i]));
				latz = Deg2Rad(cp0.latitude[i]);
				EclToEqu(&lonz, &latz);
			}
			lonz = Deg2Rad(Mod(Rad2Deg(lonMC - lonz + lon)));
			lonz = Deg2Rad(Mod(Rad2Deg(lonz - lon + rPiHalf)));
			if (!fEquator)
				EquToLocal(&lonz, &latz, rPiHalf - lat);
			azi = 360.0 - Rad2Deg(lonz);
			alt = Rad2Deg(latz);
			azi = Deg2Rad(azi);
			alt = Deg2Rad(alt);
			CoorXform(&azi, &alt, rPiHalf);
			azi = Rad2Deg(azi);
			alt = Rad2Deg(alt);
			ObjPrime[i] = azi;
		}
	}

	/* Sort the index list if -Uz, -Ul, -Un, or -Ub switch in effect. */

	if (us.nStar > 1)
		for (i = 2; i <= cStar; i++)
		{
			j = i - 1;

			/* Compare star names for -Un switch. */

			if (us.nStar == 'n')
				//while (j > 0 && NCompareSz(szObjName[cLastMoving + starname[j]], szObjName[cLastMoving + starname[j + 1]]) > 0)
				
				while (j > 0 && NCompareSzW(char_to_wchar(tObjName[cLastMoving + starname[j]]).c_str(), char_to_wchar(tObjName[cLastMoving + starname[j + 1]]).c_str()) > 0)
				{
					SwapN(starname[j], starname[j + 1]);
					j--;

					/* Compare star brightnesses for -Ub switch. */

				}
			else if (us.nStar == 'b')
				while (j > 0 && rStarBright[starname[j]] > rStarBright[starname[j + 1]])
				{
					SwapN(starname[j], starname[j + 1]);
					j--;

					/* Compare star zodiac locations for -Uz switch. */

				}
			else if (us.nStar == 'z')
				while (j > 0 && cp0.longitude[cLastMoving + starname[j]] > cp0.longitude[cLastMoving + starname[j + 1]])
				{
					SwapN(starname[j], starname[j + 1]);
					j--;

					/* Compare star declinations for -Ul switch. */

				}
			else if (us.nStar == 'l')
				while (j > 0 && cp0.latitude[cLastMoving + starname[j]] < cp0.latitude[cLastMoving + starname[j + 1]])
				{
					SwapN(starname[j], starname[j + 1]);
					j--;

					/* Compare star prime vertical for -Up switch. */

				}
			else if (us.nStar == 'p')
				while (j > 0 && StarPrime[starname[j]] < StarPrime[starname[j + 1]])
				{
					SwapN(starname[j], starname[j + 1]);
					j--;
				}
		}
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
// this is like function GetOrb, except that it does not consider
// stars, parallel aspects or relationship charts
double GetOrbEx(int obj1, int obj2, int asp)
{
	double orb;

	if (obj1 <= cLastMoving && obj2 <= cLastMoving)
	{
		orb = Min(Min(rAspOrb[asp], rObjOrb[obj1]), rObjOrb[obj2]);
		orb += rObjAdd[obj1];
		orb += rObjAdd[obj2];

		return orb;
	}
	else
	{
		return 0.0;
	}
}


//=======================================================================
/* This is a subprocedure of ComputeInHouses(). Given a zodiac position,  */
/* return which of the twelve houses it falls in. Remember that a special */
/* check has to be done for the house that spans 0 degrees Aries.		  */
int HousePlaceIn(double rDeg)
{
	int i = 0;

	rDeg = Mod(rDeg + 0.5 / 60.0 / 60.0);

	if ((hRevers < 2) || !PolarMCflip)
	{

		do
		{
			i++;
		} while (!(i >= NUMBER_OF_HOUSES ||
			(rDeg >= cp0.cusp_pos[i] && rDeg < cp0.cusp_pos[Mod12(i + 1)]) ||
			(cp0.cusp_pos[i] > cp0.cusp_pos[Mod12(i + 1)] &&
				(rDeg >= cp0.cusp_pos[i] || rDeg < cp0.cusp_pos[Mod12(i + 1)]))));
		return i;

	}
	else
	{
		do
		{
			i++;
		} while (!(i >= NUMBER_OF_HOUSES ||
			(rDeg <= cp0.cusp_pos[i] && rDeg > cp0.cusp_pos[Mod12(i + 1)]) ||
			(cp0.cusp_pos[i] < cp0.cusp_pos[Mod12(i + 1)] &&
				(rDeg <= cp0.cusp_pos[i] || rDeg > cp0.cusp_pos[Mod12(i + 1)]))));
		return i;
	}
}


const char *szDir[4] = {"North", "East", "South", "West"};
enum _housemodel {
  hmEcliptic = 0,   // Standard 2D: Equator of houses is ecliptic
  hmPrime    = 1,   // 3D: Equator of houses is prime vertical
  hmHorizon  = 2,   // 3D: Equator of houses is local horizon
  hmEquator  = 3,   // 3D: Equator of houses is celestial equator
  cMethod = 3,
};

#define FNearR(r1, r2) (RAbs((r1) - (r2)) < 0.001)

double RHousePlaceIn3DCore(double rLon, double rLat)
{
  double lon, lat;

  us.nHouse3D = hmPrime;
  lon = Tropical(rLon); lat = rLat;
  EclToEqu2(&lon, &lat);
  lon = Mod(is.lonMC - lon + rDegQuad);
  if (us.nHouse3D == hmPrime)
    EquToLocal2(&lon, &lat, -Lat);
  else if (us.nHouse3D == hmHorizon)
    EquToLocal2(&lon, &lat, rDegQuad - Lat);
  lon = rDegMax - lon;
  return Mod(lon + rSmall);
}

// Compute 3D houses, or the house postion of a 3D location. Given a zodiac
// position and latitude, return the house position as a decimal number, which
// includes how far through the house the coordinates are.
double RHousePlaceIn3D(double rLon, double rLat)
{
  double deg, rRet;
  int i, di;

  // Campanus houses arranged along the prime vertical are equal sized in 3D,
  // as are a couple other combinations, and so are a simple case to handle.
  deg = RHousePlaceIn3DCore(rLon, rLat);
  if ((us.nHouseSystem == hsCampanus && us.nHouse3D == hmPrime) ||
    (us.nHouseSystem == hsHorizon && us.nHouse3D == hmHorizon) ||
    (us.nHouseSystem == hsMeridian && us.nHouse3D == hmEquator))
    return deg;

  // Determine which 3D house the prime vertical degree falls within.
  di = MinDifference(cp0.cusp3[1], cp0.cusp3[2]) >= 0.0 ? 1 : -1;
  i = 0;
  do 
  {
    i++;
  } 
  while (!(i >= cSign ||
    (deg >= cp0.cusp3[i] && deg < cp0.cusp3[Mod12(i + di)]) ||
    (cp0.cusp3[i] > cp0.cusp3[Mod12(i + di)] &&
    (deg >= cp0.cusp3[i] || deg < cp0.cusp3[Mod12(i + di)]))));
  if (di < 0)
    i = Mod12(i - 1);
  rRet = Mod(ZFromS(i) + MinDistance(cp0.cusp3[i], deg) /
    MinDistance(cp0.cusp3[i], cp0.cusp3[Mod12(i + 1)]) * 30.0);
  return rRet;
}


// This is a subprocedure of ComputeInHouses(). Given a zodiac position,
// return which of the twelve houses it falls in. Remember that a special
// check has to be done for the house that spans 0 degrees Aries.
int NHousePlaceIn(double rLon, double rLat)
{
	int i, di;

	// Special processing for 3D houses.
	if (us.fHouse3D && rLat != 0.0)
		return SFromZ(RHousePlaceIn3D(rLon, rLat));

	// This loop also works when house positions decrease through the zodiac.
	rLon = Mod(rLon + rSmall);
	di = MinDifference(cp0.cusp_pos[1], cp0.cusp_pos[2]) >= 0.0 ? 1 : -1;
	i = 0;
	do {
		i++;
	} while (!(i >= cSign ||
		(rLon >= cp0.cusp_pos[i] && rLon < cp0.cusp_pos[Mod12(i + di)]) ||
		(cp0.cusp_pos[i] > cp0.cusp_pos[Mod12(i + di)] &&
			(rLon >= cp0.cusp_pos[i] || rLon < cp0.cusp_pos[Mod12(i + di)]))));
	if (di < 0)
		i = Mod12(i - 1);
	return i;
}

void ComputeInHouses(void)
{
	int i;

	double lonMC = Tropical(is.MC);
	double latMC = 0.0;
	EclToEqu2(&lonMC, &latMC);
	latMC = Lat;
	is.lonMC = lonMC;
	is.latMC = latMC;

	// First determine 3D house cusp offsets.
	if ((us.nHouseSystem == hsCampanus && us.nHouse3D == hmPrime) ||
		(us.nHouseSystem == hsHorizon && us.nHouse3D == hmHorizon) ||
		(us.nHouseSystem == hsMeridian && us.nHouse3D == hmEquator)) {
		// 3D Campanus cusps are always equal sized wedges when distributed
		// along the prime vertical, as are a couple of other combinations.
		for (i = 1; i <= cSign; i++)
			cp0.cusp3[i] = ZFromS(i);
	}
	else
		for (i = 1; i <= cSign; i++)
			cp0.cusp3[i] = RHousePlaceIn3DCore(cp0.cusp_pos[i], 0.0);

	// Loop over each object and place it.
	for (i = 0; i <= cObj; i++)
		cp0.house_no[i] = NHousePlaceIn(cp0.longitude[i], cp0.latitude[i]);

	// Avoid roundoff error by setting houses of objects known definitively.
	if (us.fHouse3D)
	{
		// 3D wedges that are equal sized should always be in corresponding house.
		if ((us.nHouseSystem == hsCampanus && us.nHouse3D == hmPrime) ||
			(us.nHouseSystem == hsHorizon && us.nHouse3D == hmHorizon) ||
			(us.nHouseSystem == hsMeridian && us.nHouse3D == hmEquator)) {
			for (i = cuspLo; i <= cuspHi; i++)
				if ((us.nHouse3D == hmPrime || (i != oAsc && i != oDes)) &&
					FNearR(cp0.cusp_pos[i - cuspLo + 1], cp0.longitude[i]))
					cp0.house_no[i] = i - cuspLo + 1;
			// 3D angles for most systems should always be in the corresponding house.
		}
		else if (us.fHouseAngle)
		{
			if (us.nHouse3D == hmPrime)
			{
				for (i = cuspLo; i <= cuspHi; i += 3)
					if (FNearR(cp0.cusp_pos[i - cuspLo + 1], cp0.longitude[i]))
						cp0.house_no[i] = i - cuspLo + 1;
			}
			else
			{
				for (i = cuspLo + 4; i <= cuspHi; i += 6)
					if (FNearR(cp0.cusp_pos[i - cuspLo + 1], cp0.longitude[i]))
						cp0.house_no[i] = i - cuspLo + 1;
			}
		}
		if (us.nHouse3D == hmHorizon && FNearR(cp0.cusp_pos[7], cp0.longitude[oVtx]))
			cp0.house_no[oVtx] = 7;
		else if (us.nHouse3D == hmEquator && FNearR(cp0.cusp_pos[1], cp0.longitude[oEP]))
			cp0.house_no[oEP] = 1;
	}
}


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
		gregflag = FALSE;
	else
		gregflag = TRUE;

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
		isDayBirth = TRUE;
	}
	else
	{
		isDayBirth = FALSE;
	}

	ciCore.sunset = rise_set[1];
	ciCore.sunrise = rise_set[0];

	// refresh menu
	//EnableMenuItem(wi.hmenu, cmdFirdaria3, !isDayBirth);
	//EnableMenuItem(wi.hmenu, cmdFirdaria1, isDayBirth);
	//EnableMenuItem(wi.hmenu, cmdFirdaria2, isDayBirth);

	return 0;
}

#if defined(_WIN32)
void x_memset_Intel(void* dest, int nByte, size_t count)
{
	__asm {
		push edi
		mov ecx, count
		mov edi, dest
		mov eax, nByte
		mov ebx, ecx
		rep stosb
		pop edi
	}
}
#else
void x_memset_GCC_CLANG(void* dest, int nByte, size_t count)
{
	__asm__ __volatile__(
		"rep stosb"              // 核心指令：重复填充内存
		: /* 无输出操作数 */       // 无返回值
	: "D" (dest),            // 输入：EDI = dest（目标地址）
		"a" (nByte),           // 输入：EAX = nByte（填充值）
		"c" (count)            // 输入：ECX = count（重复次数）
		: "memory", "cc"         // 破坏列表：内存和标志寄存器被修改
		);
}
#endif

double CastChart(BOOL fDate)
{
	CI ci;
	double housetemp[NUMBER_OF_SIGNS + 1], Off = 0.0, vtx, j, l, m, ll, kk, alt1, alt2;
	double SunDirPos, SunDirection, ep1 = 0.0;
	double MoonDirPos, MoonDirection;
	int i, k, n, ii, mi, arcsav;
	byte ignoreT[NUMBER_OBJECTS], ignoreT3_a[NUMBER_OBJECTS];

	is.rSid = 0.0;
	computeRiseSet();

	/* Hack: Time zone +/-24 means to have the time of day be in Local Mean */
	/* Time (LMT). This is done by making the time zone value reflect the	*/
	/* logical offset from GMT as indicated by the chart's longitude value. */

	if (fabs(ciCore.zon) == 24.0) // 自动计算时区
		ciCore.zon = DecDeg2DegMin(DegMin2DecDeg(ciCore.lon) / 15.0);

	if (us.fProgressUS && !us.nRel && !us.fTransitInf && !us.fTransit && !us.fInDay)
	{
		// calculate with transit datetime
		is.JDp = MdytszToJulian(ciMain.mon, ciMain.day, ciMain.yea, ciMain.tim, ciMain.dst, ciMain.zon); // 推运日期的儒略积日数 2460468.2916666665, 2460468.6666666665
	}

	if (us.fProgressUS && (us.fSolarArc == 2 || us.fSolarArc == 4)) // solar arc and lunar arc
	{
		arcsav = us.fSolarArc;
		us.fSolarArc = 0;
		ci = ciCore;
		Off = ProcessInput(fDate);

		ComputeVariables(&vtx);
		if (us.fGeodetic)		/* Check for -G geodetic chart. */
			is.RAa = Deg2Rad(Mod(-ciCore.lon));

		memcpy(&ignoreT, &ignore1, NUMBER_OBJECTS);
		memcpy(&ignoreT3_a, &ignore3, NUMBER_OBJECTS);

		for (mi = 0; mi <= cObj; mi++)
			ignore1[mi] = ignore3[mi] = TRUE;

		ignore1[oSun] = FALSE;
		ignore3[oSun] = FALSE;
		ignore1[oMoo] = FALSE;
		ignore3[oMoo] = FALSE;
		ComputePlanets();
		ComputeWithSwissEphemeris(is.T);
		SunDirPos = cp0.longitude[oSun];
		MoonDirPos = cp0.longitude[oMoo];

		memcpy(&ignore1, &ignoreT, NUMBER_OBJECTS);
		memcpy(&ignore3, &ignoreT3_a, NUMBER_OBJECTS);

		ignore1[oSun] = FALSE;
		ignore3[oSun] = FALSE;
		ignore1[oMoo] = FALSE;
		ignore3[oMoo] = FALSE;
		ciCore = ci;
		us.fSolarArc = arcsav;
	}

	ci = ciCore;
	Longit = DegMin2DecDeg(ciCore.lon);
	Latit = DegMin2DecDeg(ciCore.lat);

	if (ciCore.mon == -1)
	{

		/* Hack: If month is negative, then we know chart was read in through a  */
		/* -o0 position file, so the cp0.longitude positions are already in the arrays. */

		is.MC = cp0.longitude[oMC];
		is.Asc = cp0.longitude[oAsc];
	}
	else
	{
		X_MEMSET(cp0.longitude, 0, sizeof(double) * cObj);
		for (i = 0; i <= cObj; i++)
		{
			//cp0.longitude[i] = cp0.latitude[i] = 0.0;	/* On ecliptic unless we say so.  */
			cp0.vel_longitude[i] = 1.0;					/* Direct until we say otherwise. */
		}

		Off = ProcessInput(fDate);
		//
		SwissHouse(is.T, ciCore.lon, ciCore.lat * rDegRad, us.nHouseSystem, &is.Asc, &is.MC, &is.RAa, &is.Vtx, &ep1, &is.OB, &Off);
		hRevers = 0;

		if (fabs(ciCore.lat) > Deg2Rad(90.0 - is.OB))
		{
			if (Mod(is.Asc - is.MC) > 180.0)
			{
				hRevers = 1;
			}
		}


		/* Go calculate planet, Moon, and North Node positions. */
		ComputePlanets();
		if (!ignore1[oMoo] || !ignore1[oNoNode] || !ignore1[oSoNode] || !ignore1[oFor])
		{
			ComputeLunar(&cp0.longitude[oMoo], &cp0.latitude[oMoo], &cp0.longitude[oNoNode], &cp0.latitude[oNoNode]);
			cp0.vel_longitude[oNoNode] = -1.0;
		}

		/* Compute more accurate ephemeris positions for certain objects. */
		SetEphemerisPath();

		ComputeWithSwissEphemeris(is.T);

		if (us.objCenter != oEar)
		{
			cp0.vel_longitude[oNoNode] = Deg2Rad(-0.053);
			cp0.vel_longitude[oMoo] = Deg2Rad(12.5);
		}

		// 南交点
		cp0.longitude[oSoNode] = Mod(cp0.longitude[oNoNode] + 180.0);
		cp0.vel_longitude[oSoNode] = cp0.vel_longitude[oNoNode];

		j = cp0.longitude[oMoo] - cp0.longitude[oSun];

		if (us.nArabicNight < 0) // Invert always
		{
			if (isDayBirth)
			{
				negV(j);
			}
		}

		// The first setting and the second setting must give the same result if the birth time is in daybirth hours.
		if (us.nArabicNight == 0) //Invert Night
		{
			if (isDayBirth == 0)
				negV(j);
		}

		// 福点
		j = fabs(j) < 90.0 ? j : j - RSgn(j) * 360.0;
		cp0.longitude[oFor] = Mod(j + is.Asc);

		/* Fill in "planet" positions corresponding to house cusps. */
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
		for (i = oFor; i <= cuspHi; i++)
			cp0.vel_longitude[i] = Deg2Rad(360.0);
	}

	/* Go calculate star positions if -U switch in effect. */
	if (us.nStar)
		ComputeStars(us.fSidereal ? 0.0 : -Off);

	/* Transform ecliptic to equatorial coordinates if -sr in effect. */
	if (us.fEquator && !us.fPrimDirs && !us.fPrimDirs3 && !IsPDsChartWithoutTable && !UsePDsInChart)
	{
		for (i = 0; i <= cObj; i++)
		{
			if (!ignore1[i])
			{
				cp0.longitude[i] = Deg2Rad(Tropical(cp0.longitude[i]));
				cp0.latitude[i] = Deg2Rad(cp0.latitude[i]);
				EclToEqu(&cp0.longitude[i], &cp0.latitude[i]);
				cp0.longitude[i] = Rad2Deg(cp0.longitude[i]);
				cp0.latitude[i] = Rad2Deg(cp0.latitude[i]);
			}
		}

		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		{
			l = 0.0;
			cp0.cusp_pos[i] = Deg2Rad(Tropical(cp0.cusp_pos[i]));
			EclToEqu(&cp0.cusp_pos[i], &l);
			cp0.cusp_pos[i] = Rad2Deg(cp0.cusp_pos[i]);
		}
	}

	/* Alternative star restrictions - by ecliptic and prime vertical */
	if (us.nStar)
	{
		if (StarRest && !PrimeRest)
		{
			for (i = starLo; i <= starHi; i++)
				ignore1[i] = 1;

			if (!us.fParallel)
			{
				for (i = starLo; i <= starHi; i++)
				{
					for (k = 1; k <= uranHi; k++)
					{
						if (!ignore1[k])
						{
							for (n = 1; n <= StarAspects; n++)
							{
								l = MinDistance(cp0.longitude[i], cp0.longitude[k]);
								m = l - rAspAngle[n];
								if (fabs(m) < GetOrb(i, k, n))
									ignore1[i] = 0;
							}
						}
					}
				}
			}
			else
			{
				for (i = starLo; i <= starHi; i++)
				{
					for (k = 1; k <= uranHi; k++)
					{
						if (!ignore1[k] && FThing(k))
						{
							ll = Deg2Rad(cp0.longitude[k]);
							alt1 = Deg2Rad(cp0.latitude[k]);
							EclToEqu(&ll, &alt1);
							alt1 = Rad2Deg(alt1);
							ll = Deg2Rad(cp0.longitude[i]);
							alt2 = Deg2Rad(cp0.latitude[i]);
							EclToEqu(&ll, &alt2);
							alt2 = Rad2Deg(alt2);
							kk = fabs(alt1 - alt2);
							if (kk < GetOrb(i, k, aCon))
								ignore1[i] = 0;
							else if (StarAspects > 1)
							{
								kk = fabs(alt2) - fabs(alt1);
								if (fabs(kk) < GetOrb(i, k, aCon))
									ignore1[i] = 0;
							}
						}
					}
				}
			}
		}
		if (PrimeRest)
		{
			for (i = 1; i <= cStar; i++)
			{
				ii = starLo + i - 1;
				ignore1[ii] = 1;
				if (PrimeRest != 2)
				{
					for (k = 1; k <= uranHi; k++)
					{
						if (!ignore1[k] && FThing(k))
						{
							l = StarPrime[i] - ObjPrime[k];
							if (fabs(l) < PrimeOrb1)
								ignore1[ii] = 0;
						}
					}
				}
				if (PrimeRest > 1)
				{
					for (k = 0; k <= 8; k++)
					{
						l = StarPrime[i] - k * 45.0;
						if (fabs(l) < PrimeOrb2)
							ignore1[ii] = 0;
					}
				}
			}
		}
	}

	/* Now, we may have to modify the base positions we calculated above  */
	/* based on what type of chart we are generating.					  */

	/* Bugfix by VA. When "degree per day/month" charts are calculatd,	  */
	/* is.JDp - is.JD gives wrong difference in time, because is.JD is	  */
	/* calculated for midnigth of birthday (time is set to 00:00:00 GMT), */
	/* so the birthtime isn't used and time difference (in days) is off   */
	/* just by birthtime which gives error in positions up to 10 arcsec.  */
	/* When is.JD below is replaced by JulianDayFromTime(is.T) which is   */
	/* real Julian Day for birthtime, time difference is still off, this  */
	/* time exactly by 12 hours, because Julian Day starts from midday,   */
	/* not from midnight (as in case of is.JDp) and in subroutine		  */
	/* ProcessInput() time is.T is shifted appropriately. (Just compare   */
	/* calculation of is.T in named routine with reverse caclulation	  */
	/* JulianDayFromTime(is.T) and see this shift by 0.5 day). Therefore  */
	/* we have substract here the same 12 ours (or 0.5 days).			  */

	if (us.fProgressUS && us.fSolarArc)
	{							/* Are we doing -p0 solar arc chart? */
		if (us.fSolarArc == 1)
		{
			for (i = 0; i <= cObj; i++)
				cp0.longitude[i] = Mod(cp0.longitude[i] + (is.JDp - JulianDayFromTime(is.T) - 0.5) / us.rProgDay);

			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + (is.JDp - JulianDayFromTime(is.T) - 0.5) / us.rProgDay);
		}
		if (us.fSolarArc == 2)
		{
			SunDirection = SunDirPos - cp0.longitude[oSun];

			if (fConverseArc)
				SunDirection *= -1;

			for (i = 0; i <= cObj; i++)
				cp0.longitude[i] = Mod(cp0.longitude[i] + SunDirection);
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + SunDirection);


			memcpy(&ignore1, &ignoreT, NUMBER_OBJECTS);
			memcpy(&ignore3, &ignoreT3_a, NUMBER_OBJECTS);
		}
		if (us.fSolarArc == 4)
		{
			MoonDirection = MoonDirPos - cp0.longitude[oMoo];
			if (fConverseArc)
				MoonDirection *= -1;
			for (i = 0; i <= cObj; i++)
				cp0.longitude[i] = Mod(cp0.longitude[i] + MoonDirection);
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
				cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + MoonDirection);

			memcpy(&ignore1, &ignoreT, NUMBER_OBJECTS);
			memcpy(&ignore3, &ignoreT3_a, NUMBER_OBJECTS);
		}
	}

	if (us.nHarmonic != 1.0)		/* Are we doing a -x harmonic chart?	 */
	{
		for (i = 0; i <= cObj; i++)
		{
			cp0.longitude[i] = Mod(cp0.longitude[i] * us.nHarmonic);
		}
	}

	if (us.objOnAsc)
	{
		if (us.objOnAsc > 0)	/* Is -1 put on Ascendant in effect?	 */
			j = cp0.longitude[us.objOnAsc] - is.Asc;
		else					/* Or -2 put object on Midheaven switch? */
			j = cp0.longitude[-us.objOnAsc] - is.MC;
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)	/* If so, rotate the houses accordingly. */
			cp0.cusp_pos[i] = Mod(cp0.cusp_pos[i] + j);
	}

	/* Check to see if we are -F forcing any objects to be particular values. */

	for (i = 0; i <= cObj; i++)
	{
		if (force[i] != 0.0)
		{
			cp0.longitude[i] = force[i] - 360.0;
			cp0.latitude[i] = cp0.vel_longitude[i] = 0.0;
		}
	}

	ComputeInHouses();			/* Figure out what house everything falls in. */

	/* If -f domal chart switch in effect, switch cp0.longitude and house positions. */

	if (us.fFlip)
	{
		for (i = 0; i <= cObj; i++)
		{
			k = cp0.house_no[i];
			cp0.house_no[i] = Z2Sign(cp0.longitude[i]);
			cp0.longitude[i] = Sign2Z(k) + MinDistance(cp0.cusp_pos[k], cp0.longitude[i]) /
				MinDistance(cp0.cusp_pos[k], cp0.cusp_pos[Mod12(k + 1)]) * 30.0;
		}
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
		{
			k = HousePlaceIn(Sign2Z(i));
			housetemp[i] = Sign2Z(k) + MinDistance(cp0.cusp_pos[k], Sign2Z(i)) /
				MinDistance(cp0.cusp_pos[k], cp0.cusp_pos[Mod12(k + 1)]) * 30.0;
		}
		for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			cp0.cusp_pos[i] = housetemp[i];
	}

	/* If -3 decan chart switch in effect, edit cp0.longitude positions accordingly. */

	if (us.fDecan)
	{
		for (i = 0; i <= cObj; i++)
			cp0.longitude[i] = Decan(cp0.longitude[i]);
		//ComputeInHouses();
	}

	/* If -9 navamsa chart switch in effect, edit positions accordingly. */

	if (us.fNavamsa)
	{
		for (i = 0; i <= cObj; i++)
			cp0.longitude[i] = Navamsa(cp0.longitude[i]);
		//ComputeInHouses();
	}

	for (i = 0; i <= cObj; i++)
	{
		if (!ignore1[i])
			cp0.dist[i] = PtLen(cp0.pt[i]);
	}

	ciCore = ci;

	return is.T;
}

/* Look, if cp0.longitude[i] is listed as ruler cp0.longitude in rules[]  */
/* Used by XChartDispositor() and by PlanetPPower()         */
int RulerPlanet(int i) 
{
	int j, IsRuler = 0;

	for (j = 1; j <= NUMBER_OF_SIGNS; j++)
	{
		if (i == rules[j])
			IsRuler = 1;
	}
	return IsRuler;
}


/* Calculate position part of cp0.longitude powers. Used by calculation of total */
/* cp0.longitude powers, also by calculating of aspects powers.                  */
void PlanetPPower() //  for ppower1[]
{
	double x, Hy;
	int xSub, i, j, l, Rulers = 12;
	int oDis[cLastMoving + 1], dLev[cLastMoving + 1], cLev[cLastMoving + 1];
	int dLevH[cLastMoving + 1], dLevS[cLastMoving + 1], dLevHd[cLastMoving + 1];
	byte SaveRest[cLastMoving + 1], SaveStar;
	wchar_t c;
	CP cpT;

	cpT = cp0;

	for (i = 1; i <= NUMBER_OF_SIGNS - 1; i++)
		for (j = i + 1; j <= NUMBER_OF_SIGNS; j++)
			if ((rules[i] == rules[j]) && (i != j))
				Rulers -= 1;

	for (i = 0; i <= cLastMoving; i++)
		SaveRest[i] = ignore1[i];
	for (i = 0; i <= cLastMoving; i++)
		if (RulerPlanet(i))
			ignore1[i] = 0;

	SaveStar = us.nStar;
	us.nStar = FALSE;
	CastChart(TRUE);

	if (us.nHouseSystem != cHouseNull)
		Hy = 1.0;
	else
		Hy = 0.0;

	ppower1[0] = rObjInf[0];
	for (i = 0; i <= cLastMoving; i++)
	{
		if (ignore1[i] || !FThing(i))
			ppower1[i] = rObjInf[i];
		else
			ppower1[i] = 0.0;
	}
	for (i = 0; i <= cLastMoving; i++)
	{
		dLevS[i] = 0;
		dLevH[i] = 0;
		dLevHd[i] = 0;
	}

	/* Looking for dispositors, look xcharts1.c, XChartDispositor  */
	for (xSub = 0; xSub <= 2; xSub++)
	{
		for (i = 0; i <= cLastMoving; i++)
		{
			dLev[i] = 1;
			if (xSub == 0)
				oDis[i] = rules[Z2Sign(cp0.longitude[i])];
			if (xSub == 1)
				oDis[i] = rules[cp0.house_no[i]];
			if (xSub == 2)
				oDis[i] = rules[Z2Sign(cp0.cusp_pos[cp0.house_no[i]])];
		}
		do
		{
			j = FALSE;
			for (i = 0; i <= cLastMoving; i++)
				cLev[i] = FALSE;
			for (i = 0; i <= cLastMoving; i++)
				if (dLev[i] && RulerPlanet(i))
					cLev[oDis[i]] = TRUE;
			for (i = 0; i <= cLastMoving; i++)	/* A cp0.longitude isn't a final dispositor */
			{
				if (dLev[i] && !cLev[i] && RulerPlanet(i))
				{				/* if nobody is pointing to it.      */
					dLev[i] = 0;
					j = TRUE;
				}
			}
		} while (j);

		for (i = 0; i <= cLastMoving; i++)
		{
			if (!RulerPlanet(i))
				dLev[i] = 0;
		}

		do
		{
			j = FALSE;
			for (i = 0; i <= cLastMoving; i++)
			{
				if (RulerPlanet(i))
				{
					if (!dLev[i])
					{
						if (!dLev[oDis[i]])
							j = TRUE;
						else	/* If my dispositor already has */
							dLev[i] = dLev[oDis[i]] + 1;	/* a level, mine is one more.   */
					}
				}
			}
			if(j== TRUE) j = FALSE;
		} while (j);

		if (xSub == 0)
		{
			for (i = 0; i <= cLastMoving; i++)
				dLevS[i] = dLev[i];	/* Store sign dispositors info */
		}
		if (xSub == 1)
		{
			for (i = 0; i <= cLastMoving; i++)
				dLevH[i] = dLev[i];	/* Store house dispositors info */
		}
		if (xSub == 2)
		{
			for (i = 0; i <= cLastMoving; i++)
				dLevHd[i] = dLev[i];	/* Store house cusp dispositor info */
		}
	}

	for (i = 0; i <= cLastMoving; i++)
		ignore1[i] = SaveRest[i];
	us.nStar = SaveStar;
	cp0 = cpT;

	/* For each object, find its power based on its placement alone. */

	for (i = 0; i <= cLastMoving; i++)
	{
		if (!ignore1[i] && FThing(i) && i != us.objCenter)
		{
			j = Z2Sign(cp0.longitude[i]);
			ppower1[i] += rObjInf[i];	/* Influence of cp0.longitude itself. */
			ppower1[i] += rHouseInf[cp0.house_no[i]];	/* Influence of house it's in. */

			c = Dignify(i, j);
			if (!wi.chs)
			{
				switch (c)
				{
				case L'R':
					x = rObjInf[cLastMoving + 1];
					break;			/* Planets in signs they rule or */
				case L'F':
					x = rObjInf[cLastMoving + 2];
					break;			/* exalted in have influence.    */
				case L'e':
					x = -rObjInf[i] / 3.0;
					break;
				case L'd':
					x = -rObjInf[i] / 4.0;
					break;
				default:
					x = 0.0;
				}
			}
			else
			{
				switch (c)
				{
				case L'庙':
					x = rObjInf[cLastMoving + 1];
					break;			/* Planets in signs they rule or */
				case L'旺':
					x = rObjInf[cLastMoving + 2];
					break;			/* exalted in have influence.    */
				case L'陷':
					x = -rObjInf[i] / 3.0;
					break;
				case L'落':
					x = -rObjInf[i] / 4.0;
					break;
				default:
					x = 0.0;
				}
			}

			c = Dignify(i, cp0.house_no[i]);
			if (!wi.chs)
			{
				switch (c)
				{
				case L'R':
					x += rHouseInf[NUMBER_OF_SIGNS + 1] / 2.0;
					break;			/* Item in house aligned with */
				case L'F':
					x += rHouseInf[NUMBER_OF_SIGNS + 2] / 2.0;
					break;			/* sign ruled has influence. */
				case L'e':
					x -= rObjInf[i] / 6.0;
					break;
				case L'd':
					x -= rObjInf[i] / 8.0;
					break;
				default:;
				}
			}
			else
			{
				switch (c)
				{
				case L'庙':
					x += rHouseInf[NUMBER_OF_SIGNS + 1] / 2.0;
					break;			/* Item in house aligned with */
				case L'旺':
					x += rHouseInf[NUMBER_OF_SIGNS + 2] / 2.0;
					break;			/* sign ruled has influence. */
				case L'陷':
					x -= rObjInf[i] / 6.0;
					break;
				case L'落':
					x -= rObjInf[i] / 8.0;
					break;
				default:;
				}
			}
			/*  Same additions by situative houses.  */

			c = Dignify(i, Z2Sign(cp0.cusp_pos[cp0.house_no[i]]));
			if (!wi.chs)
			{
				switch (c)
				{
				case L'R':
					x += rHouseInf[NUMBER_OF_SIGNS + 1] / 2.0;
					break;
				case L'F':
					x += rHouseInf[NUMBER_OF_SIGNS + 2] / 2.0;
					break;
				case L'e':
					x -= rObjInf[i] / 6.0;
					break;
				case L'd':
					x -= rObjInf[i] / 8.0;
					break;
				default:;
				}
			}
			else
			{
				switch (c)
				{
				case L'庙':
					x += rHouseInf[NUMBER_OF_SIGNS + 1] / 2.0;
					break;
				case L'旺':
					x += rHouseInf[NUMBER_OF_SIGNS + 2] / 2.0;
					break;
				case L'陷':
					x -= rObjInf[i] / 6.0;
					break;
				case L'落':
					x -= rObjInf[i] / 8.0;
					break;
				default:;
				}
			}

			ppower1[i] += x;
			/* Sign ruler has influence, if cp0.longitude is in this sign.             */
			/* Addition depends of ruling level (see graphics influence chart). */

			if (i != rules[j])
			{
				if (RulerPlanet(i))
					ppower1[rules[j]] += rObjInf[i] / (5.0 * sqrt((double)dLevS[rules[j]]));
				else
					ppower1[rules[j]] += rObjInf[i] / 5.0;
			}

			/* House cusp dispositor has influence, if cp0.longitude is in this house. */
			/* Addition depends of ruling level (see graphics influence chart). */
			/* Influence has both natural and situative rulers of house.        */

			if (i != (j = rules[cp0.house_no[i]]))
			{
				if (RulerPlanet(i))
					ppower1[j] += rObjInf[i] / (10.0 * sqrt((double)dLevH[j]));
				else
					ppower1[j] += rObjInf[i] / 10.0;
			}

			l = Z2Sign(cp0.cusp_pos[cp0.house_no[i]]);
			if (i != rules[l])
			{
				if (RulerPlanet(i))
					ppower1[rules[l]] += Hy * rObjInf[i] / (10.0 * sqrt((double)dLevHd[rules[l]]));
				else
					ppower1[rules[l]] += Hy * rObjInf[i] / 10.0;
			}
		}
	}

	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		/* Various planets get influence */
		j = Z2Sign(cp0.cusp_pos[i]);	/* if house cusps fall in signs  */
		ppower1[rules[j]] += rHouseInf[i] / 2.0;	/* they rule.                    */
	}

	/* And now, when all things have calculated positional powers, give  */
	/* additions to house cusps (as objects), if there are something in  */
	/* this house. */

	for (i = 0; i <= cLastMoving; i++)
	{
		if (!ignore1[i] && !FCusp(i) && i != us.objCenter)
			ppower1[cp0.house_no[i] + cuspLo - 1] += ppower1[i] / 10.0;
	}
}

wchar_t retA[255];
wchar_t *addspace(const wchar_t *str, int num)
{
	wcscpy(retA, str);
	int len = wcslen(str) * 2;
	for (int i = 0; i < (num - len); i++)
		wcscat(retA, L" ");
	return retA;
}

wchar_t *addspaceB(wchar_t *str, int num)
{
	wcscpy(retA, L"");
	int len = strlen(wchar_to_char(str).c_str());
	for (int i = 0; i < (num - len); i++)
		wcscat(retA, L" ");
	wcscat(retA, str);
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
		swprintf(sz,sizeof(sz)/sizeof(wchar_t), L"%ls", tSignName[obj2]);
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

double SphDistance(double lon1, double lat1, double lon2, double lat2)
{
  double dLon, r;

  dLon = RAbs(lon1 - lon2);
  r = RAcosD(RSinD(lat1)*RSinD(lat2) + RCosD(lat1)*RCosD(lat2)*RCosD(dLon));
  return r;
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
  if (FBetween(obj, oJup, oPlu) && (!ignore1[obj - oJup + cobLo]
    ))
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
	BOOL fSwap;

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
				swprintf(szW, sizeof(szW) / sizeof(wchar_t), L" (%s %ls%ls", szEclipse[nEclipse], source == oSun ?
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
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%s %s %s", a1, b1, c1 );
	else
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%s %s %s", a1, b1, c1 );
	PrintSzW(sz);
	PrintSzW(L":");
	PrintSzW(L"\n");

	//AnsiColor(kDefault);
	if(wi.chs)
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%s的",szPerson);
	else
		swprintf(sz, sizeof(sz)/sizeof(wchar_t),L"%s's ",szPerson);

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

enum _rulerships {
	rrStd = 0,  // Standard exoteric
	rrEso = 1,  // Esoteric
	rrHie = 2,  // Hierarchical
	rrExa = 3,  // Exaltation
	rrRay = 4,  // Ray rulership
	rrMax = 5,
  };
  
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
  int rgSignEso1[12+1] = {-1,
	oMer, oVul, oVen, oNep, oSun, oMoo, oUra, oMar, oEar, oSat, oJup, oPlu};
  int rgSignEso2[12+1] = {-1,
	-1, -1, -1, -1, oNep, -1, -1, -1, -1, -1, -1, -1};
  int rgSignHie1[12+1] = {-1,
	oUra, oVul, oEar, oNep, oSun, oJup, oSat, oMer, oMar, oVen, oMoo, oPlu};
  int rgSignHie2[12+1] = {-1,
	-1, -1, -1, -1, oUra, -1, -1, -1, -1, -1, oVul, -1};
  
  int rgObjRay[90] = {3,
	2, 4, 4, 5, 6, 2, 3, 7, 6, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 4, 2, 3, 1, 2, 3, 4, 5, 7, 5, 6,
	1, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 3, 2, 1, 4, 7, 6, 5,    // 3214765: Seven circuit Labyrinth sequence!
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int rgSignRay[12+1] = {0,
	17, 4, 2, 37, 15, 26, 3, 4, 456, 137, 5, 26};
  int rgSignRay2[12+1][cRay+1];
  
  byte ignore7[rrMax] = {0, 1, 1, 0, 1};  // Restrictions for rulership types.
  int rules2[12+1] = {-1,
	-1, -1, -1, -1, -1, -1, -1, oMar, -1, -1, oSat, oJup};
  

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


/* Indicate whether some aspect between two objects should be shown. */
BOOL FAcceptAspect(int obj1, int asp, int obj2) 
{
	int fSupp;

	if (ignoreA[asp])			/* If the aspect restricted, reject immediately. */
		return FALSE;

	if ( (us.nRel > rcDual && !us.fPrimDirs ) && FCusp(obj1) && FCusp(obj2))
		return FALSE;

	if (FStar(obj1) && FStar(obj2))
		return FALSE;

	if (us.fSmartAspects)
	{

		/* Allow only conjunctions to the minor house cusps. */

		if ((FMinor(obj1) || FMinor(obj2)) && asp > aCon)
			return FALSE;

		/* Prevent any simultaneous aspects to opposing angle cusps,	 */
		/* e.g. if conjunct one, don't be opposite the other; if trine	 */
		/* one, don't sextile the other; don't square both at once, etc. */

		fSupp = (asp == aOpp || asp == aSex || asp == aSSx || asp == aSes);

		if ((FAngle(obj1) || FAngle(obj2)) &&
			(fSupp || (asp == aSqu && (obj1 == oDes || obj2 == oDes || obj1 == oNad || obj2 == oNad))))
			return FALSE;

		// Prevent any simultaneous aspects to the North and South Node, or 
		// aspects between the nodes. North Node will always be given the 
		// priority for aspects. An aspect is prevented if there exists another
		// aspect defined in Astrolog, the sum of which totals 180 degrees.

		// This test is not foolproof, because if the user enables biquintiles
		// but disables semiquintiles, for example, he will miss one aspect.

		if (obj1 == oNoNode && obj2 == oSoNode || 
			((obj1 == oSoNode || obj2 == oSoNode) && 
			(asp >= aCon && asp <= aSQn && asp != aQui)))
			return FALSE;
	}
	return TRUE;
}


// Determine the influence of each cp0.longitude's position and aspects. Called from
// the ChartInfluence() routine for the -j chart, and the ChartEsoteric()
// routine for the -7 chart which also makes use of cp0.longitude influences.

////GetParallel(double        *planet1,       double *planet2,      double *planetalt1,       double *planetalt2,      double *ret1,          double *ret2, double *altret1, double *altret2, int i, int j);
int GetParallelA(const double *planet1, const double *planet2,const double *planetalt1, const double *planetalt2,const double *retalt1, const double *retalt2, int i, int j, double *prOrb)
{
  int asp;
  double rDiff, rOrb, azi, alt1, alt2, retalt1a;

  // Compute the declination of the two planets.
  alt1 = planetalt1[j];
  alt2 = planetalt2[i];
  if (!us.fEquator2 && !us.fParallel2) 
  {
    // If have ecliptic latitude and want equatorial declination, convert.
    azi = planet1[j]; 
	EclToEqu2(&azi, &alt1);
    azi = planet2[i]; 
	EclToEqu2(&azi, &alt2);
  } 
  else if (us.fEquator2 && us.fParallel2) 
  {
    // If have equatorial declination and want ecliptic latitude, convert.
    azi = planet1[j]; 
	EquToEcl2(&azi, &alt1);
    azi = planet2[i]; 
	EquToEcl2(&azi, &alt2);
  }

  // Check each vertical aspect type to see if it applies.
  for (asp = 1; asp <= (!us.fDistance ? aOpp : us.nAsp); asp++)
  {
    if (!FAcceptAspect(i, asp, j))
      continue;
    if (asp == aCon)
      rDiff = alt1 - alt2;
    else if (asp == aOpp)
      rDiff = alt1 + alt2;
    else 
	{
      retalt1a = rAspAngle[asp] / rDegHalf;
      if (RAbs(alt1) > RAbs(alt2))
        alt2 /= retalt1a;
      else
        alt1 /= retalt1a;
      rDiff = ((alt1 >= 0.0) == (alt2 >= 0.0) ? alt1 - alt2 : alt1 + alt2);
    }
    rOrb = GetOrbA(i, j, asp);
    if (us.nAppSep == 1) 
	{
      if (FCmSwissAny()) 
	  {
        retalt1a = us.nRel > rcTransit ? retalt1[j] : 0.0;
        rDiff *= RSgn2(retalt1a - retalt2[i]);
      } 
	  else 
	  {
        // If no declination velocity, make aspect separating.
        rDiff = RAbs(rDiff);
      }
    } 
	else if (us.nAppSep == 2) 
	{
      // "Waxing" is if bodies on same side, "waning" if on different sides.
      // (This means Parallel is always waxing, Contraparallel always waning.)
      rDiff = RAbs(rDiff) * ((alt1 >= 0.0) == (alt2 >= 0.0) ? -1.0 : 1.0);
    }

    // If vertical aspect within orb, return it.
    if (RAbs(rDiff) < rOrb) 
	{
      if (prOrb != NULL)
        *prOrb = rDiff;
      return asp;
    }
  }
  return 0;
}

#define space     cp0.pt
int GetDistance(const PT3R *space1, const PT3R *space2,
  const double *retlen1, const double *retlen2, int i, int j, double *prOrb)
{
  int asp;
  double dist1, dist2, rPct, rDiff, rOrb, retlen1a;

  // Compute the distances of and between the two planets.
  dist1 = PtLen(space1[i]); dist2 = PtLen(space2[j]);
  if (us.fSmartCusp && (dist1 <= 0.0 || dist2 <= 0.0))
    return 0;
  if (dist1 <= 0.0 && dist2 <= 0.0)
    rPct = 100.0;
  else
    rPct = (dist1 <= dist2 ? dist1 / dist2 : dist2 / dist1) * 100.0;

  // Check each distance aspect proportion to see if it applies.
  for (asp = 1; asp <= us.nAsp; asp++) {
    if (asp == aOpp || !FAcceptAspect(i, asp, j))
      continue;
    rDiff = rPct - (rAspAngle[asp == aCon ? aOpp : asp] / rDegHalf * 100.0);
    rOrb = GetOrb(i, j, asp);

    // If -ga switch in effect, then change the sign of the orb to correspond
    // to whether the aspect is applying or separating. To do this, check the
    // distance velocity vectors to see if the planets are moving toward,
    // away, or are overtaking each other.

    if (us.nAppSep == 1) {
      if (FCmSwissAny()) {
        retlen1a = us.nRel > rcTransit ? retlen1[i] : 0.0;
        rDiff *= RSgn2(retlen2[j]-retlen1a);
      } else {
        // If no distance velocity, make aspect separating.
        rDiff = RAbs(rDiff);
      }
    } else if (us.nAppSep == 2) {
      // "Waxing" is if nearer body applying, "waning" if nearer separating.
      rDiff = RAbs(rDiff) *
        (dist1 <= dist2 ? RSgn2(retlen1[i]) : RSgn2(retlen2[j]));
    }

    // If distance aspect within orb, return it.
    if (RAbs(rDiff) < rOrb) {
      if (prOrb != NULL)
        *prOrb = rDiff;
      return asp;
    }
  }
  return 0;
}

int GetAspectA(const double *planet1, const double *planet2,
  const double *planetalt1, const double *planetalt2,
  const double *ret1, const double *ret2, int i, int j, double *prOrb)
{
  int asp;
  double rAngle, rAngle3D, rDiff, rOrb, ret1a;

  // Compute the angle between the two planets.
  rAngle = MinDistance(planet1[i], planet2[j]);
  if (us.fAspect3D || us.fAspectLat)
    rAngle3D = SphDistance(planet1[i], planetalt1[i],
      planet2[j], planetalt2[j]);

  // Check each aspect angle to see if it applies.
  //us.nAsp=5;
  for (asp = 1; asp <= us.nAsp; asp++) 
  {
    if (!FAcceptAspect(i, asp, j))
      continue;
    rDiff = (!us.fAspect3D ? rAngle : rAngle3D) - rAspAngle[asp];
    rOrb = GetOrbA(i, j, asp);

    // If -ga switch in effect, then change the sign of the orb to correspond
    // to whether the aspect is applying or separating. To do this, check the
    // velocity vectors to see if the planets are moving toward, away, or are
    // overtaking each other.

    if (us.nAppSep == 1)
	{
      ret1a = us.nRel > rcTransit ? ret1[i] : 0.0;
      rDiff *= RSgn2(ret2[j]-ret1a) * RSgn2(MinDifference(planet1[i],
        planet2[j]));
    } 
	else if (us.nAppSep == 2)
	{
      // The -gx switch means make aspect orb signs reflect waxing/waning.
      ret1a = us.nRel > rcTransit ? ret1[i] : 0.0;
      rDiff = RAbs(rDiff) * RSgn2(ret1a-ret2[j]) *
        RSgn2(MinDifference(planet1[i], planet2[j]));
    }

    // If aspect within orb, return it.
    if (RAbs(rDiff) < rOrb) 
	{
      if (us.fAspectLat &&
        !(RAbs((!us.fAspect3D ? rAngle3D : rAngle) - rAspAngle[asp]) < rOrb))
        continue;
      if (prOrb != NULL)
        *prOrb = rDiff;
      return asp;
    }
  }
  return 0;
}

// Convert 3D rectangular to spherical coordinates.
void RecToSph3(double rx, double ry, double rz, double *azi, double *alt)
{
  double ang, rad;

  RecToPol(rx, ry, &ang, &rad);
  *azi = DFromR(ang);
  ang = RAngleD(rad, rz);
  // Ensure latitude is from -90 to +90 degrees.
  if (ang > rDegHalf)
    ang -= rDegMax;
  *alt = ang;
}

void SphRatio(double lon1, double lat1, double lon2, double lat2, double rRatio,double *lon, double *lat)
{
  double x1, y1, z1, x2, y2, z2, x, y, z, len, ang, adj, ang2;

  SphToRec(1.0, lon1, lat1, &x1, &y1, &z1);
  SphToRec(1.0, lon2, lat2, &x2, &y2, &z2);
  if (rRatio != 0.5){
    // Bisecting an arc is easy, however other proportions require extra math.
    len = RLength3(x2 - x1, y2 - y1, z2 - z1) / 2.0;
    ang = RAsinD(len);
    adj = 1.0 / RTanD(ang);
    rRatio = (rRatio - 0.5) / 0.5;
    ang2 = rRatio * ang;
    rRatio = adj * RTanD(ang2);
    rRatio = (rRatio / 2.0) + 0.5;
  }
  x = x1 + (x2 - x1) * rRatio;
  y = y1 + (y2 - y1) * rRatio;
  z = z1 + (z2 - z1) * rRatio;
  RecToSph3(x, y, z, lon, lat);
}

/* This is Astrolog's memory allocation routine, returning a pointer given  */
/* a size, and if the allocation fails prints a warning, including the user */
/* passed as parameter.														*/
void *allocate(long length, char *user)
{
	void *p;

	p = malloc(length);

	if (p == NULL && user)
	{
		wprintf(L"Can't allocate memory for %s (%d bytes)", user, length);
	}

	return p;
}


/*
******************************************************************************
** Aspect Calculations.
******************************************************************************
*/
/* Set up the aspect/midpoint grid. Allocate memory for this array, if not */
/* already done. Allocation is only done once, first time this is called.  */

BOOL FEnsureGrid()
{
	if (grid != NULL)
		return TRUE;
	grid = (GridInfo *) allocate(sizeof(GridInfo), (char*)"grid");
	memset(grid,0,sizeof(GridInfo));
	return grid != NULL;
}

/* This is a subprocedure of FCreateGrid() and FCreateGridRelation().	*/
/* Given two planets, determine what aspect, if any, is present between */
/* them, and save the aspect name and orb in the specified grid cell.	*/
void GetAspect(double *planet1, double *planet2, double *ret1, double *ret2, int i, int j) 
{
	int k;
	double l, m;

	grid->v[i][j] = grid->n[i][j] = 0;
	l = MinDistance(planet2[i], planet1[j]);

	for (k = cAspect; k >= 1; k--)
	{
		if (!FAcceptAspect(i, k, j))
			continue;
		m = l - rAspAngle[k];

		if (fabs(m) < GetOrb(i, j, k))
		{
			grid->n[i][j] = k;

			/* If -ga switch in effect, then change the sign of the orb to	  */
			/* correspond to whether the aspect is applying or separating.	  */
			/* To do this, we check the velocity vectors to see if the		  */
			/* planets are moving toward, away, or are overtaking each other. */

			if (us.fAppSep)
				m = RSgn2(ret1[j] - ret2[i]) * RSgn2(MinDifference(planet2[i], 
				planet1[j])) * RSgn2(m) * fabs(m);
			grid->v[i][j] = (int) (m * 60.0);
		}
	}
}

/* Very similar to GetAspect(), this determines if there is a parallel or */
/* contraparallel aspect between the given two planets, and stores the	  */
/* result as above. The settings and orbs for conjunction are used for	  */
/* parallel and those for opposition are used for contraparallel.		  */
void GetParallel(double *planet1, double *planet2, double *planetalt1, double *planetalt2, double *ret1, double *ret2, double *altret1, double *altret2, int i, int j) 
{
	int k;
	double l, alt1, alt2, latdiri, latdirj;

	/* Planet 1 */
	l = Deg2Rad(planet1[j]);
	alt1 = Deg2Rad(planetalt1[j]);
	/* No conversion to Geo-equatorial in a heliocentric chart */
	if (us.objCenter != oSun)
		EclToEqu(&l, &alt1);
	alt1 = Rad2Deg(alt1);

	/* Planet 2*/
	l = Deg2Rad(planet2[i]);
	alt2 = Deg2Rad(planetalt2[i]);
	/* No conversion to Geo-equatorial in a heliocentric chart */
	if (us.objCenter != oSun)
		EclToEqu(&l, &alt2);
	/* */
	alt2 = Rad2Deg(alt2);

	grid->v[i][j] = grid->n[i][j] = 0;
	for (k = aOpp; k >= 1; k--)
	{
		if (!FAcceptAspect(i, k, j))
			continue;
		l = (k == aCon ? fabs(alt1 - alt2) : fabs(alt2) - 
			fabs(alt1));
		if (fabs(l) < GetOrb(i, j, k))
		{
			grid->n[i][j] = k;

			if (us.fAppSep)
			{
				latdiri = ret2[i] * RCosD(planet2[i]) * RSinD(Rad2Deg(is.rObliquity))
					+ altret2[i] * RCosD(Rad2Deg
					(is.rObliquity) - fabs(planetalt2[i] - alt2));
				latdirj = ret1[j] * RCosD(planet1[j]) * RSinD(Rad2Deg(is.rObliquity))
					+ altret1[j] * RCosD(Rad2Deg
					(is.rObliquity) - fabs(planetalt1[j] - alt1));
				l = RSgn2(latdirj - latdiri) * RSgn2
					(alt1 - alt2) * RSgn2(l) * fabs(l);
			}
			grid->v[i][j] = (int) (l * 60.0);
		}
	}
}

void ClearB(pbyte pb, int cb)
{
  while (cb-- > 0)
    *pb++ = 0;
}


/* Fill in the aspect grid based on the aspects taking place among the */
/* planets in the present chart. Also fill in the midpoint grid.	   */
BOOL FCreateGrid(BOOL fFlip)
{
	int i, j, k;
	double l;

	if (!FEnsureGrid())
		return FALSE;

	CP cpBak;
/*
-		cp0	{longitude=0x00eac6b8 latitude=0x00eaca68 vel_longitude=0x00eace18 ...}	_ChartPositions
-		longitude	0x00eac6b8 struct _ChartPositions cp0	double [118]
		[0]	0.00000000000000000	double
		[1]	44.984566108684170	double
		[2]	197.74470517259891	double
		[3]	60.599180950545879	double
		[4]	14.524953284407447	double
		[5]	137.23055398673893	double
		[6]	7.3477674453250126	double
		[7]	322.46697152238005	double
		[8]	151.15239902342844	double
		[9]	224.27960518628393	double
		[10]	159.59718121765471	double

-		cp1	{longitude=0x00c88180 latitude=0x00c88530 vel_longitude=0x00c888e0 ...}	_ChartPositions
-		longitude	0x00c88180 struct _ChartPositions cp1	double [118]
		[0]	0.00000000000000000	double
		[1]	103.18804943153673	double
		[2]	258.68664522776169	double
		[3]	116.75887302624810	double
		[4]	74.392616945762953	double
		[5]	192.69827145250628	double
		[6]	66.903115516646366	double
		[7]	18.513486369792965	double
		[8]	208.02847509683596	double
		[9]	283.23785858865949	double
		[10]	218.24444792944507	double
		[11]	168.57823022620073	double

-		cp2	{longitude=0x00eb1310 latitude=0x00eb16c0 vel_longitude=0x00eb1a70 ...}	_ChartPositions
-		longitude	0x00eb1310 struct _ChartPositions cp2	double [118]
		[0]	0.00000000000000000	double
		[1]	103.18804943153673	double
		[2]	258.68664522776169	double
		[3]	116.75887302624810	double
		[4]	74.392616945762953	double
		[5]	192.69827145250628	double
		[6]	66.903115516646366	double
		[7]	18.513486369792965	double
		[8]	208.02847509683596	double
		[9]	283.23785858865949	double
		[10]	218.24444792944507	double
		[11]	168.57823022620073	double

-		cpPDs	{longitude=0x00eae938 latitude=0x00eaece8 vel_longitude=0x00eaf098 ...}	_ChartPositions
-		longitude	0x00eae938 struct _ChartPositions cpPDs	double [118]
		[0]	0.00000000000000000	double
		[1]	103.18804943153673	double
		[2]	258.68664522776169	double
		[3]	116.75887302624810	double
		[4]	74.392616945762953	double
		[5]	192.69827145250628	double
		[6]	66.903115516646366	double
		[7]	18.513486369792965	double
		[8]	208.02847509683596	double
		[9]	283.23785858865949	double
		[10]	218.24444792944507	double
		[11]	168.57823022620073	double

*/
	//if( (UsePDsInChart && IsPDsChartWithoutTable) || UsePDsInChart )
	//{
	//	//cpBak = cp0;
	//	//cp0=cpPDs;
	//	cpBak=cp1;
	//	cp1=cpPDs;
	//}
	if(UsePDsInChart && us.fGrid)
		int a=0;

	if( UsePDsInChart && IsPDsChartWithoutTable )
	{
		cpBak = cp0;
		cp0=cpPDs;
	}

	memset(&grid->n, 0, sizeof(grid->n));

	for (j = 0; j <= cObj; j++)
	{
		if (!FIgnore(j))
		{
			for (i = 0; i <= cObj; i++)
			{
				if (!FIgnore(i))
				{

					/* The parameter 'flip' determines what half of the grid is filled in */
					/* with the aspects and what half is filled in with the midpoints.	  */

					if (fFlip ? i > j : i < j)
					{
						if (us.fParallel)
							GetParallel(cp0.longitude, cp0.longitude, cp0.latitude, cp0.latitude, cp0.vel_longitude, cp0.vel_longitude, 
								cp0.vel_latitude, cp0.vel_latitude, i, j);
						else
							GetAspect(cp0.longitude, cp0.longitude, cp0.vel_longitude, cp0.vel_longitude, i, j);
					}
					else if (fFlip ? i < j : i > j)
					{
						l = Mod(Midpoint(cp0.longitude[i], cp0.longitude[j]));
						k = (int) l;				/* Calculate */
						grid->n[i][j] = k / 30 + 1; /* midpoint. */
						grid->v[i][j] = (int) ((l - (double) (k / 30) * 30.0) * 60.0);
					}
					else
					{
						grid->n[i][j] = Z2Sign(cp0.longitude[j]);
						grid->v[i][j] = (int) (cp0.longitude[j] - (double) (grid->n[i][j] - 1) * 30.0);
					}
				}
			}
		}
	}

    //if( (UsePDsInChart && IsPDsChartWithoutTable) || UsePDsInChart )
	//	cp1=cpBak;
    if( UsePDsInChart && IsPDsChartWithoutTable )
		cp0=cpBak;

	return TRUE;
}


//#define FIgnore(i)  ( ignore1[i] || (i) == us.objCenter || (i) == oLil && us.objCenter != oEar)
BOOL FCreateGridA(BOOL fFlip)
{
  int x, y, k, asp;
  double l, rOrb=0.0, rT=0.0;

  is.nObj=42;
  if (!FEnsureGrid())
    return FALSE;
  ClearB((pbyte)grid, sizeof(GridInfo));

  for (y = 0; y <= is.nObj; y++) if (!ignore1[y])
  {
    for (x = 0; x <= is.nObj; x++) if (!ignore1[x])
	{
      if (fFlip ? x > y : x < y) 
	  {
        if (us.fParallel)
          asp = GetParallelA(cp0.longitude, cp0.longitude, cp0.latitude, cp0.latitude,cp0.vel_latitude, cp0.vel_latitude, x, y, &rOrb);
        else if (us.fDistance)   
          asp = GetDistance(cp0.pt, cp0.pt, cp0.vel_distance, cp0.vel_distance, x, y, &rOrb);
        else
          asp = GetAspectA(cp0.longitude, cp0.longitude, cp0.latitude, cp0.latitude,cp0.vel_longitude, cp0.vel_longitude, x, y, &rOrb);
        grid->n[x][y] = asp;
        grid->v[x][y] = asp > 0 ? rOrb : 0.0;
      } 
	  else if (fFlip ? x < y : x > y)
	  {
        // Calculate midpoint in 2D or 3D.
        if (!us.fHouse3D)
          l = Mod(Midpoint(cp0.longitude[x], cp0.longitude[y]));
        else
          SphRatio(cp0.longitude[x], cp0.latitude[x], cp0.longitude[y], cp0.latitude[y], 0.5,
            &l, &rT);
        k = SFromZ(l);
        grid->n[x][y] = k;
        grid->v[x][y] = l - (double)((k-1)*30);
      } 
	  else 
	  {
        l = cp0.longitude[y];
        k = SFromZ(l);
        grid->n[x][y] = k;
        grid->v[x][y] = l - (double)((k-1)*30);
      }
	}
  }
  return TRUE;
}


void ComputeInfluence() // for power1,power2
{
  int i, j, k, l;
  double x;
  char *c;

  is.nObj=42;

  for (i = 0; i <= is.nObj; i++)
    power1[i] = power2[i] = 0.0;

  // First, for each object, find its power based on its placement alone.

  for (i = 0; i <= is.nObj; i++) 
  {
	  if (!FIgnore(i)) 
	  {
		  j = SFromZ(cp0.longitude[i]);
		  power1[i] += RObjInfA(i);               // Influence of cp0.longitude itself.
		  power1[i] += rHouseInfA[cp0.house_no[i]];    // Influence of house it's in.
		  x = 0.0;
		  c = DignifyA(i, j);
		  int a=84;
		  if (c[rrStd+1] == 'R') 
			  x += rObjInfA[a+1];  // Planets in signs they
		  if (c[rrExa+1] == 'X') 
			  x += rObjInfA[a+2];  // rule or are exalted
		  if (c[rrEso+1] == 'S') 
			  x += rObjInfA[a+3];  // in have influence.
		  if (c[rrHie+1] == 'H') 
			  x += rObjInfA[a+4];
		  if (c[rrRay+1] == 'Y') 
			  x += rObjInfA[a+5];
		  c = DignifyA(i, cp0.house_no[i]);
		  if (c[rrStd+1] == 'R') 
			  x += rHouseInfA[12+1];  // Planets in houses
		  if (c[rrExa+1] == 'X') 
			  x += rHouseInfA[12+2];  // aligned with sign
		  if (c[rrEso+1] == 'S') 
			  x += rHouseInfA[12+3];  // ruled or exalted
		  if (c[rrHie+1] == 'H') 
			  x += rHouseInfA[12+4];  // in have influence.
		  if (c[rrRay+1] == 'Y') 
			  x += rHouseInfA[12+5];
		  power1[i] += x;
		  x = RObjInfA(i)/2.0;
		  if (!ignore7[rrStd]) 
		  {
			  // Planet ruling sign and house current cp0.longitude is in, gets influence.
			  k = rulesA[j];           
			  if (k > 0 && i != k)
				  power1[k] += x;
			  k = rules2A[j];          
			  if (k > 0 && i != k) 
				  power1[k] += x;
			  k = rulesA[cp0.house_no[i]];  
			  if (k > 0 && i != k) 
				  power1[k] += x;
			  k = rules2A[cp0.house_no[i]]; 
			  if (k > 0 && i != k) 
				  power1[k] += x;
		  }
		  if (!ignore7[rrEso]) 
		  {
			  k = rgSignEso1[j];          
			  if (k > 0 && i != k) 
				  power1[k] += x;
			  k = rgSignEso2[j];          
			  if (k > 0 && i != k) 
				  power1[k] += x;
			  k = rgSignEso1[cp0.house_no[i]]; 
			  if (k > 0 && i != k) 
				  power1[k] += x;
			  k = rgSignEso2[cp0.house_no[i]]; 
			  if (k > 0 && i != k) 
				  power1[k] += x;
		  }
		  if (!ignore7[rrHie]) 
		  {
			  k = rgSignHie1[j];          
			  if (k > 0 && i != k) 
				  power1[k] += x;
			  k = rgSignHie2[j];          
			  if (k > 0 && i != k) 
				  power1[k] += x;
			  k = rgSignHie1[cp0.house_no[i]]; 
			  if (k > 0 && i != k) 
				  power1[k] += x;
			  k = rgSignHie2[cp0.house_no[i]]; 
			  if (k > 0 && i != k) 
				  power1[k] += x;
		  }
	  }
  }
  for (i = 1; i <= 12; i++) 
  {         
	  // Various planets get influence
	  j = SFromZ(cp0.cusp_pos[i]);       // if house cusps fall in signs
	  power1[rulesA[j]] += rHouseInfA[i];    // they rule.
	  if (!ignore7[rrEso]) 
	  {
		  k = rgSignEso1[j]; 
		  if (k) 
			  power1[k] += rHouseInfA[i];

		  k = rgSignEso2[j]; 
		  if (k) 
			  power1[k] += rHouseInfA[i];
	  }
	  if (!ignore7[rrHie]) 
	  {
		  k = rgSignHie1[j]; 
		  if (k) 
			  power1[k] += rHouseInfA[i];

		  k = rgSignHie2[j]; 
		  if (k) 
			  power1[k] += rHouseInfA[i];
	  }
  }

  // Second, for each object, find its power based on aspects it makes.

  if (!FCreateGridA(FALSE))
	  return;
  for (j = 0; j <= is.nObj; j++) if (!ignore1[j])
  {
	  for (i = 0; i <= is.nObj; i++) if (!ignore1[i] && i != j) 
	  {
		  k = grid->n[Min(i, j)][Max(i, j)];
		  if (k)
		  {
			  l = (int)(grid->v[Min(i, j)][Max(i, j)]*3600.0);
			  power2[j] += rAspInfA[k]*rObjInfA[i]*(1.0-RAbs((double)l)/3600.0/GetOrbA(i, j, k));
		  }
	  }
  }
}

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
	int asp;

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
	PlanetPPower();
	//ComputeInfluence();

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
						if (k = tempgrid.n[j][i])
						{
							ip = i <= cLastMoving ? ppower1[i] : 2.5;
							jp = j <= cLastMoving ? ppower1[j] : 2.5;
							orb = tempgrid.v[j][i];
							p = rAspInf[k] * sqrt(ip * jp) * (1.0 - fabs((double)orb) / 60.0 / GetOrb(i, j, k));
						
							if (us.fParallel)
								p *= PowerPar;
						
							// here we sort by decreasing power or increasing orb
							if (fSortAspectsByOrbs && abs(orb) < abs(saveorb) || !fSortAspectsByOrbs && p > savepower)
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

void ClearUserSettings(void)
{
	/* Chart types */

	us.fListing = FALSE;
	us.fWheel = FALSE;
	us.fGrid = FALSE;
	us.fAspList = FALSE;
	us.fMidpoint = FALSE;
	us.fDifference = FALSE;
	us.fHorizon = FALSE;
	us.fOrbit = FALSE;
	us.fSector = FALSE;
	us.fInfluence = FALSE;
	us.fAstroGraph = FALSE;
	us.fCalendar = FALSE;
	us.fInDay = FALSE;
	us.fInDayInf = FALSE;
	us.fEphemeris = FALSE;
	us.fTransit = FALSE;
	us.fTransitInf = FALSE;
	us.fSphere = FALSE;

	////////////////////////////////////////////////////////////////////

	us.fFirdaria = FALSE;
	us.fPlanetaryHours = FALSE;
	us.fSquareChart = FALSE;
	us.fAlmtsPoint = FALSE;
	us.fAlmtsChart = FALSE;
	us.fPrimDirs = FALSE;
	us.fJyotish = FALSE;
	us.fPrimDirs2 = FALSE;
	us.fPrimDirs3 = FALSE;
	////////////////////////////////////////////////////////////////////

	/* Table chart types */
	us.fSwitch = FALSE;
	us.fSwitchRare = FALSE;
	us.fKeyGraph = FALSE;
	us.fSign = FALSE;
	us.fSolarEclipse = FALSE;
	us.fLunarEclipse = FALSE;
	us.fLunarPhases = FALSE;
	us.fObject = FALSE;
	us.fAspect = FALSE;
	us.fConstel = FALSE;
	us.fOrbitData = FALSE;
	us.fMeaning = FALSE;
	us.fPrintProgramStatus = FALSE;
	us.f12parts = FALSE;
	us.fAnti = FALSE;
	us.fAnti2 = FALSE;
	us.f12parts2 = FALSE;
	us.fNavamsas2 = FALSE;
	us.nArabic = FALSE;
	us.fZodRel = FALSE;
	us.fDecennials = FALSE;
}

#if 0
// for animate
extern CP cpPDs;
int calc2(BOOL terrestrial, BOOL* valid, int* y, int* m, int* d, int* ho, int* mi, int* se, double* t, BOOL* direct, double* da)
{

	if ((ciTwin.yea - ciNatal.yea) > 130 || (ciTwin.yea - ciNatal.yea) < 0)
	{
		not(wi.fAnimate);
		wi.fAnimate = FALSE;
		MessageBoxA(0, "Date is out of range", "Note", MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		return -1;
	}

	int year, month, day, hour, minute, second;
	double jut;

	int pdnum = CurrentRec;
	double da1 = pds[pdnum].arc;

	year = ciTwin.yea;
	month = ciTwin.mon;
	day = ciTwin.day;
	jut = DegMin2DecDeg(ciTwin.tim) + 0.5 / 3600;

	hour = (int)jut;
	minute = (int)fmod(jut * 60, 60);
	second = (int)fmod(jut * 3600, 60);

	*da = 0.0;

	double arc = 0.0;

	*direct = direction;
	Place place;
	jut = DegMin2DecDeg(ciNatal.tim) + 0.5 / 3600;
	int jhour = (int)jut;
	int jmin = (int)fmod(jut * 60, 60);
	int jsec = (int)fmod(jut * 3600, 60);

	BOOL calflag = (
		ciNatal.yea < 1582 ||
		(ciNatal.yea == 1582 && (ciNatal.mon < 10 || ciNatal.mon == 10 && ciNatal.day < 15)
			)) ? 0 : 1;

	timeP.time(ciNatal.yea, ciNatal.mon, ciNatal.day, jhour, jmin, jsec, 0, FALSE, ciNatal.zon, ciNatal.dst, place, FALSE);

	timeP.cal = calflag;

	chart.ChartInit("Roberto", TRUE, timeP, place, RADIX, "", TRUE, 1, FALSE);

	*y = year;
	*m = month;
	*d = day;
	*ho = hour;
	*mi = minute;
	*se = second;
	*t = double(*ho) + double(*mi) / 60.0 + double(*se) / 3600.0;

	double jd = swe_julday(*y, *m, *d, *t, calflag);

	if (chart.time.jd > jd)
	{
		// so, we have to restore the date to natal date
		memcpy(&ciMain, &ciNatal, sizeof(ciMain));
		return 0;
	}
	arc = calcArc(jd, *direct);

	ARC = arc;
	*da = arc;
	if (*direct == 1)
		*da *= -1;

	PDsInChart pdinch;
	pdinch.PDsInChartInit(chart, *da, calflag);// #self.yz, mz, dz, tz ==> chart;       da=29.804327657788399
	int pdh, pdm, pds;
	decToDeg(pdinch.tz, &pdh, &pdm, &pds);

	Time tim;
	tim.time(pdinch.yz, pdinch.mz, pdinch.dz, pdh, pdm, pds, GREENWICH, TRUE, 0, 0, chart.place, FALSE);
	is.rSid = 0;
	if (us.fSidereal)
	{
		swe_set_sid_mode(us.nSiderealMode, 0, 0);
		is.rSid = -swe_get_ayanamsa_ut(tim.jd) + us.rSiderealCorrection;// -24.893349387443948
	}

	Chart pdchartpls;

	if (!terrestrial)
	{
		if (options.pdincharttyp == 0)
		{
			pdchart.ChartInit(chart.name, chart.male, tim, chart.place, PDINCHART, "", FALSE, 1, FALSE);//#, proftype, nolat);
			pdchartpls.ChartInit(chart.name, chart.male, chart.time, chart.place, PDINCHART, "", FALSE, 1, FALSE);

			//#modify planets ...
			if (options.primarydir == PLACIDIANSEMIARC || options.primarydir == PLACIDIANUNDERTHEPOLE)
				pdchart.planets.calcMundaneProfPos(pdchart.houses.ascmc2, pdchartpls.planets.planets, chart.place.lat, chart.obl[0]);
			else
			{
				//			pdchart.houses.HousesInit(tim.jd, 0, pdchart.place.lat, pdchart.place.lon, 'R', pdchart.obl[0], options.ayanamsha, pdchart.ayanamsha);
				pdchart.planets.calcRegioPDsInChartsPos(pdchart.houses.ascmc2, pdchartpls.planets.planets, chart.place.lat, chart.obl[0]);
			}
			//#modify lof
			if (options.primarydir == PLACIDIANSEMIARC || options.primarydir == PLACIDIANUNDERTHEPOLE)
				pdchart.fortune.calcMundaneProfPos(pdchart.houses.ascmc2, pdchartpls.fortune, chart.place.lat, chart.obl[0]);
			else
				pdchart.fortune.calcRegioPDsInChartsPos(pdchart.houses.ascmc2, pdchartpls.fortune, chart.place.lat, chart.obl[0]);
		}
		else if (options.pdincharttyp == 1)
		{
			pdchart.ChartInit(chart.name, chart.male, tim, chart.place, PDINCHART, "", FALSE, 0, TRUE);

			pdchartpls.ChartInit(chart.name, chart.male, chart.time, chart.place, PDINCHART, "", FALSE, 0, TRUE);
			//#modify planets ...
			if (options.primarydir == PLACIDIANSEMIARC || options.primarydir == PLACIDIANUNDERTHEPOLE)
				pdchart.planets.calcMundaneProfPos(pdchart.houses.ascmc2, pdchartpls.planets.planets, chart.place.lat, chart.obl[0]);
			else
			{
				pdchart.planets.calcRegioPDsInChartsPos(pdchart.houses.ascmc2, pdchartpls.planets.planets, chart.place.lat, chart.obl[0]);
			}
			//#modify lof
			if (options.primarydir == PLACIDIANSEMIARC || options.primarydir == PLACIDIANUNDERTHEPOLE)
				pdchart.fortune.calcMundaneProfPos(pdchart.houses.ascmc2, pdchartpls.fortune, chart.place.lat, chart.obl[0]);
			else
				pdchart.fortune.calcRegioPDsInChartsPos(pdchart.houses.ascmc2, pdchartpls.fortune, chart.place.lat, chart.obl[0]);
		}
		else//:#Full Astronomical Procedure
		{
			pdchart.ChartInit(chart.name, chart.male, tim, chart.place, 7, "", FALSE, 1, FALSE);//#, proftype, nolat);
			pdchartpls.ChartInit(chart.name, chart.male, chart.time, chart.place, 7, "", FALSE, 1, FALSE);
			Planet* pdpls = pdchartpls.planets.planets;
			if (options.pdinchartsecmotion)
				pdpls = pdchart.planets.planets;
			double raequasc, declequasc, dist;
			swe_cotrans2(pdchart.houses.ascmc[EQUASC], 0.0, 1.0, -chart.obl[0], &raequasc, &declequasc, &dist);
			pdchart.planets.calcFullAstronomicalProc(*da, chart.obl[0], pdpls, pdchart.place.lat, pdchart.houses.ascmc2, raequasc);//#planets
			pdchart.fortune.calcFullAstronomicalProc(pdchartpls.fortune, *da, chart.obl[0]);
		}
	}
	else
	{
		pdchart.ChartInit(chart.name, chart.male, tim, chart.place, 7, "", FALSE, 1, FALSE);
		pdchart.fortune.recalcForMundaneChart(chart.fortune.fortune[0], chart.fortune.fortune[1], chart.fortune.fortune[2], chart.fortune.fortune[3], pdchart.houses.ascmc2, pdchart.raequasc, pdchart.obl[0], pdchart.place.lat);

		if (!options.pdinchartterrsecmotion)// Sec. Motion not set
		{
			double xmp;
			if (options.primarydir == 0 || options.primarydir == 1)
				xmp = pdchart.fortune.speculum[10];
			else if (options.primarydir == 2)
				xmp = pdchart.fortune.speculum2[11];
			else if (options.primarydir == 3)
				xmp = pdchart.fortune.speculum2[10];

			pdchart.ChartInit(chart.name, chart.male, chart.time, chart.place, 7, "", FALSE, 1, FALSE);//#, proftype, nolat)// 
			double raequasc, declequasc, dist;
			swe_cotrans2(pdchart.houses.ascmc[4], 0.0, 1.0, -chart.obl[0], &raequasc, &declequasc, &dist);
			pdchart.planets.calcMundaneWithoutSM(da1, chart.obl[0], pdchart.place.lat, pdchart.houses.ascmc2, raequasc);

			pdchart.fortune.recalcForMundaneChart(chart.fortune.fortune[0], chart.fortune.fortune[1], chart.fortune.fortune[2], chart.fortune.fortune[3], pdchart.houses.ascmc2, pdchart.raequasc, pdchart.obl[0], pdchart.place.lat);

			if (options.primarydir == 0 || options.primarydir == 1)
				pdchart.fortune.speculum[10] = xmp;
			else if (options.primarydir == 2)
				pdchart.fortune.speculum2[11] = xmp;
			else if (options.primarydir == 3)
				pdchart.fortune.speculum2[10] = xmp;
		}
	}

	////////////////////////////////////////////////////////////
	if (*direct == 1)
		*da *= -1;
	////////////////////////////////////////////////////////////
	//	pdchart.houses.ascmc2[0][0]+=is.rSid;
	//	pdchart.houses.ascmc2[1][0]+=is.rSid;

	int num = 12;
	int i;
	for (i = 0; i < num; i++)
	{
		pdchart.planets.planets[i].data[0] += is.rSid;
		pdchart.planets.planets[i].speculums[0][0] += is.rSid;
		pdchart.planets.planets[i].speculums[1][0] += is.rSid;
	}
	pdchart.fortune.fortune[0] += is.rSid;

	wcscpy(keytxt, Lang(tListDyn, options.pdkeyd));
	if (!options.pdkeydyn)
		wcscpy(keytxt, Lang(tListStat, options.pdkeys));

	wcscpy(txtdir, L"D");
	if (*direct == 1)
		wcscpy(txtdir, L"C");

	wcscpy(dirtxt, Lang(tListDirs, options.primarydir));

	wcscpy(keytypetxt, L"Dynamic Key");
	if (!options.pdkeydyn)
		wcscpy(keytypetxt, L"Static Key");

	//char typeListDyn[4][40] = {"TRUE Solar Equatorial Arc", "TRUE Solar Equatorial Arc(Birthday)", "TRUE Solar Ecliptical Arc", "TRUE Solar Ecliptical Arc(Birthday)"};
	wcscpy(keytxt, Lang(tListDyn, options.pdkeyd));

	if (!options.pdkeydyn)
		wcscpy(keytxt, Lang(tListStat, options.pdkeys));

	if (options.subprimarydir == 0/*mundane*/)
		wcscpy(MZtxt, L"Mundane");
	else
		wcscpy(MZtxt, L"Zodiacal");

	memset(&cpPDs, 0, sizeof(cpPDs));
	Chart chrt = pdchart;

	int speculum = 0;
	if (options.primarydir == 2 || options.primarydir == 3)
		speculum = 1;

	//#create Ingress-chart
	CI ciT;
	ciT = ciCore;
	ciCore = ciTwin;
	Place place1;
	ciCore = ciT;

	//tim.time(*y, *m, *d, *ho, *mi, *se, 1,  TRUE,   ciTwin.zon, ciTwin.dst, place1, FALSE);
	tim.time(*y, *m, *d, *ho, *mi, *se, 0, FALSE, ciTwin.zon, ciTwin.dst, place1, FALSE);
	ingchart.ChartInit(chart.name, chart.male, tim, place1, 7, "", FALSE, 1, FALSE);

	double mi2 = *mi, se2 = *se;
	memcpy(&ciThre, &ciCore, sizeof(ciCore));
	ciThre.yea = *y;
	ciThre.mon = *m;
	ciThre.day = *d;
	ciThre.tim = *ho + mi2 / 100.0 + se2 / 6000.0;

	for (i = 0; i < num; i++)
	{
		ingchart.planets.planets[i].speculums[0][0] += is.rSid;
		ingchart.planets.planets[i].speculums[1][0] += is.rSid;
		ingchart.planets.planets[i].data[0] += is.rSid;
	}
	ingchart.fortune.fortune[0] += is.rSid;

	CP cpT;
	ciCore = ciTwin;
	cpT = cp0;
	byte ignoreT[NUMBER_OBJECTS];
	memcpy(&ignoreT, &ignore1, NUMBER_OBJECTS);
	memcpy(&ignore1, &ignore2, NUMBER_OBJECTS);
	CastChart(TRUE);
	memcpy(&ignore1, &ignoreT, NUMBER_OBJECTS);
	cpIng = cp0;
	cp0 = cpT;
	ciCore = ciMain;

	if ((IsPDsChartWithoutTable /*&& UsePDsInChart*/) || IsPDsChartWithoutTable2 || us.nRel == rcProgTran || UsePDsInChart)
	{
		for (i = 1; i < 13; i++)
		{
			cpPDs.cusp_pos[i] = pdchart.houses.cusps[i];
			cpPDs.longitude[21 + i] = pdchart.houses.cusps[i];
		}

		// Copy Logitude and logitude vel
		for (i = 1; i < 11; i++)
		{
			cpPDs.longitude[i] = pdchart.planets.planets[i - 1].data[LON];
			cpPDs.latitude[i] = pdchart.planets.planets[i - 1].data[LAT];
			cpPDs.vel_longitude[i] = pdchart.planets.planets[i - 1].data[SPLON];
		}

		cpPDs.longitude[16] = pdchart.planets.planets[10].data[LON];//  NorthNode
		cpPDs.longitude[17] = pdchart.planets.planets[11].data[LON];//  SouthNode
		cpPDs.longitude[19] = pdchart.fortune.fortune[0];//  fortune
		cpPDs.longitude[22] = pdchart.houses.ascmc2[0][0];//  Asc
		cpPDs.longitude[31] = pdchart.houses.ascmc2[1][0];//  Mc

		cpPDs.latitude[16] = pdchart.planets.planets[10].data[LAT];//  NorthNode
		cpPDs.latitude[17] = pdchart.planets.planets[11].data[LAT];//  SouthNode
		cpPDs.latitude[19] = pdchart.fortune.fortune[LAT];//  fortune
		cpPDs.latitude[22] = pdchart.houses.ascmc2[0][1];
		cpPDs.latitude[31] = pdchart.houses.ascmc2[1][1];

		cpPDs.vel_longitude[16] = pdchart.planets.planets[10].data[SPLON];//  NorthNode
		cpPDs.vel_longitude[17] = pdchart.planets.planets[11].data[SPLON];//  SouthNode
		cpPDs.vel_longitude[19] = pdchart.fortune.fortune[SPLON];//  fortune 
	}
	else

	{
		// this is the PMP position:
		for (i = 0; i < 13; i++)
		{

			if (i < 10)
			{
				if (options.primarydir == 0 || options.primarydir == 1)
					cpPDs.longitude[i + 1] = pdchart.planets.planets[i].speculums[0][10];
				else if (options.primarydir == 2)
					cpPDs.longitude[i + 1] = pdchart.planets.planets[i].speculums[1][11];
				else if (options.primarydir == 3)
					cpPDs.longitude[i + 1] = pdchart.planets.planets[i].speculums[1][10];
			}
			else if (i == 10)
			{
				// North Node
				if (options.primarydir == 0 || options.primarydir == 1)
					cpPDs.longitude[16] = pdchart.planets.planets[i].speculums[0][10];
				else if (options.primarydir == 2)
					cpPDs.longitude[16] = pdchart.planets.planets[i].speculums[1][11];
				else if (options.primarydir == 3)
					cpPDs.longitude[16] = pdchart.planets.planets[i].speculums[1][10];

			}
			else if (i == 11)
			{
				// Sorth Node
				if (options.primarydir == 0 || options.primarydir == 1)
					cpPDs.longitude[17] = pdchart.planets.planets[i].speculums[0][10];
				else if (options.primarydir == 2)
					cpPDs.longitude[17] = pdchart.planets.planets[i].speculums[1][11];
				else if (options.primarydir == 3)
					cpPDs.longitude[17] = pdchart.planets.planets[i].speculums[1][10];

			}
			else if (i == 12)
			{
				// Part Fortune 
				if (options.primarydir == 0 || options.primarydir == 1)
					cpPDs.longitude[19] = pdchart.fortune.speculum[10];
				else if (options.primarydir == 2)
					cpPDs.longitude[19] = pdchart.fortune.speculum2[11];
				else if (options.primarydir == 3)
					cpPDs.longitude[19] = pdchart.fortune.speculum2[10];
			}
		}
	}
	return 0;
}
#endif

typedef struct PrimDir
{
	BOOL mundane;
	int prom;
	int prom2;
	int sig;
	int promasp;
	int sigasp;
	double arc;
	BOOL direct;
	int parallelaxis;
	double time;
	double age;
}
PrimDir;

PrimDir pds[30480];

// Compares two NULL-terminated strings, byte by byte, until the end of 
// the shorter string. Case is disregarded. Third parameter is the maximum
// number of characters to compare, if greater than zero, otherwise it 
// is disregarded.
// Returns true if both strings are the same, false otherwise. 
BOOL fSstringsMatch(const wchar_t* s1, const wchar_t* s2, int n)
{
	for (; *s1 && *s2; s1++, s2++)
	{
		if (towlower(*s1) != towlower(*s2))
		{
			return FALSE;
		}

		if (--n == 0)
		{
			break;
		}
	}
	return TRUE;
}


int NParseSzW(wchar_t* szEntry, int pm)
{
	wchar_t szLocal[cchSzMax], * sz;
	int cch, n, i;
	int returnflag = 0;

	/* First strip off any leading or trailing spaces. */
	for (cch = 0; szLocal[cch] = szEntry[cch]; cch++)
		;
	szLocal[cch] = chNull;
	while (cch && (byte)szLocal[cch - 1] <= ' ')
		szLocal[--cch] = chNull;
	for (sz = szLocal; *sz && (byte)(*sz) <= ' '; sz++, cch--)
		;
	// fix IC name to be "IC "
	if (sz[0] == L'I' && sz[1] == L'C')
	{
		sz[2] = L' ';
		sz[3] = 0;
		cch++;
	}
	if (cch >= 3) // ask for a minimum of 3 characters, but why? sometimes less is enough!
	{
		switch (pm)
		{
			/* Parse months, e.g. "February" or "Feb" -> 2 for February. */
		case pmMon:
			for (i = 1; i < NUMBER_OF_SIGNS; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar(tMonth[i]).c_str(), 0))
				{
					returnflag = i;
					break;
				}
			}
			break;

			/* Parse months in English (e.g. for data files), e.g. "February" or "Feb" -> 2 for February. */
		case pmMonEn:
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar(szMonthEnglish[i]).c_str(), 0))
				{
					returnflag = i;
					break;
				}
			}
			break;

			/* Parse planets, e.g. "Jupiter" or "Jup" -> 6 for Jupiter. */
		case pmObject:

			for (i = 0; i <= cObj; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar(tObjShortName[i]).c_str(), 3))
					// we can match maximum 3 characters because of cp0.longitude position files that have three chars for cp0.longitude names
				{
					returnflag = i;
					break;
				}
			}
			break;
		case pmObjectEn:

			for (i = 0; i <= cObj; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar((char*)szObjShortNameEnglish[i]).c_str(), 3))
					// we can match maximum 3 characters because of cp0.longitude position files that have three chars for cp0.longitude names
				{
					returnflag = i;
					break;
				}
			}
			break;

			/* Parse aspects, e.g. "Conjunct" or "Con" -> 1 for the Conjunction. */
		case pmAspect:
			for (i = 1; i <= cAspect; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar(tAspectAbbrev[i]).c_str(), 0))
				{
					returnflag = i;
					break;
				}
			}
			break;

			/* Parse house systems, e.g. "Koch" or "Koc" -> 1 for Koch houses. */
		case pmHouseSystem:
			for (i = 0; i < NUMBER_OF_HOUSE_SYSTEMS; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar(tHouseSystem[i]).c_str(), 0))
				{
					returnflag = i;
					break;
				}
			}
			break;

			/* Parse zodiac signs, e.g. "Scorpio" or "Sco" -> 8 for Scorpio. */
		case pmSign:
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar(tSignName[i]).c_str(), 0))
				{
					returnflag = i;
					break;
				}
			}
			break;

		case pmSignEn:
			for (i = 1; i <= NUMBER_OF_SIGNS; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar((char*)szSignAbbrevEnglish[i]).c_str(), 0))
				{
					returnflag = i;
					break;
				}
			}
			break;
			/* Parse colors, e.g. "White" or "Whi" -> 15 for White. */
		case pmColor:
			for (i = 0; i < cColor; i++)
			{
				if (fSstringsMatch(sz, char_to_wchar(tColor[i]).c_str(), 0))
				{
					returnflag = i;
					break;
				}
			}
			break;
		}

		if(returnflag) return returnflag;
	}

	n = std::stoi(sz);

	if (pm == pmYea)
	{
		/* For years, process any "BC" (or "B.C.", "b.c", and variations) and   */
		/* convert an example such as "5BC" to -4. For negative years, note the */
		/* difference of one, as 1AD was preceeded by 1BC, with no year zero.   */
		i = Max(cch - 1, 0);
		if (i && sz[i] == '.')
			i--;
		if (i && ChCap(sz[i]) == 'C')
			i--;
		if (i && sz[i] == '.')
			i--;
		if (i && ChCap(sz[i]) == 'B')
			n = 1 - n;
	}
	return n;
}


/* Given a string, return a floating point number corresponding to what the  */
/* string indicates, based on a given parsing mode, like above for integers. */
double RParseSz(const char* szEntry, int pm)
{
	char szLocal[cchSzMax], * sz, * pch, ch, chdot, minutes[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	int cch, havedot, dot1, dot2, newstyle = FALSE, i, j, f = FALSE;
	double r, rMinutes, rSeconds;

	/* First strip off any leading or trailing spaces. */
	for (cch = 0; szLocal[cch] = szEntry[cch]; cch++)
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
					f = TRUE;
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

	newstyle = TRUE;
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
			newstyle = FALSE;

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

void GetChartResult(CI& ciInput,bool useInput = true)
{
	//默认参数设置
	CI ciDefault = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
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
	CI ci = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
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

	wi.nDlgChart = 1;
	switch (wi.nDlgChart)
	{
	case 1:
		ciMain = ciCore = ci;
		IsDoubleReturn = FALSE;
		memcpy(&ciNatal, &ci, sizeof(CI));
		break;
	case 2:
		ciTwin = ci;
		ciTran = ci;
		IsDoubleReturn = FALSE;
		memcpy(&ciNatal2, &ci, sizeof(CI));

		if (us.fPrimDirs)
		{
			//BOOL valid = TRUE;
			//int y, m, d, ho, mi, se;
			//double t, da;
			//BOOL direct;
			////calc2(pds[CurrentRec].mundane, &valid, &y, &m, &d, &ho, &mi, &se, &t, &direct, &da);
//
			//pds[CurrentRec].arc = da;
//
			//BOOL cal_type = (
			//	ciTwin.yea < 1582 ||
			//	(ciTwin.yea == 1582 && (ciTwin.mon < 10 || ciTwin.mon == 10 && ciTwin.day < 15)
			//		)) ? 0 : 1;
//
			//double jd = swe_julday(y, m, d, t, cal_type);
			//pds[CurrentRec].time = jd;
		}
		break;
	case 3:
		ciThre = ci;
		//ciTran = ciThre;
		break;
	default:
		ciFour = ci;
		break;
	}

	wi.fCast = TRUE;
	us.fEuroDate = 1;


	useswe.setMYD(ciCore.mon, ciCore.day, ciCore.yea);
	useswe.setMdyToJulian();
	double res = CastChart(1);
	//wprintf(L"%lf\n", res);
	ChartListing();

	us.fAspList = TRUE;
	FCreateGrid(FALSE);

	us.fAspSummary = 1;
	ChartAspect();
	//ListAspect();
}

int main(int argc,char* argv[])
{
	////设置输入输出编码为 UTF-8
	//SetConsoleOutputCP(65001);
	//SetConsoleCP(65001);
	////设置控制台为宽字符输出模式
	//int seno = _setmode(_fileno(stdout), _O_U16TEXT);  // 关键步骤！
	wchar_t szWindowName[1024];

	// 程序中使用的相位数量
	us.nAsp = cAspect;
	for (int i=0;i<=18;i++)
	{
		if( ignoreA[i] )
			us.nAsp--;
	}

	swprintf(szWindowName,sizeof(szWindowName)/sizeof(wchar_t),
	 L"%ls version %ls for Current System with Ephemeris: JPL DE406 / Swiss V%ls\n", 
	 char_to_wchar(szAppNameCore).c_str(), char_to_wchar(szVersionCore).c_str(), char_to_wchar(useswe.getSweVersion()).c_str());
	wprintf(szWindowName);

	int i;
	memcpy(&ignoreSO,&ignore1,NUMBER_OBJECTS);
	for (i = starLo; i <= starHi; i++)
		ignore1[i]=1;
	options.OptionsInit();
	options.NoNutation = FALSE;
	options.NoPrecession = FALSE;
	options.AddNutation = FALSE;
	options.topocentric = FALSE;
	options.zodprof = FALSE;
	eepp = 1;
	CI ciInput = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
	ciInput.mon = atoi(argv[1]);
	ciInput.day = atoi(argv[2]);
	ciInput.yea = atoi(argv[3]);
	ciInput.tim = RParseSz(argv[4], pmTim);
	ciInput.dst = RParseSz(argv[5], pmDst);
	ciInput.zon = RParseSz(argv[6], pmZon);
	ciInput.lon = RParseSz(argv[7], pmLon);
	ciInput.lat = RParseSz(argv[8], pmLat);
	ciInput.alt = atof(argv[9]);
	swprintf(ciInput.nam, sizeof(ciInput.nam) / sizeof(wchar_t), L"%ls", char_to_wchar(argv[10]).c_str());
	swprintf(ciInput.loc, sizeof(ciInput.loc) / sizeof(wchar_t), L"%ls", char_to_wchar(argv[11]).c_str());
	//swprintf(ciInput.nam, sizeof(ciInput.nam) / sizeof(wchar_t), L"%s", argv[9]);
	//swprintf(ciInput.loc, sizeof(ciInput.loc) / sizeof(wchar_t), L"%s", argv[10]);
	//GetChartResult(ciInput,false);  调试用 
	GetChartResult(ciInput);
	//神奇的代码，把下面这行加入调试到这里会报错哦
	//printf("直接输入：° | Unicode转义：\u00B0\n");

	return 0;
}
