#include "../../include/core/houses.h"
#include "../../include/core/planet.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/utils/utils.h"
#include "../../swe/swisseph/swephexp.h"
#include "../../swe/swe_sidtime_args.h"
#include <math.h>

extern US us;
extern CP cp0;
extern int hRevers;
extern byte PolarMCflip;
extern CI ciMain;
extern IS is;
extern int cSign;

/* P1.2 A2 fix — Swiss Ephemeris has no mapping for hsEqualMC(10)/hsWhole(12)/
 * hsNull(14); they fall through to 'A' inside SwissHouse(), and the original
 * engine then recomputes them with dedicated algorithms (original
 * ComputeHouses dispatch). Ported verbatim from E:\data\astrolog_golden
 * astrolog.cpp 71280/71422/71634. */
void HouseEqualMidheaven(void);
void HouseWhole(void);
void HouseNull(void);
void ComputeHouses(int housesystem);

const char* szHouseSystem[NUMBER_OF_HOUSE_SYSTEMS] = {
	"Placidus", "Koch", "Equal(Asc)", "Campanus", "Meridian",
	"Regiomontanus", "Porphyry", "Morinus", "Topocentric", "Alcabitius",
	"Equal(MC)", "Neo-Porphyry", "Whole", "Vedic", "Null", "Shripati"
};

double RHousePlaceIn3DCore(double rLon, double rLat)
{
  double lon, lat;

  us.nHouse3D = hmPrime;
  lon = Tropical(rLon); lat = rLat;
  CoorXform2(&lon, &lat,Rad2Deg(is.rObliquity));
  lon = Mod(is.lonMC - lon + rDegQuad);
  if (us.nHouse3D == hmPrime)
    CoorXform2(&lon, &lat, -ciMain.lat);
  else if (us.nHouse3D == hmHorizon)
    CoorXform2(&lon, &lat, rDegQuad - ciMain.lat);
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
	for (i = 0; i <= cObj; i++)
		cp0.house_no[i] = NHousePlaceIn(cp0.longitude[i], cp0.latitude[i]);
}

static int32 iflag = 0;              /* external flag: helio, geo... */

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

	if (us.fSidereal)
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

	/* Have Astrolog compute the houses if Swiss Ephemeris didn't do so.
	 * (P1.2 A2 fix: this dispatch was commented out in the refactor, which
	 * silently degraded hsEqualMC/hsWhole/hsNull to the swe 'A' whole-sign
	 * placeholder. Restored to match the original engine.) */
	if (ch == 'A')
		ComputeHouses(housesystem);
}

/* Equal (MC) houses: 10th cusp on the MC, all houses equal 30-degree wedges
 * in zodiac order; house 1 cusp is forced to the Ascendant by CastChart()
 * via the oAsc longitude override. Port of original astrolog.cpp:71280. */
void HouseEqualMidheaven()
{
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		if (PolarMCflip)
		{
			hRevers = 2;
			is.MC = Mod(is.MC - 180.0);
		}
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
	{
		if (hRevers && PolarMCflip)
			cp0.cusp_pos[i] = Mod(is.MC - 90.0 - 30.0 * (double)(i - 1));
		else
			cp0.cusp_pos[i] = Mod(is.MC - 270.0 + 30.0 * (double)(i - 1));
	}
}

/* Whole-sign houses: house 1 = the zodiac sign containing the Ascendant,
 * each subsequent house the next whole sign (cusps at 0 degrees). Port of
 * original astrolog.cpp:71634. */
void HouseWhole()
{
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		if (PolarMCflip)
			is.MC = Mod(is.MC - 180.0);
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
		cp0.cusp_pos[i] = Mod((double)((Z2Sign(is.Asc) - 1) * 30) + Sign2Z(i));
}

/* Null house system: no real houses; each cusp simply at the start of the
 * successive zodiac sign (cusp i = (i-1)*30 degrees). Port of original
 * astrolog.cpp:71422. */
void HouseNull()
{
	int i;

	if (hRevers)
	{
		is.Asc = Mod(is.Asc - 180.0);
		is.MC = Mod(is.MC - 180.0);
	}

	for (i = 1; i <= NUMBER_OF_HOUSES; i++)
		cp0.cusp_pos[i] = Mod(Sign2Z(i));
}

/* Astrolog-side house recomputation for systems Swiss Ephemeris cannot do.
 * Only hsEqualMidheaven/hsWhole/hsNull reach here (they are the only ones
 * mapped to the swe 'A' placeholder in SwissHouse); the extreme-latitude and
 * Vedic Asc-flip guards of the original dispatch are unreachable on this
 * path and omitted. */
void ComputeHouses(int housesystem)
{
	switch (housesystem)
	{
	case hsEqualMC:
		HouseEqualMidheaven();
		break;
	case hsWhole:
		HouseWhole();
		break;
	case hsNull:
		HouseNull();
		break;
	default:
		break; /* unreachable — other systems have explicit swe mappings */
	}
}