#ifndef _USESWE_H_
#define _USESWE_H_

/* Month index values */
typedef enum
{
	mJan = 1,
	mFeb,
	mMar,
	mApr,
	mMay,
	mJun,
	mJul,
	mAug,
	mSep,
	mOct,
	mNov,
	mDec
} month_t;

#define yeaJ2G     1582
#define monJ2G     mOct

#if defined(_WIN32)
	#include "windows.h"
	#include "../swe/inc/swedll.h"
#else
	#include "../swe/inc/swephexp.h"
	#include "../swe/inc/sweph.h"
	#include "../swe/inc/swephlib.h"
	#include "../swe/inc/swehouse.h"
	#include "../swe/inc/sweodef.h"
#endif

class UseSwe
{
private:
    char* m_sweVersion=nullptr;//swe版本号
    int mIn_mon,mIn_day,mIn_yea;//输入的月日年
	double mIn_tim;
	double mIn_lon,mIn_lat,mIn_height;//输入的经纬度
    int32 mIn_mod;
    double mIn_JD,mIn_tjd;

    int32 nSiderealMode;

    double mOut_MdyToJulian;
    int mOut_mon,mOut_day,mOut_yea;
    int mOut_deltat;
    double mOut_ayanamsa;
public:
	//UseSwe();
	//~UseSwe();
	char* getSweVersion();//获取swe版本号
    void setMYD(int mon, int day, int yea);
    void getMYD(int *mon, int *day, int *yea);
    void setMdyToJulian();
    double getMdyToJulian();
    void setMod(int32 mod);
    int32 getMod();
    void setDeltat(double JD);
    double getDeltat();
    void setAyanamsa(double tjd);
    double getAyanamsa();
    void sweCalc(double tjd, int ipl, int32 iflag, double *xx,char *serr); 
};

#endif