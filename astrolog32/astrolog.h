// astrolog32.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>

// TODO: 在此处引用程序需要的其他标头。
#define ChDst(dst)    (dst == 0.0 ? 'S' : (dst == 1.0 ? 'D' : 'A'))
#define ChDstW(dst)    (dst == 0.0 ? L'S' : (dst == 1.0 ? L'D' : L'A'))
typedef int BOOL;
#define FALSE 0
#define TRUE 1

#if 0
class Syzygy
{
public:

	//	int RA;

		//#for topical almutens
	int TOPICALDEFAULT;
	int TOPICALCONIUNCTIO;
	int TOPICALOPPOSITIO;
	int TOPICALOPPOSITIORADIX;
	int TOPICALMOON;
	Time time;
	Time time2;
	double lon;
	double lon2;
	int flags;
	double lons[20];
	BOOL newmoon;
	BOOL ready;
	BOOL ready2;
	double speculum[4];
	double speculum2[4];
	Houses hses;
	Houses hses2;
	Planets planets;

	Syzygy()  // 古典占星中，对于出生前的新月点称呼“syzygy”     最接近生日的出生前的新月或满月
	{
		//#for topical almutens
		TOPICALDEFAULT = 0;
		TOPICALCONIUNCTIO = 1;
		TOPICALOPPOSITIO = 2;
		TOPICALOPPOSITIORADIX = 3;
		TOPICALMOON = 4;

		Place place;

		double jut = DegMin2DecDeg(ciCore.tim) + 0.5 / 3600 - ciCore.dst;
		int jhour = (int)jut;
		int jmin = (int)fmod(jut * 60, 60);
		int jsec = (int)fmod(jut * 3600, 60);
		timeP.time(ciCore.yea, ciCore.mon, ciCore.day, jhour, jmin, jsec, 0, FALSE, ciCore.zon, FALSE, place, TRUE);

		chart.ChartInit("Roberto", TRUE, timeP, place, RADIX, "", TRUE, 1, FALSE);

		this->time = chart.time;
		this->lon = chart.planets.planets[SE_MOON].data[LON];// 生时的月亮黄经

		this->flags = 0;

		if (eepp == 1)
			this->flags = SEFLG_SPEED | SEFLG_SWIEPH;
		else if (eepp == 0)
			this->flags = SEFLG_SPEED | SEFLG_JPLEPH;
		else if (eepp == 2)
			this->flags = SEFLG_SPEED | SEFLG_MOSEPH;

		if (options.topocentric)
			this->flags = this->flags | SEFLG_TOPOCTR;

		int add = 0;

		//		if (! chart.time.bc)
		//		{
		double lonsun = chart.planets.planets[SE_SUN].data[LON];  // 生时的太阳黄经
		double lonmoon = chart.planets.planets[SE_MOON].data[LON];// 生时的月亮黄经

		int d, m, s;
		decToDeg(lonsun, &d, &m, &s);
		lonsun = d + m / 60.0 + s / 3600.0;

		decToDeg(lonmoon, &d, &m, &s);
		lonmoon = d + m / 60.0 + s / 3600.0;

		double diff1 = lonmoon - lonsun;// 太阳与月亮的黄经差值
		isNewMoon(diff1, &newmoon, &ready);// diff1 为零则表示当前月相为新月，ready 为真则表示当前月相为新月或满月
		this->newmoon = newmoon;
		this->ready = ready;

		if (!this->ready)// 当前月相非新月非满月
		{
			BOOL new1 = this->newmoon;
			BOOL ok = this->getDateHour(this->time, chart.place, this->newmoon, TRUE);// 查找出生时间的前一个满月
			this->newmoon = new1;
			if (!this->ready)
			{
				ok = this->getDateMinute(this->time, chart.place, this->newmoon, TRUE);
				this->newmoon = new1;
				if (!this->ready)
				{
					ok = this->getDateSecond(this->time, chart.place, this->newmoon, TRUE);
					this->newmoon = new1;
				}
			}
		}

		Houses hses;
		hses.HousesInit(this->time.jd, 0, chart.place.lat, chart.place.lon, options.hsys, chart.obl[0], us.fSidereal, -is.rSid);

		Planet moon;
		moon.PlanetInit(this->time.jd, SE_MOON, this->flags, chart.place.lat, hses.ascmc2, 0.0, NULL, NULL, FALSE, 0.0, -1);

		if (this->newmoon)
			// 新月
			this->lon = moon.data[LON];
		else
		{
			// 满月
			if (options.syzmoon == options.MOON)
				this->lon = moon.data[LON];
			else if (options.syzmoon || options.ABOVEHOR)
			{
				if (moon.abovehorizon)
					this->lon = moon.data[LON];
				else
				{
					Planet sun;
					sun.PlanetInit(this->time.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
					this->lon = sun.data[LON];
				}
			}
			else
			{
				moon.PlanetInit(this->time.jd, SE_MOON, this->flags, chart.place.lat, chart.houses.ascmc2, 0.0, NULL, NULL, FALSE, -1);
				if (moon.abovehorizon)
					this->lon = moon.data[LON];
				else
				{
					Planet sun;
					sun.PlanetInit(this->time.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
					this->lon = sun.data[LON];
				}
			}
		}
		//}

		double xpo[3];
		xpo[0] = this->lon;
		xpo[1] = 0.0;
		xpo[2] = 1.0;
		double xpn[3];
		swe_cotrans(xpo, xpn, -chart.obl[0]);// ????
		double dist;
		double ra = xpn[0];
		double decl = xpn[1];
		dist = xpn[2];

		this->speculum[0] = this->lon;
		this->speculum[1] = 0.0;
		this->speculum[2] = ra;
		this->speculum[3] = decl;
		this->lon2 = chart.planets.planets[SE_MOON].data[LON]; //             M=103.58125004733833 本命盘的月亮经度
		//		if (! chart.time.bc)                                   //               103.58098776494091          
		//		{
		this->time2 = this->time;
		BOOL new1 = this->newmoon;
		this->ready2 = this->getDateHour(this->time2, chart.place, !this->newmoon, FALSE);
		this->newmoon = new1;
		if (!this->ready2)
		{
			this->ready2 = this->getDateMinute(this->time2, chart.place, !this->newmoon, FALSE);
			this->newmoon = new1;
			if (!this->ready2)
			{
				this->ready2 = this->getDateSecond(this->time2, chart.place, !this->newmoon, FALSE);
				this->newmoon = new1;
			}
		}
		hses2.HousesInit(this->time2.jd, 0, chart.place.lat, chart.place.lon, options.hsys, chart.obl[0], us.fSidereal, -is.rSid);
		Planet moon2;
		moon2.PlanetInit(this->time2.jd, SE_MOON, this->flags, chart.place.lat, hses2.ascmc2, 0.0, NULL, NULL, FALSE, 0.0, -1);
		if (!this->newmoon)
			// 满月
			this->lon2 = moon2.data[LON];
		else
		{
			// 新月
			if (options.syzmoon || options.MOON)
				this->lon2 = moon2.data[LON];
			else if (options.syzmoon || options.ABOVEHOR)
			{
				if (moon2.abovehorizon)
					this->lon2 = moon2.data[LON];
				else
				{
					Planet sun2;
					sun2.PlanetInit(this->time2.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
					this->lon2 = sun2.data[LON];
				}
			}
			else
			{
				moon2.PlanetInit(this->time2.jd, SE_MOON, this->flags, chart.place.lat, chart.houses.ascmc2, 0.0, NULL, NULL, FALSE, 0.0, -1);
				if (moon2.abovehorizon)
					this->lon2 = moon2.data[LON];
				else
				{
					Planet sun2;
					sun2.PlanetInit(this->time2.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
					this->lon2 = sun2.data[LON];
				}
			}
		}

		xpo[0] = this->lon2;
		xpo[1] = 0.0;
		xpo[2] = 1.0;
		//			double xpn[3];
		swe_cotrans(xpo, xpn, -chart.obl[0]);
		double ra2, decl2, dist2;
		ra2 = xpn[0];
		decl2 = xpn[1];
		dist2 = xpn[2];

		this->speculum2[0] = this->lon2;
		this->speculum2[1] = 0.0;
		this->speculum2[2] = ra2;
		this->speculum2[3] = decl2;

		this->lons[add] = this->lon;
		add++;//#Default

		if (this->newmoon)//#Conjunction
		{
			// 新月
			this->lons[add] = this->lon;
			add++;
		}
		else
		{
			// 满月
			this->lons[add] = this->lon2;
			add++;
		}

		hses.HousesInit(this->time.jd, 0, chart.place.lat, chart.place.lon, options.hsys, chart.obl[0], us.fSidereal, -is.rSid);
		Planet moonSyz;
		moonSyz.PlanetInit(this->time.jd, SE_MOON, this->flags, chart.place.lat, hses.ascmc2, 0.0, NULL, NULL, FALSE, 0.0, -1);
		hses2.HousesInit(this->time2.jd, 0, chart.place.lat, chart.place.lon, options.hsys, chart.obl[0], us.fSidereal, -is.rSid);
		Planet moonSyz2;
		moonSyz2.PlanetInit(this->time2.jd, SE_MOON, this->flags, chart.place.lat, hses2.ascmc2, 0.0, NULL, NULL, FALSE, 0.0, -1);

		if (!this->newmoon)//#Opposition
		{
			// 满月
			if (moonSyz.abovehorizon)
			{
				this->lons[add] = moonSyz.data[LON];
				add++;
			}
			else
			{
				Planet sun;
				sun.PlanetInit(this->time.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
				this->lons[add] = sun.data[LON];
				add++;
			}
		}
		else
		{
			// 新月
			if (moonSyz2.abovehorizon)
			{
				this->lons[add] = moonSyz2.data[LON];
				add++;
			}
			else
			{
				Planet sun2;
				sun2.PlanetInit(this->time2.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
				this->lons[add] = sun2.data[LON];
				add++;
			}
		}
		if (!this->newmoon)//#OppositionRadix
		{
			// 满月
			Planet moon;
			moon.PlanetInit(this->time.jd, SE_MOON, this->flags, chart.place.lat, chart.houses.ascmc2, 0.0, NULL, NULL, FALSE, 0.0, -1);
			if (moon.abovehorizon)
			{
				this->lons[add] = moon.data[LON];
				add++;
			}
			else
			{
				Planet sun;
				sun.PlanetInit(this->time.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
				this->lons[add] = sun.data[LON];
				add++;
			}
		}
		else
		{
			// 新月
			Planet moon;
			moon.PlanetInit(this->time2.jd, SE_MOON, this->flags, chart.place.lat, chart.houses.ascmc2, 0.0, NULL, NULL, FALSE, 0.0, -1);
			if (moon.abovehorizon)
			{
				this->lons[add] = moon.data[LON];
				add++;
			}
			else
			{
				Planet sun;
				sun.PlanetInit(this->time.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
				this->lons[add] = sun.data[LON];
				add++;
			}
		}

		if (!this->newmoon)//#Opposition Moon
		{
			// 满月
			this->lons[add] = moonSyz.data[LON];
			add++;
		}
		else
		{
			// 新月
			this->lons[add] = moonSyz2.data[LON];
			add++;
		}
		//}
	}
	BOOL getDateHour(Time tim, Place place, BOOL newmoonorig, BOOL flag)
	{
		Time time2;
		CopyMemory(&time2, &tim, sizeof(Time));
		int h, m, s, y, mo, d;
		Planet sun;
		Planet moon;
		double lonsun;
		double lonmoon;
		double diff;
		while (TRUE)
		{
			decToDeg(time2.time1, &h, &m, &s);
			y = time2.year;
			mo = time2.month;
			d = time2.day;

			BOOL caltype = (
				y < 1582 ||
				(y == 1582 && (mo < 10 || mo == 10 && d < 15)
					)) ? 0 : 1;

			double T1 = swe_julday(y, mo, d, h + m / 60.0 + s / 3600.0 - 1, caltype);
			double t1;
			swe_revjul(T1, caltype, &y, &mo, &d, &t1);

			t1 += 0.5 / 3600;
			h = (int)t1;
			m = (int)fmod(t1 * 60, 60);
			s = (int)fmod(t1 * 3600, 60);

			time2.time(y, mo, d, h, m, s, 1, TRUE, 0, FALSE, place, FALSE);

			sun.PlanetInit(time2.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
			moon.PlanetInit(time2.jd, SE_MOON, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
			lonsun = sun.data[LON];
			lonmoon = moon.data[LON];

			decToDeg(lonsun, &d, &m, &s);
			lonsun = d + m / 60.0 + s / 3600.0;
			decToDeg(lonmoon, &d, &m, &s);
			lonmoon = d + m / 60.0 + s / 3600.0;

			diff = lonmoon - lonsun;
			isNewMoon(diff, &newmoon, &this->ready);
			if (newmoon != newmoonorig || this->ready)
			{
				if (flag)
					CopyMemory(&this->time, &time2, sizeof(time2));
				else
					CopyMemory(&this->time2, &time2, sizeof(time2));
				return this->ready;
			}
		}

		if (flag)
			CopyMemory(&this->time, &time2, sizeof(time2));
		else
			CopyMemory(&this->time2, &time2, sizeof(time2));

		return FALSE;
	}

	BOOL getDateMinute(Time tim, Place place, BOOL newmoonorig, BOOL flag)
	{
		Time time2;
		CopyMemory(&time2, &tim, sizeof(Time));

		int h, m, s;
		decToDeg(tim.time1, &h, &m, &s);
		int y = tim.year;
		int mo = tim.month;
		int d = tim.day;

		//h += 1;
		//if (h > 23)
		//{
		//	h = 0;
		//	incrDay(&y, &mo, &d);
		//}
		BOOL caltype = (
			y < 1582 ||
			(y == 1582 && (mo < 10 || mo == 10 && d < 15)
				)) ? 0 : 1;

		double T1 = swe_julday(y, mo, d, h + m / 60.0 + s / 3600.0 + 1, caltype);
		double t1;
		swe_revjul(T1, caltype, &y, &mo, &d, &t1);

		t1 += 0.5 / 3600;
		h = (int)t1;
		m = (int)fmod(t1 * 60, 60);
		s = (int)fmod(t1 * 3600, 60);

		time2.time(y, mo, d, h, m, s, 1, TRUE, 0, FALSE, place, FALSE);

		while (TRUE)
		{
			int h, m, s;
			decToDeg(time2.time1, &h, &m, &s);
			int y = time2.year, mo = time2.month, d = time2.day;
			subtractMins(&y, &mo, &d, &h, &m, 1);
			if (y == 0)
			{
				y = 1;
				this->time.time(y, mo, d, h, m, s, 1, TRUE, 0, FALSE, place, FALSE);
				return TRUE;
			}

			time2.time(y, mo, d, h, m, s, 1, TRUE, 0, FALSE, place, FALSE);

			Planet sun;
			sun.PlanetInit(time2.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
			Planet moon;
			moon.PlanetInit(time2.jd, SE_MOON, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
			double lonsun = sun.data[LON];
			double lonmoon = moon.data[LON];

			decToDeg(lonsun, &d, &m, &s);
			lonsun = d + m / 60.0 + s / 3600.0;
			decToDeg(lonmoon, &d, &m, &s);
			lonmoon = d + m / 60.0 + s / 3600.0;

			double diff = lonmoon - lonsun;
			isNewMoon(diff, &this->newmoon, &this->ready);
			if (this->newmoon != newmoonorig || this->ready)
			{
				if (flag)
					CopyMemory(&this->time, &time2, sizeof(time2));
				else
					CopyMemory(&this->time2, &time2, sizeof(time2));
				return ready;
			}
		}

		if (flag)
			CopyMemory(&this->time, &time2, sizeof(time2));
		else
			CopyMemory(&this->time2, &time2, sizeof(time2));
		return FALSE;
	}
	BOOL getDateSecond(Time tim, Place place, BOOL newmoonorig, BOOL flag)
	{
		Time time2;
		CopyMemory(&time2, &tim, sizeof(Time));

		int h, m, s;
		decToDeg(tim.time1, &h, &m, &s);
		int y = tim.year, mo = tim.month, d = tim.day;
		addMins(&y, &mo, &d, &h, &m, 1);

		time2.time(y, mo, d, h, m, s, 1, TRUE, 0, FALSE, place, FALSE);

		int qty = 0;

		while (qty < 60)
		{
			int h, m, s;
			decToDeg(time2.time1, &h, &m, &s);
			int y = time2.year, mo = time2.month, d = time2.day;
			subtractSecs(&y, &mo, &d, &h, &m, &s, 1);
			if (y == 0)
			{
				y = 1;
				time2.time(y, mo, d, h, m, s, 1, TRUE, 0, FALSE, place, FALSE);

				if (flag)
					CopyMemory(&this->time, &time2, sizeof(time2));
				else
					CopyMemory(&this->time2, &time2, sizeof(time2));

				return TRUE;
			}

			time2.time(y, mo, d, h, m, s, 1, TRUE, 0, FALSE, place, FALSE);

			Planet sun;
			sun.PlanetInit(time2.jd, SE_SUN, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
			Planet moon;
			moon.PlanetInit(time2.jd, SE_MOON, this->flags, NULL, NULL, 0.0, NULL, NULL, FALSE, 0.0, 0);
			double lonsun = sun.data[LON];
			double lonmoon = moon.data[LON];

			decToDeg(lonsun, &d, &m, &s);
			lonsun = d + m / 60.0 + s / 3600.0;
			decToDeg(lonmoon, &d, &m, &s);
			lonmoon = d + m / 60.0 + s / 3600.0;

			double diff = lonmoon - lonsun;
			isNewMoon(diff, &this->newmoon, &this->ready);
			if (this->newmoon != newmoonorig || this->ready)
			{
				if (flag)
					CopyMemory(&this->time, &time2, sizeof(time2));
				else
					CopyMemory(&this->time2, &time2, sizeof(time2));
				return ready;
			}
			qty++;
		}

		if (flag)
			CopyMemory(&this->time, &time2, sizeof(time2));
		else
			CopyMemory(&this->time2, &time2, sizeof(time2));

		return FALSE;
	}
};
#endif

#include "useswe/useswe.h"
UseSwe useswe;

#if defined(_WIN32)
    #define STR_CMPI _stricmp
	#define MEM_CCPY _memccpy
	#define STR_DUP _strdup
	#define X_MEMSET x_memset_Intel
#else
	#include <string.h>
	#define STR_CMPI strcasecmp
	#define MEM_CCPY memccpy
	#define STR_DUP strdup
	#define X_MEMSET x_memset_GCC_CLANG
#endif