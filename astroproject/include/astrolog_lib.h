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

/* P0.3 — machine-readable chart output for astrolog32-cli.
 * GetChartMachineText() reproduces the original @0203 format (9-decimal
 * positions) so output can be diffed against the original-engine golden
 * samples. GetChartJSON() emits the same data as JSON. */
ASTROLOG32_API void SetSilent(bool silent);
ASTROLOG32_API std::wstring GetChartMachineText(const ChartInput& chartInput);
ASTROLOG32_API std::string GetChartJSON(const ChartInput& chartInput);

/* P2/A8 — 次限推进盘机器文本（@0203 同格式）。natal=本命；tgtMon/Day/Yea/Tim
 * =推进目标日期时刻（地点沿用本命：dst/zon 与本命一致传入）。推进公式与原版
 * rcProgress 一致（is.T = natal_UT + (JDp-natal_UT)/rProgDay，rProgDay=365.24219）。 */
ASTROLOG32_API std::wstring GetProgressedMachineText(const ChartInput& natal,
	int tgtMon, int tgtDay, int tgtYea, const char* tgtTim, double tgtDst, double tgtZon);
/* P2/A8-1 — 行运盘机器文本（rcTransit 镜像，单盘数值 ≡ 目标时刻普通盘）。 */
ASTROLOG32_API std::wstring GetTransitMachineText(const ChartInput& target);
/* P2/A8-2 — 太阳/月亮弧方向机器文本（mode 1=naive 度/年, 2=太阳弧, 4=月亮弧）。 */
ASTROLOG32_API std::wstring GetSolarArcMachineText(const ChartInput& natal,
	int tgtMon, int tgtDay, int tgtYea, const char* tgtTim, int mode,
	double tgtDst, double tgtZon);
/* P2/A9 — 太阳返照盘机器文本（引擎空间解算 civil 时刻后 cast；地点/时区沿用本命）。 */
ASTROLOG32_API std::wstring GetSolarReturnMachineText(const ChartInput& natal, int year);
/* P2/A10-1 — 合成盘（Composite）机器文本/@0203 与 JSON。
 * 双 ChartInput（a=盘1, b=盘2）经 SetChartData 同款 ciTwin 通道注入后镜像原版
 * CastRelation rcComposite 分支（astrolog.cpp:3765-3800）：全体 longitude/latitude/
 * vel/cusp 取两盘 50:50 中点（Ratio()，跨 180° 补 Mod(+360*ratio)），cusp 互补对/
 * ASC 与宫头 >90° 差补 180 校正，宫位按合成宫头落宫。中点=两独立本命盘算术中点，
 * 数值正确性由 unit_composite 恒等断言锁定（A/B==A 恒等 + 全对象中点公式）。 */
ASTROLOG32_API std::wstring GetCompositeMachineText(const ChartInput& chartA,
	const ChartInput& chartB);
ASTROLOG32_API std::string GetCompositeChartJSON(const ChartInput& chartA,
	const ChartInput& chartB);
/* P2/A10-2 — 时空中点（Midpoint / -rm）机器文本/@0203 与 JSON。
 * 双 ChartInput（a=盘1, b=盘2）复用 A10-1 双盘通道，us.nRel=rcMidpoint：时空中点 =
 * 两盘时刻与地理坐标各取 50:50 中点（跨 180° 经度补 Mod(+360*ratio)）后按中点时空
 * 重新 cast（金样路线同 A8/A10 已关，验证走自反/对称/数值锚定）。 */
ASTROLOG32_API std::wstring GetMidpointMachineText(const ChartInput& chartA,
	const ChartInput& chartB);
ASTROLOG32_API std::string GetMidpointChartJSON(const ChartInput& chartA,
	const ChartInput& chartB);
/* P2/A10-3 — 关系网格（Synastry grid）机器文本/@0403 与 JSON。
 * 双 ChartInput（a=盘1=A, b=盘2=B）复用 A10-1/2 双盘通道，us.nRel=rcDual 让 CastRelation
 * 把两盘分别 cast 入 cp1(=A)/cp2(=B)，再 FCreateGridRelation(FALSE) 把全局 grid 填为
 * 「盘A×盘B 交叉相位」（grid->n[i][j]=aspect，i=cp2=B, j=cp1=A）。机器文本每行：
 *   <objA_idx> <asp_abbr> <objB_idx> <lonA> <lonB> <orb_deg> <exact_deg>
 * 对象序号沿用引擎 numbering（0=Earth,1=Sun,2=Moon,...见 s_szObjShortNameEnglish），asp 取
 * tAspectAbbrev（Con/Opp/Squ/Tri/Sex/...）。忽略集同步 ignore2=ignore3=ignore1，使网格只用
 * 单一对象集（默认不纳入恒星）。正确性由 unit_synastry 对称/角度自洽/泄漏 断言锁定。 */
ASTROLOG32_API std::wstring GetSynastryGridMachineText(const ChartInput& chartA,
	const ChartInput& chartB);
ASTROLOG32_API std::string GetSynastryGridJSON(const ChartInput& chartA,
	const ChartInput& chartB);
/* P2/A15 — 日月食表（对齐 golden PrintSolarEclipse / PrintLunarEclipse）。
 * 以盘年份为窗口（Jan 1 00:00 UT 起）循环 swe_sol/lun_eclipse_when_glob 求下一次食的
 * 极大时刻，越年即止（≤10 条）；时刻按盘时区 zon 转本地时（swe_utc_time_zone）。
 * 机器文本：@0503 日食表 / @0504 月食表，每行
 *   <year> <mon> <day> <hour> <min> <type> <jd_ut> <mag> <lon> <lat>
 * type 取英文稳定 token（Total/Annular/AnnularTotal/Partial/Penumbral/NonCentral）；
 * lon/lat = 极大点地理坐标（月食恒 0），mag = 食分（月食 umbral magnitude）。
 * 正确性由 unit_eclipse 锁定：2026 已知食（2 月环食 / 8 月全食 / 3 月全影月食）
 * + 「极大时刻即日月距角极值点」自洽断言。 */
ASTROLOG32_API std::wstring GetSolarEclipseTableText(const ChartInput& chart);
ASTROLOG32_API std::wstring GetLunarEclipseTableText(const ChartInput& chart);
ASTROLOG32_API std::string GetSolarEclipseTableJSON(const ChartInput& chart);
ASTROLOG32_API std::string GetLunarEclipseTableJSON(const ChartInput& chart);
/* P2/轴3 — 原版 -a 相位表机器行（ChartAspect 同装配，字符串版）。
 * 前置：调用方需已 cast 盘并 FCreateGrid(false) 填充全局 grid（同 --grands 路径）。
 * 每行 `<rank>: <obj1>(sign1) <ASP> (sign2)<obj2> | <orb>°<mm>' <app> | <power> |`
 * （rank 自 1、power 降序、orb 弧分取整、app = +/- 或 a/s），供轴3 容许度金样对拍。 */
ASTROLOG32_API std::wstring GetChartAspectMachineText();

#endif // ASTROLOG_LIB_H
