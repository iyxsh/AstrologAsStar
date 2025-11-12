#ifndef CHART_H
#define CHART_H

#include "../models/chart_data.h"
#include "../utils/utils.h"

extern double ppower1[];

// 星盘计算相关常量和函数声明
int computeRiseSet();
double CastChart(bool fDate);
void ComputeInfluence(void);
wchar_t Dignify(int obj, int sign,bool chs);
char *DignifyA(int obj, int sign);

#endif // CHART_H