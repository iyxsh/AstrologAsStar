#include "../../include/core/ephemeris.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/utils/utils.h"
#include "../../swe/swisseph/swephexp.h"
#include "../../include/core/planet.h"
#include <math.h>
extern US us;
extern CP cp0;
extern CI ciCore;
extern byte ignore1[];
extern double spacex[];
extern double spacey[];
extern double spacez[];
extern byte oscLilith;
extern int cSign;

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
	char path[2 * MAX_FILE_NAME + 8];	// 两个目录 + 分隔符，避免拼接截断

	// Build an ephemeris path in the same way as FileOpen() in io.c 
	// searchs for a file. First look in the current directory, then 
	// in the executable directory, and finally in the ephemeris directory.
	snprintf(path, sizeof(path), "." ";" "%s" ";" "%s",
		dirs.executable.dir, dirs.ephemeris.dir);

	// now set the directory
	swe_set_ephe_path(path);
}

void ComputeWithSwissEphemeris(double t)
{
	int i;
	double SE_coordinates[6];
	double longitude, latitude, distance, speed_longitude, speed_latitude;
	bool	use_SE_heliocentric_mode;
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
			cp0.vel_distance[i] = SE_coordinates[5];

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