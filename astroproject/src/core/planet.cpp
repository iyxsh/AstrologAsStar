#include "../../include/core/planet.h"
#include "../../include/core/aspects.h"
#include "../../include/core/houses.h"
#include "../../include/core/chart.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/utils/utils.h"
#include "../../swe/swisseph/swephexp.h"
#include "../../include/data/signs.h"

#include <math.h>

extern int eepp;
extern double spacex[];
extern double spacey[];
extern double spacez[];
extern byte oscLilith;
extern int cSign;
extern CP cp0;
extern CI ciCore,ciMain;
extern IS is;
extern US us;

const char* tObjName[] = {"Earth", "Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", "Chiron", "Ceres", "Pallas Athena", "Juno", "Vesta", "North Node", "South Node", "Lilith", "Part Fortune", "Vertex", "East Point", "Ascendant", "2nd Cusp", "3rd Cusp", "IC ", "5th Cusp", "6th Cusp", "Descendant", "8th Cusp", "9th Cusp", "Midheaven", "11th Cusp", "12th Cusp", "Cupido", "Hades", "Zeus", "Kronos", "Apollon", "Admetos", "Vulkanus", "Poseidon", "Proserpina", "Achernar", "Polaris", "Zeta Retic.", "Alcyone", "Aldebaran", "Capella", "Rigel", "Bellatrix", "Elnath", "Alnilam", "Betelgeuse", "Menkalinan", "Mirzam", "Canopus", "Alhena", "Sirius", "Adhara", "Wezen", "Castor", "Procyon", "Pollux", "Suhail", "Avior", "Miaplacidus", "Alphard", "Regulus", "Dubhe", "Acrux", "Gacrux", "Mimosa", "Alioth", "Spica", "Alkaid", "Agena", "Arcturus", "Toliman", "Antares", "Shaula", "Sargas", "Kaus Austr.", "Vega", "Altair", "Peacock", "Deneb", "Alnair", "Fomalhaut", "Andromeda", "Alpheratz", "Algenib", "Schedar", "Mirach", "Alrischa", "Almac", "Algol", "Mintaka", "Wasat", "Acubens", "Merak", "Vindemiatrix", "Mizar", "Kochab", "Zuben Elgen.", "Zuben Escha.", "Alphecca", "Unuk Alhai", "Ras Alhague", "Albireo", "Alderamin", "Nashira", "Skat", "Scheat", "Markab", "Gal.Center", "Apex", "Sadalmelik" };
const char* tObjShortName[]= {"Earth","Sun","Moon","Mercury","Venus","Mars","Jupiter","Saturn","Uranus","Neptune","Pluto","Chiron","Ceres","Pallas","Juno","Vesta","NoNode","SoNode","Lilith","Fortune","Vertex","EaPoint","Ascendant","2nd Cusp","3rd Cusp","IC ","5th Cusp","6th Cusp","Descendant","8th Cusp","9th Cusp","Midheaven","11th Cusp","12th Cusp","Cupido","Hades","Zeus","Kronos","Apollon","Admetos","Vulkanus","Poseidon","Proserpina","Achernar","Polaris","Zeta Retic.","Alcyone","Aldebaran","Capella","Rigel","Bellatrix","Elnath","Alnilam","Betelgeuse","Menkalinan","Mirzam","Canopus","Alhena","Sirius","Adhara","Wezen","Castor","Procyon","Pollux","Suhail","Avior","Miaplacidus","Alphard","Regulus","Dubhe","Acrux","Gacrux","Mimosa","Alioth","Spica","Alkaid","Agena","Arcturus","Toliman","Antares","Shaula","Sargas","Kaus Austr.","Vega","Altair","Peacock","Deneb","Alnair","Fomalhaut","Andromeda","Alpheratz","Algenib","Schedar","Mirach","Alrischa","Almac","Algol","Mintaka","Wasat","Acubens","Merak","Vindemiatrix","Mizar","Kochab","Zuben Elgen.","Zuben Escha.","Alphecca","Unuk Alhai","Ras Alhague","Albireo","Alderamin","Nashira","Skat","Scheat","Markab","4","7","10"};
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

double ppower1[cLastMoving + 1] = { 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define RObjInf(i) rObjInf[Min(i, cLastMoving)]
#define RObjInfA(i) rObjInfA[Min(i, cLastMoving)]
double rObjInfA[90] = {
30,30,25,10,10,10,10,10,8,8,8,6,5,5,5,5,5,5,4,4,4,4,
20,10,10,10,10,10,10,10,10,15,10,10,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,20,10,10,
10,10
};

/* The inherent strength of each aspect - */
double rAspInf[cAspect + 1] = { 0.0,1.0, 0.8, 0.8, 0.6, 0.6, 0.4, 0.4, 0.2, 0.2,0.2, 0.2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
double rAspInfA[25] = { 0.0,
1.0,0.8,0.8,0.6,0.6,0.4,0.4,0.2,0.2,
0.2,0.2,0.1,0.1,0.1,0.1,0.1,0.1,0.1,
0.05,0.05,0.05,0.05,0.05,0.05 };

/* This array is the reverse of the ruler arrays:   */
/* Here, given a sign, return what planet rules it. */
int rulesA[12 + 1] = { -1,oMar, oVen, oMer, oMoo, oSun, oMer, oVen, oPlu, oJup, oSat, oUra, oNep };
int rules2A[12 + 1] = { -1,-1, -1, -1, -1, -1, -1, -1, oMar, -1, -1, oSat, oJup };
int rgSignEso1[12+1] = {-1,oMer, oVul, oVen, oNep, oSun, oMoo, oUra, oMar, oEar, oSat, oJup, oPlu};
int rgSignEso2[12+1] = {-1,-1, -1, -1, -1, oNep, -1, -1, -1, -1, -1, -1, -1};
int rgSignHie1[12+1] = {-1,oUra, oVul, oEar, oNep, oSun, oJup, oSat, oMer, oMar, oVen, oMoo, oPlu};
int rgSignHie2[12+1] = {-1,-1, -1, -1, -1, oUra, -1, -1, -1, -1, -1, oVul, -1};
int rgSignRay[12+1] = {0,17, 4, 2, 37, 15, 26, 3, 4, 456, 137, 5, 26};  
int rules2[12+1] = {-1,-1, -1, -1, -1, -1, -1, -1, oMar, -1, -1, oSat, oJup};

double ReadThree(double r0, double r1, double r2)
{
	return Deg2Rad(r0 + r1 * is.T + r2 * is.T * is.T);
}
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

bool CalculatePlanetSE(int ind, double jde, bool heliocentric, double SE_coordinates[6])
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
		swe_set_sid_mode(us.nSiderealMode, 0, 0);
	}
	flag |= SEFLG_SPEED;

	iobj = astrolog_object_2_SE_object(ind);

	if (iobj >= 0)
	{
		flag |= heliocentric ? SEFLG_SPEED | SEFLG_HELCTR : SEFLG_SPEED;

		if (swe_calc(jde, iobj, flag, SE_coordinates, serr) >= 0)
		{
			return true;	// success
		}
		else if (eepp == 1)
		{
			flag = SEFLG_MOSEPH;
			flag |= heliocentric ? SEFLG_SPEED | SEFLG_HELCTR : SEFLG_SPEED;
			if (swe_calc(jde, iobj, flag, SE_coordinates, serr) >= 0)
			{
				return true;
			}
		}
	}
	return false; // object not supported or other kinds of errors
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

/* Transform spherical to rectangular coordinates in x, y, z. */
void SphToRec(double r, double azi, double alt, double* rx, double* ry, double* rz)
{
	double rT;

	*rz = r * RSinD(alt);
	rT = r * RCosD(alt);
	*rx = rT * RCosD(azi);
	*ry = rT * RSinD(azi);
}

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

/* Convert polar to rectangular coordinates. */
void PolToRec(double A, double R, double* X, double* Y) {
	if (A == 0.0)
		A = rSmall;
	*X = R * cos(A);
	*Y = R * sin(A);
}

/*
******************************************************************************
** Planetary Position Calculations.
******************************************************************************
*/

/* Given three values, return them combined as the coefficients of a */
/* quadratic equation as a function of the chart time.               */

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

char rules[NUMBER_OF_SIGNS + 1] = { 0, 5, 4, 3, 2, 1, 3, 4, 5,  6, 7, 7, 6 };
/* The inherent strength of each house - */
double rHouseInf[NUMBER_OF_SIGNS + 3] = { 0,20, 0, 0, 10, 0, 0, 5, 0, 0, 15, 0, 0,15, 5};
double rHouseInfA[12 + 6] = {0,20, 0, 0, 10, 0, 0, 5, 0, 0, 15, 0, 0,15, 5, 5, 5, 5 };
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
void PlanetPPower(bool chs) //  for ppower1[]
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
	us.nStar = false;
	CastChart(true);

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
			j = 0;
			for (i = 0; i <= cLastMoving; i++)
				cLev[i] = false;
			for (i = 0; i <= cLastMoving; i++)
				if (dLev[i] && RulerPlanet(i))
					cLev[oDis[i]] = true;
			for (i = 0; i <= cLastMoving; i++)	/* A cp0.longitude isn't a final dispositor */
			{
				if (dLev[i] && !cLev[i] && RulerPlanet(i))
				{				/* if nobody is pointing to it.      */
					dLev[i] = 0;
					j = 1;
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
			j = 0;
			for (i = 0; i <= cLastMoving; i++)
			{
				if (RulerPlanet(i))
				{
					if (!dLev[i])
					{
						if (!dLev[oDis[i]])
							j = 1;
						else	/* If my dispositor already has */
							dLev[i] = dLev[oDis[i]] + 1;	/* a level, mine is one more.   */
					}
				}
			}
			if(j == 1) j = 0;
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

			c = Dignify(i, j,chs);
			if (!chs)
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

			c = Dignify(i, cp0.house_no[i],chs);
			if (!chs)
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

			c = Dignify(i, Z2Sign(cp0.cusp_pos[cp0.house_no[i]]),chs);
			if (!chs)
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
					ppower1[(int)rules[j]] += rObjInf[i] / (5.0 * sqrt((double)dLevS[rules[j]]));
				else
					ppower1[(int)rules[j]] += rObjInf[i] / 5.0;
			}

			/* House cusp dispositor has influence, if cp0.longitude is in this house. */
			/* Addition depends of ruling level (see graphics influence chart). */
			/* Influence has both natural and situative rulers of house.        */

			if (i != (j = (int)rules[cp0.house_no[i]]))
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
					ppower1[(int)rules[l]] += Hy * rObjInf[i] / (10.0 * sqrt((double)dLevHd[rules[l]]));
				else
					ppower1[(int)rules[l]] += Hy * rObjInf[i] / 10.0;
			}
		}
	}

	for (i = 1; i <= NUMBER_OF_SIGNS; i++)
	{
		/* Various planets get influence */
		j = Z2Sign(cp0.cusp_pos[i]);	/* if house cusps fall in signs  */
		ppower1[(int)rules[j]] += rHouseInf[i] / 2.0;	/* they rule.                    */
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
void ComputeInfluence() // for power1,power2
{
  int i, j, k, l;
  double x;
  char *c;
  double power1[118];
  double power2[118];

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

  if (!FCreateGridA(false))
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


