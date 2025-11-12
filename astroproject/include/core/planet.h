#ifndef PLANET_H
#define PLANET_H

#include "../utils/utils.h"
#define NUMBER_OBJECTS 118
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
	oSoNode,					// 17
	oLil,						// 18
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
	cObj = starHi,				// 117
} objects_t;

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
// obj name global variables not modified by the program
extern const char* tObjName[];
extern const char* tObjShortName[];
// cPlanet global variables not modified by the program
extern byte ignore1[];
extern byte ignore2[];
extern byte ignore3[];
extern double rAspInf[];
extern double rObjInf[];
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

#define cUran (uranHi - uranLo + 1)	// 9
#define SEFLG_EPHMASK   (SEFLG_JPLEPH|SEFLG_SWIEPH|SEFLG_MOSEPH)

// 函数声明
double ReadThree(double r0, double r1, double r2);
bool CalculatePlanetSE(int ind, double jde, bool heliocentric, double SE_coordinates[6]);
void ProcessPlanet(int ind, double aber);
void SphToRec(double r, double azi, double alt, double* rx, double* ry, double* rz);
void ComputePlanets();
void PolToRec(double A, double R, double* X, double* Y);
int RulerPlanet(int i);
void PlanetPPower(bool chs);

#endif // PLANET_H