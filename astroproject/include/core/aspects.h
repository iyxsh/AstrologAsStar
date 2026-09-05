#ifndef ASPECTS_H
#define ASPECTS_H

#include "../utils/utils.h"
#include "../models/chart_data.h"
#include "../models/settings.h"

/* Aspects */
typedef enum
{
	aCon = 1,
	aOpp,
	aSqu,
	aTri,
	aSex,
	aInc,				// 6
	aSSx,
	aSSq,
	aSes,
	aQui,
	aBQn,
	cAspectInt = aBQn,	// 11
	aSQn,
	aSep,
	aNov,
	aBNv,
	aBSp,
	aTSp,
	aQNv,
	cAspect = aQNv		// 18
} aspects_t;

/* Aspect configurations */
#define acS  1
#define acGT 2
#define acTS 3
#define acY  4
#define acGC 5
#define acC  6
#define acMR 7
#define acK  8

extern GridInfo *grid;
extern US us;
extern byte ignoreA[];      /* 相位屏蔽表（-RA/-RE 可改写；默认 6..18 屏蔽） */
extern IS is;

// 函数声明
void *allocate(long length, char *user);
void UpdateAspectCount(void);   /* 相位数量重算：nAsp = cAspect − 被屏蔽数（A6） */
bool FCreateGridA(bool fFlip);
bool FCreateGrid(bool fFlip);
void ComputeAspects(void);
void GetAspect(double *planet1, double *planet2, double *ret1, double *ret2, int i, int j);
bool FAcceptAspect(int obj1, int asp, int obj2);
bool FCreateGridRelation(bool fMidpoint);
#endif // ASPECTS_H