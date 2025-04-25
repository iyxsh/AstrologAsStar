#include "useswe.h"

char* UseSwe::getSweVersion()
{
    char *sweVersion = new char[16];
    this->m_sweVersion = swe_version(sweVersion);
    return this->m_sweVersion;
}

void UseSwe::setMYD(int mon,int day,int yea)
{
    this->mIn_mon = mon;
    this->mIn_day = day;
    this->mIn_yea = yea;
}

void UseSwe::getMYD(int *mon, int *day, int *yea)
{
    *mon = this->mOut_mon;
    *day = this->mOut_day;
    *yea = this->mOut_yea;
}

/* Given a month, day, and year, convert it into a single Julian day value, */
/* i.e. the number of days passed since a fixed reference date.             */
void UseSwe::setMdyToJulian() {
	int fGreg = true;
	if (this->mIn_yea < yeaJ2G || (this->mIn_yea == yeaJ2G && (this->mIn_mon < monJ2G || (this->mIn_mon == monJ2G && this->mIn_day < 15))))
		fGreg = false;
	//	long t = floor(swe_julday(yea, mon, day, 12.0, fGreg) + 0.5);
	//	long t1 = floor(swe_julday(yea, mon, day, 12.0, TRUE) + 0.5);
	this->mOut_MdyToJulian = (double)floor(swe_julday(this->mIn_yea, this->mIn_mon, this->mIn_day, 12.0, fGreg) + 0.5);
}

double UseSwe::getMdyToJulian(){return mOut_MdyToJulian;}

void UseSwe::setMod(int32 mod){
    this->nSiderealMode = mod;
    swe_set_sid_mode(this->nSiderealMode, 0, 0);
}
int32 UseSwe::getMod(){return this->nSiderealMode;}

void UseSwe::setDeltat(double JD)
{
    this->mIn_JD = JD;
    this->mOut_deltat = swe_deltat(this->mIn_JD);
}

double UseSwe::getDeltat(){return this->mOut_deltat;}

void UseSwe::setAyanamsa(double tjd)
{
    this->mIn_tjd = tjd;
    this->mOut_ayanamsa = swe_get_ayanamsa(this->mIn_tjd);
}
double UseSwe::getAyanamsa(){return this->mOut_ayanamsa;}

void UseSwe::sweCalc(double tjd, int ipl, int32 iflag, double *xx, char *serr)
{
    swe_calc(tjd, ipl, iflag, xx, serr);
}
