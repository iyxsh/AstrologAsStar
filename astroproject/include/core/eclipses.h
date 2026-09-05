#ifndef ECLIPSES_H
#define ECLIPSES_H

/* P2/A15 日月食表（2026-09-05，对齐 golden PrintSolarEclipse/PrintLunarEclipse）。
 *
 * golden 语义（astrolog.cpp:31805/31827 → solar_eclipse()/lunar_eclipse()）：
 *   以**当前盘年份**为窗口（Jan 1 00:00 UT 起），循环 swe_sol/lun_eclipse_when_glob
 *   求下一次食的极大时刻，越年（UT 年 > 盘年）即止，最多 10 次；时刻按 ciMain.zon
 *   转本地时（swe_utc_time_zone），逐行输出「日期时刻 + 食型」。
 *   原版推进为 `t_ut += direction`（direction 初值 0）→ 依赖 swe 自身向前搜索；本实现
 *   显式 `t = tret[0] + 0.5`（相邻同类食间隔 ≥ 29d，0.5d 既保证前移又不会漏食）。
 *
 * 本地无 .se1 星历文件时回退 Moshier（与主链 planet.cpp/returns.cpp 同策略）；
 * 食分与极大点地理坐标取自 swe_sol_eclipse_where（geopos[0/1]=极大点经纬）
 * / swe_lun_eclipse_how（attr[0]=umbral magnitude）。
 */

#include "../astrolog_lib.h"   /* ChartInput */
#include <vector>
#include <string>

/* 食型（稳定 token，映射自 SE_ECL_* 位标志；顺序见 .cpp Map* eclipse 表） */
enum EclipseType {
	eclNone          = 0,
	eclPenumbral     = 1,   /* 半影月食 */
	eclTotalPenumbral= 2,   /* 全半影月食（保留位） */
	eclPartial       = 3,   /* 偏食 */
	eclAnnular       = 4,   /* 环食 */
	eclAnnularTotal  = 5,   /* 全环食（hybrid） */
	eclTotal         = 6,   /* 全食 */
	eclNonCentral    = 7    /* 非中心食 */
};

struct EclipseEvent {
	int    type;            /* EclipseType */
	int    year, mon, day;  /* 本地日期（盘时区 zon） */
	int    hour, minute;    /* 本地时刻（盘时区 zon） */
	double jdUt;            /* 极大时刻 UT JD */
	double mag;             /* 食分（日食 attr[0]；月食 umbral magnitude） */
	double lon, lat;        /* 极大点地理坐标（度，东经/北纬为正） */
};

/* 本命盘所在年份的全部日食 / 月食（按 UT JD 升序；越年即止，≤10 条）。 */
std::vector<EclipseEvent> SolarEclipseTable(const ChartInput& chart);
std::vector<EclipseEvent> LunarEclipseTable(const ChartInput& chart);

/* 食型 token（英文稳定串，机器文本与 JSON 共用）。 */
const char* EclipseTypeName(int type);

#endif // ECLIPSES_H
