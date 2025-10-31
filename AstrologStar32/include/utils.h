#ifndef UTILS_H
#define UTILS_H

// 工具函数声明

// 解析模式枚举
#define pmMon     1
#define pmDay     2
#define pmYea     3
#define pmTim     4
#define pmDst     5
#define pmZon     6
#define pmLon     7
#define pmLat     8
#define pmObject  9
#define pmAspect  10
#define pmHouseSystem 11
#define pmSign    12
#define pmColor   13
#define pmMonEn   14
#define pmSignEn  15
#define pmObjectEn 16
#define pmWeek    18

// 解析函数
double RParseSz(const char* szEntry, int pm);
int NParseSzW(wchar_t* szEntry, int pm);

#endif // UTILS_H