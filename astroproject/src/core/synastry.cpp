#include "../../include/core/synastry.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/core/planet.h"
#include "../../include/core/houses.h"
#include "../../include/utils/utils.h"
#include <math.h>
extern CI ciTran;
extern IS  is;
extern double Longit;
extern double Latit;
void CastSynastry(bool fDate)
{
	double Off = 0.0, j;
	double ep1 = 0.0;
	int i;
	is.rSid = 0.0;
	Longit = DegMin2DecDeg(ciTran.lon);
	Latit = DegMin2DecDeg(ciTran.lat);

	//is.JD = MdytszToJulianDay(ciTran.mon, ciTran.day, ciTran.yea, ciTran.tim, ciTran.dst, ciTran.zon);
	//j = JulianDayToTime(is.JD);

	is.T = is.JD - 2415020.0;
	is.T = (is.T - 0.5) / 365.25;

	ComputePlanets();
	ComputeInHouses();
}