#include "../../include/core/fixed_stars.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/core/ephemeris.h"
#include "../../swe/swisseph/swephexp.h"
#include "../../include/utils/utils.h"
#include "../../include/core/planet.h"
#include "../../include/utils/TransU.h"
#include <math.h>

extern US us;
extern CP cp0;
extern IS is;
extern int hRevers;
extern byte PolarMCflip;
extern int starname[];
extern double Longit;
extern double Latit;

// 恒星名称数组
const char* st[75] = {
"Algenib", "Alpheratz", "Algol", "Miras", "Betelgeuse", "Rigel", 
"Capella", "Bellatrix", "Alnilam", "Alnitak", "Saiph", "Castor", 
"Pollux", "Alhena", "Sirius", "Canopus", "Castor", "Procyon", 
"Alphard", "Regulus", "Graffias", "Dschubba", "Antares", "Arcturus", 
"Alkaid", "Cor Caroli", "Vindemiatrix", "Spica", "Algorab", "Rasalhague", 
"Scheat", "Algedi", "Deneb", "Alderamin", "Alnair", "Fomalhaut", 
"Peacock", "Markab", "Gal. Center", "Apex", "Sadalmelik"
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

/* This is used by the chart calculation routine to calculate the positions */
/* of the fixed stars. Since the stars don't move in the sky over time, 	*/
/* getting their positions is mostly just reading info from an array and	*/
/* converting it to the correct reference frame. However, we have to add	*/
/* in the correct precession for the tropical zodiac, and sort the final	*/
/* index list based on what order the stars are supposed to be printed in.	*/
bool FileFind(const char* szFile, char* szDir, char* path_found)
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
				return false; // no luck finding file
			}
		}
	}

	fclose(file);
	if (path_found)
	{
		strcpy(path_found, file_name);
	}

	return true;
}

// 5.41G used SE of 02/12/1998
bool CalculateStarSE(int ind, double jd, double* longitude, double* latitude)
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
		rStarBright[ind] = SE_coordinates[2];
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

		return true;
	}

	if (us.fSeconds)
	{
		//		ErrorEphem(SE_STARFILE, ind);  
	}
	return false;
}

int NCompareSzW(const wchar_t* s1, const wchar_t* s2)
{
	while (*s1 && *s1 == *s2)
		s1++, s2++;
	return *s1 - *s2;
}

int PrimeRest = 0;
byte fEquator = 0;
double ObjPrime[cLastMoving + 1];
double StarPrime[cStar + 1];
void ComputeStars(double SD)
{
	int i, j;
	double x, y, z;
	double lon, lat, lonMC, latMC, lonz, latz, azi, alt;

	bool HaveStarFile;
	double t_plac, longitude, latitude;

	HaveStarFile = true;
	t_plac = JulianDayFromTime(is.T);

	if (!FileFind(sefstarsName, dirs.main.dir, NULL))
	{
		HaveStarFile = false;
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
			CoorXform(&lonMC, &latMC,is.rObliquity);
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
		CoorXform(&cp0.longitude[cLastMoving + i], &cp0.latitude[cLastMoving + i],-is.rObliquity);
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
				CoorXform(&lonz, &latz,is.rObliquity);
			}
			lonz = Deg2Rad(Mod(Rad2Deg(lonMC - lonz + lon)));
			lonz = Deg2Rad(Mod(Rad2Deg(lonz - lon + rPiHalf)));
			if (!fEquator)
				CoorXform(&lonz, &latz, rPiHalf - lat);
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
				CoorXform(&lonz, &latz,is.rObliquity);
			}
			lonz = Deg2Rad(Mod(Rad2Deg(lonMC - lonz + lon)));
			lonz = Deg2Rad(Mod(Rad2Deg(lonz - lon + rPiHalf)));
			if (!fEquator)
				CoorXform(&lonz, &latz, rPiHalf - lat);
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