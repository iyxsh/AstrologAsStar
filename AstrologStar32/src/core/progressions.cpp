#include "../../include/core/progressions.h"
#include "../../include/core/ephemeris.h"
#include "../../swe/inc/swephexp.h"
#include <cmath>

// 计算次限推进（Secondary Progressions）
void ProgressionCalculator::calculateSecondaryProgressions(const BirthData& birthData, 
                                                        double progressionTime,
                                                        std::vector<PlanetData>& progressedPlanets,
                                                        HouseData& progressedHouses) {
    // 计算推进因子
    double progressionFactor = getProgressionFactor(PROGRESSION_FACTOR_NAIBOD);
    
    // 计算推进后的儒略日
    double progressedJD = progressionTime;
    
    // 计算推进后的行星位置
    PlanetCalculator::calculateAllPlanets(progressedJD, progressedPlanets);
    
    // 计算推进后的宫位
    HouseCalculator::calculateHouses(progressedJD, birthData.latitude, birthData.longitude, 0, &progressedHouses);
}

// 计算太阳弧推进（Solar Arc Progressions）
void ProgressionCalculator::calculateSolarArcProgressions(const BirthData& birthData,
                                                        double progressionTime,
                                                        std::vector<PlanetData>& progressedPlanets) {
    // 首先计算出生时的行星位置
    double birthJD = EphemerisCalculator::calculateJulianDay(
        birthData.year, birthData.month, birthData.day, 
        birthData.time - birthData.timezone + birthData.dst);
    
    std::vector<PlanetData> natalPlanets;
    PlanetCalculator::calculateAllPlanets(birthJD, natalPlanets);
    
    // 计算当前太阳的位置
    PlanetData currentSun;
    PlanetCalculator::calculatePlanetPosition(progressionTime, SE_SUN, &currentSun);
    
    // 计算出生时太阳的位置
    PlanetData natalSun;
    PlanetCalculator::calculatePlanetPosition(birthJD, SE_SUN, &natalSun);
    
    // 计算太阳弧偏移
    double solarArcOffset = calculateSolarArcOffset(natalSun.longitude, currentSun.longitude);
    
    // 应用太阳弧偏移到所有行星
    progressedPlanets = natalPlanets;
    for (size_t i = 0; i < progressedPlanets.size(); i++) {
        progressedPlanets[i].longitude += solarArcOffset;
        // 确保经度在0-360范围内
        if (progressedPlanets[i].longitude < 0) 
            progressedPlanets[i].longitude += 360.0;
        if (progressedPlanets[i].longitude >= 360.0) 
            progressedPlanets[i].longitude -= 360.0;
    }
}

// 计算日返盘（Solar Return）
void ProgressionCalculator::calculateSolarReturn(const BirthData& birthData,
                                              int returnYear,
                                              BirthData& solarReturnData) {
    // 复制出生数据作为基础
    solarReturnData = birthData;
    solarReturnData.year = returnYear;
    
    // 这里应该实现精确的日返时间计算
    // 简化处理：使用相同的月份和日期
    // 实际实现需要计算太阳回到出生时的确切位置的时间
}

// 计算月返盘（Lunar Return）
void ProgressionCalculator::calculateLunarReturn(const BirthData& birthData,
                                              double returnTime,
                                              BirthData& lunarReturnData) {
    // 复制出生数据作为基础
    lunarReturnData = birthData;
    
    // 这里应该实现精确的月返时间计算
    // 简化处理：基于给定的返回时间
}

// 获取推进因子（Naibod法则等）
double ProgressionCalculator::getProgressionFactor(int method) {
    switch (method) {
        case PROGRESSION_FACTOR_PRECISE:
            // 精确计算：太阳平均日行度数
            return 0.9833; // 约为 59'/天
        case PROGRESSION_FACTOR_NAIBOD:
        default:
            // Naibod法则：1度=1年
            return 1.0;
    }
}

// 转换为推进时间
double ProgressionCalculator::convertToProgressedTime(const BirthData& birthData, 
                                                    double progressionFactor,
                                                    int daysFromBirth) {
    // 根据推进因子计算推进时间
    double progressedDays = daysFromBirth * progressionFactor;
    
    // 计算出生儒略日
    double birthJD = EphemerisCalculator::calculateJulianDay(
        birthData.year, birthData.month, birthData.day, 
        birthData.time - birthData.timezone + birthData.dst);
    
    // 返回推进后的儒略日
    return birthJD + progressedDays;
}

// 计算太阳弧偏移
double ProgressionCalculator::calculateSolarArcOffset(double natalSunLon, double currentSunLon) {
    double offset = currentSunLon - natalSunLon;
    
    // 处理360度循环
    if (offset < 0) offset += 360.0;
    if (offset >= 360.0) offset -= 360.0;
    
    return offset;
}