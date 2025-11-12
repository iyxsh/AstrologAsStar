#ifndef HOUSES_H
#define HOUSES_H

#include "../utils/utils.h"
extern const char* szHouseSystem[];
/* House system types */
typedef enum
{
	cHousePlacidus,
	cHouseKoch,
	cHouseEqual,
	cHouseCampanus,
	cHouseMeridian,				/* axial rotation system */
	cHouseRegiomontanus,
	cHousePorphyry,
	cHouseMorinus,
	cHouseTopocentric,			/* Polich/Page */
	cHouseAlcabitius,
	cHouseEqualMidheaven,
	cHouseNeoPorphyry,
	cHouseWhole,
	cHouseVedic,				/* Vehlow */
	cHouseNull,					/* each sign is a house, starting from Aries */
	cHouseShripati,				/* Sripati, Shripati, Bhava Chakra, Hindu Bhava, Hindu Unequal */
	NUMBER_OF_HOUSE_SYSTEMS,	/* 16 */
} house_system_t;

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

enum _housemodel {
  hmEcliptic = 0,   // Standard 2D: Equator of houses is ecliptic
  hmPrime    = 1,   // 3D: Equator of houses is prime vertical
  hmHorizon  = 2,   // 3D: Equator of houses is local horizon
  hmEquator  = 3,   // 3D: Equator of houses is celestial equator
  cMethod = 3,
};

#define NUMBER_OF_HOUSES	12

// 添加rulerships枚举
typedef enum
{
	rrStd = 0,  // Standard exoteric
	rrEso = 1,  // Esoteric
	rrHie = 2,  // Hierarchical
	rrExa = 3,  // Exaltation
	rrRay = 4,  // Ray rulership
	rrMax = 5
} rulerships_t;

/* Chart modes */
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

void ComputeInHouses(void);
void SwissHouse(double jd, double lon, double lat, int housesystem, double* asc, double* mc, double* ra, double* vtx, double* ep, double* ob, double* off);

#endif // HOUSES_H