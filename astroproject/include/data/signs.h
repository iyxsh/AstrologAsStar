#ifndef SIGNS_H
#define SIGNS_H
#define NUMBER_OF_SIGNS		12
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

/* Elements */
typedef enum
{
	eFir,
	eEar,
	eAir,
	eWat
} elements_t;

/* Zodiac signs */
typedef enum
{
	sAri = 1,
	sTau,
	sGem,
	sCan,
	sLeo,
	sVir,
	sLib,
	sSco,
	sSag,
	sCap,
	sAqu,
	sPis
} zodiac_signs_t;

#endif // SIGNS_H