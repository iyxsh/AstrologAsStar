/* ============================================================================
 * STUB — 重构占位模块（当前零调用，编译期不可达）
 * ----------------------------------------------------------------------------
 * 原版（A32_V3_51_Proj_2022 MFC 工程）中本函数为完整实现；2026-08 重构后
 * CastChart 主链仅走瑞士星历路径（src/core/ephemeris.cpp
 * ComputeWithSwissEphemeris / CalculatePlanetSE），本模块保留为 P2 功能重写
 * 前的骨架：
 *   - 全仓无任何调用方（仅头文件被 astrolog.h 聚合包含），不参与运行时主链；
 *   - 仍调用旧 Kepler 引擎 ComputePlanets()（见 planet.cpp LEGACY 标注），
 *     P2 重写后随 Kepler 引擎一并移除；
 *   - 函数体内时间换算/循环仅为示意，与原版语义不对齐，勿据此断言行为。
 * P2 计划：按原版语义重写并接入 ChartMode/命令行开关后，删除本横幅与本骨架。
 * ==========================================================================*/

#include "../../include/core/progressions.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/core/planet.h"
#include "../../include/core/houses.h"
#include "../../include/utils/utils.h"
#include <math.h>

extern US us;
extern CI ciTran;
extern IS  is;
extern double Longit;
extern double Latit;

void CastProgressions(bool fDate)
{
	double Off = 0.0, j;
	double ep1 = 0.0;
	int i;
	is.rSid = 0.0;
	Longit = DegMin2DecDeg(ciTran.lon);
	Latit = DegMin2DecDeg(ciTran.lat);

	//is.JD = MdytszToJulianDay(ciTran.mon, ciTran.day, ciTran.yea, ciTran.tim, ciTran.dst, ciTran.zon);
	//j = JulianDayToTime(is.JD);

	if (us.fProgressUS) {
		// 处理次限进展
		is.T = is.JD - 2415020.0;
		is.T = (is.T - 0.5) / 365.25;
	}
	else {
		// 处理太阳弧进展
		is.T = is.JD - 2415020.0;
		is.T = (is.T - 0.5) / 365.25;
	}

	ComputePlanets();
	ComputeInHouses();
}