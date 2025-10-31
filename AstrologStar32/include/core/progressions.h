#ifndef PROGRESSIONS_H
#define PROGRESSIONS_H

#include "planet.h"
#include "houses.h"
#include "../models/chart_data.h"
#include <vector>

class ProgressionCalculator {
public:
    // 计算次限推进（Secondary Progressions）
    static void calculateSecondaryProgressions(const BirthData& birthData, 
                                             double progressionTime,
                                             std::vector<PlanetData>& progressedPlanets,
                                             HouseData& progressedHouses);
    
    // 计算太阳弧推进（Solar Arc Progressions）
    static void calculateSolarArcProgressions(const BirthData& birthData,
                                            double progressionTime,
                                            std::vector<PlanetData>& progressedPlanets);
    
    // 计算日返盘（Solar Return）
    static void calculateSolarReturn(const BirthData& birthData,
                                   int returnYear,
                                   BirthData& solarReturnData);
    
    // 计算月返盘（Lunar Return）
    static void calculateLunarReturn(const BirthData& birthData,
                                   double returnTime,
                                   BirthData& lunarReturnData);
    
    // 获取推进因子（Naibod法则等）
    static double getProgressionFactor(int method = 0);
    
    // 转换为推进时间
    static double convertToProgressedTime(const BirthData& birthData, 
                                        double progressionFactor,
                                        int daysFromBirth);

private:
    // 内部辅助函数
    static double calculateSolarArcOffset(double natalSunLon, double currentSunLon);
};

// 推进方法常量
#define PROGRESSION_SECONDARY 1
#define PROGRESSION_SOLAR_ARC 2
#define PROGRESSION_LUNAR 3
#define PROGRESSION_ANNUAL 4

// 推进因子计算方法
#define PROGRESSION_FACTOR_NAIBOD 0      // 1度=1年
#define PROGRESSION_FACTOR_PRECISE 1     // 精确计算
#define PROGRESSION_FACTOR_DEFAULT 0     // 默认使用Naibod法则

#endif // PROGRESSIONS_H