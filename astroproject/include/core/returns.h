#ifndef RETURNS_H
#define RETURNS_H

/* P2/A9-0 太阳返照（2026-09-05，定义驱动）。
 *
 * 太阳返照盘 = 太阳再次回到本命黄经那一时刻所起的盘。golden 原版经
 * ChartTransitSearch（行运搜索内核，96 段/月 + 段内线性插值）解算；本地无该内核，
 * 且 A9 属整段移植大模块。为不臆造 golden 步进语义，本模块按**天文定义**直接
 * swe 迭代求「目标年内 Sun 黄经 == 本命 Sun 黄经」的时刻（精度远高于原版分段
 * 插值），供返照盘 cast 与对拍使用。moment 采用 UT(JD)，Sun 计算路径与主链
 * CastChart/CalculatePlanetSE 一致（tropical 视黄经，jde = jd + ΔT）。
 *
 * 注：本模块只解时刻，不涉引擎 civil 时区转换；返照盘 cast（ChartInput 装配）
 * 与 golden 语义对齐留待 A9-1。
 */

/* 目标年内首次太阳返照时刻（UT JD）。natalSunLonDeg=本命太阳黄经(0..360)，
 * targetYear=目标年（≥1）。失败返回 NaN。 */
double SolarReturnJulian(double natalSunLonDeg, int targetYear);

/* 引擎同路径太阳视黄经（deg，0..360），供身份恒等校验。jdUt 为 UT JD。 */
double SunEclipticLon(double jdUt);

#endif // RETURNS_H
