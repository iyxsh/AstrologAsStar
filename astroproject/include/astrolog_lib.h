#ifndef ASTROLOG_LIB_H
#define ASTROLOG_LIB_H

#include "models/chart_data.h"
#include <string>

#ifdef _WIN32
#  ifdef ASTROLOG32_STATIC_LIB
#    define ASTROLOG32_API
#  else
#    ifdef astrolog32_EXPORTS
#      define ASTROLOG32_API __declspec(dllexport)
#    else
#      define ASTROLOG32_API __declspec(dllimport)
#    endif
#  endif
#else
#  define ASTROLOG32_API
#endif

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

/**
 * 星盘输入数据结构体
 */
typedef struct _ChartInput {
	int mon;					/* Month            */
	int day;					/* Day              */
	int yea;					/* Year             */
	double tim;					/* Time in hours    */
	double dst;					/* Daylight offset  */
	double zon;					/* Time zone        */
	double lon;					/* Longitude        */
	double lat;					/* Latitude         */
	double alt;
	wchar_t nam[256];			/* Name for chart   */
	wchar_t loc[256];			/* Name of location */
} ChartInput;

/**
 * 初始化环境
 */

ASTROLOG32_API void initEnv();

/**
 * 解析字符串,返回double类型, pm 为解析模式 使用此文件中的解析模式枚举
 */
ASTROLOG32_API double RParseSz(const char* szEntry, int pm);

/**
 * 设置星盘数据
 * @param chartInput - 星盘输入数据
 * @param useInput - 是否使用输入数据，默认为true
 */
ASTROLOG32_API void SetChartData(const int ChartMode,const ChartInput& chartInput, bool useInput = true);

/**
 * 获取星盘结果
 * @return 星盘结果的宽字符串表示
 */
ASTROLOG32_API std::wstring GetMainChartAspect();
ASTROLOG32_API std::wstring GetChartAspectRelation();
ASTROLOG32_API void GetChartResult(CI& ciInput,bool useInput = true); 

#endif // ASTROLOG_LIB_H
