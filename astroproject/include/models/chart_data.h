#ifndef CHART_DATA_H
#define CHART_DATA_H

#include "../core/planet.h"
#include "../core/houses.h"

// Grid information structure
typedef struct GridInfo
{
	int n[118][118];  // NUMBER_OBJECTS = 118
	short v[118][118];
} GridInfo;

// Chart information structure
typedef struct ChartInfo
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
	bool daybirth;
	wchar_t desc[10240];
	wchar_t file[260];
	wchar_t Events[4096];
	double Sun;
	double Moon;
	double ASC;
	char ASP[512];
	char house[8000];
} CI;

typedef struct _PT3R {
	double x;
	double y;
	double z;
} PT3R;

typedef struct ChartPositions
{
	double longitude[NUMBER_OBJECTS];			/* The zodiac positions.    */
	double latitude[NUMBER_OBJECTS];			/* Ecliptic declination.    */
	double vel_longitude[NUMBER_OBJECTS];			/* Retrogradation velocity. */
	double vel_latitude[NUMBER_OBJECTS];		/* Speed in declination  */
	double cusp_pos[NUMBER_OF_HOUSES + 1];	/* House cusp positions.    */
	double cusp3[NUMBER_OF_HOUSES + 1];  // 3D house cusp positions.
	char house_no[NUMBER_OBJECTS];			/* House each object is in. */

	double vel_distance[NUMBER_OBJECTS];  // Distance velocity
	PT3R pt[NUMBER_OBJECTS];      // X,Y,Z coordintes in space
	double dist[NUMBER_OBJECTS];    // Distance to X,Y,Z coordinates

	double deltaT;
} CP;

#endif // CHART_DATA_H