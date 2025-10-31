#include "../../include/core/transits.h"
#include "../../swe/inc/swephexp.h"
#include <algorithm>
#include <cmath>

// 计算行星对本命盘的推运相位
void TransitCalculator::calculateTransitAspects(const std::vector<PlanetData>& natalPlanets,
                                             double transitTime,
                                             const AspectSettings& settings,
                                             std::vector<TransitEvent>& transitEvents) {
    transitEvents.clear();
    
    // 计算推运时的行星位置
    std::vector<PlanetData> transitPlanets;
    calculateTransitPlanets(transitTime, transitPlanets);
    
    // 检查每个推运行星与本命行星的相位
    for (size_t i = 0; i < transitPlanets.size(); i++) {
        for (size_t j = 0; j < natalPlanets.size(); j++) {
            double transitingLon = transitPlanets[i].longitude;
            double natalLon = natalPlanets[j].longitude;
            
            // 检查所有启用的相位类型
            for (int aspectType = 1; aspectType <= 11; aspectType++) {
                if (!settings.enabled[aspectType]) continue;
                
                double orb;
                if (AspectCalculator::checkAspect(transitingLon, natalLon, aspectType, 
                                                settings.orbLimits[aspectType], &orb)) {
                    TransitEvent event;
                    event.transitingPlanet = i;
                    event.natalPlanet = j;
                    event.aspectType = aspectType;
                    event.orb = orb;
                    event.exactTime = transitTime;
                    event.applying = AspectCalculator::isAspectApplying(
                        transitingLon, natalLon, 
                        transitPlanets[i].speed, natalPlanets[j].speed, 
                        aspectType);
                    
                    transitEvents.push_back(event);
                }
            }
        }
    }
}

// 计算特定行星的推运
void TransitCalculator::calculateSpecificTransits(const std::vector<PlanetData>& natalPlanets,
                                               int transitPlanet,
                                               double startTime,
                                               double endTime,
                                               double stepDays,
                                               const AspectSettings& settings,
                                               std::vector<TransitEvent>& transitEvents) {
    transitEvents.clear();
    
    for (double jd = startTime; jd <= endTime; jd += stepDays) {
        // 计算推运行星位置
        PlanetData transitPlanetData;
        PlanetCalculator::calculatePlanetPosition(jd, transitPlanet, &transitPlanetData);
        
        // 检查与所有本命行星的相位
        for (size_t i = 0; i < natalPlanets.size(); i++) {
            double natalLon = natalPlanets[i].longitude;
            
            // 检查所有启用的相位类型
            for (int aspectType = 1; aspectType <= 11; aspectType++) {
                if (!settings.enabled[aspectType]) continue;
                
                double orb;
                if (AspectCalculator::checkAspect(transitPlanetData.longitude, natalLon, aspectType, 
                                                settings.orbLimits[aspectType], &orb)) {
                    TransitEvent event;
                    event.transitingPlanet = transitPlanet;
                    event.natalPlanet = i;
                    event.aspectType = aspectType;
                    event.orb = orb;
                    event.exactTime = jd;
                    event.applying = AspectCalculator::isAspectApplying(
                        transitPlanetData.longitude, natalLon, 
                        transitPlanetData.speed, natalPlanets[i].speed, 
                        aspectType);
                    
                    transitEvents.push_back(event);
                }
            }
        }
    }
}

// 查找重要的推运事件
void TransitCalculator::findSignificantTransits(const std::vector<PlanetData>& natalPlanets,
                                             double startTime,
                                             double endTime,
                                             const AspectSettings& settings,
                                             std::vector<TransitEvent>& transitEvents) {
    transitEvents.clear();
    
    // 对于主要推运行星（外行星），以较慢的步长检查
    // 对于内行星，以较快的步长检查
    const int majorTransits[] = {SE_SATURN, SE_URANUS, SE_NEPTUNE, SE_PLUTO};
    const int minorTransits[] = {SE_SUN, SE_MOON, SE_MERCURY, SE_VENUS, SE_MARS, SE_JUPITER};
    
    // 检查主要推运行星
    for (int planet : majorTransits) {
        std::vector<TransitEvent> planetEvents;
        calculateSpecificTransits(natalPlanets, planet, startTime, endTime, 1.0, settings, planetEvents);
        transitEvents.insert(transitEvents.end(), planetEvents.begin(), planetEvents.end());
    }
    
    // 检查次要推运行星
    for (int planet : minorTransits) {
        std::vector<TransitEvent> planetEvents;
        calculateSpecificTransits(natalPlanets, planet, startTime, endTime, 0.1, settings, planetEvents);
        transitEvents.insert(transitEvents.end(), planetEvents.begin(), planetEvents.end());
    }
    
    // 按时间排序
    std::sort(transitEvents.begin(), transitEvents.end(), 
              [](const TransitEvent& a, const TransitEvent& b) {
                  return a.exactTime < b.exactTime;
              });
}

// 检查推运行星是否进入新星座
bool TransitCalculator::checkSignChange(int planet, double prevLon, double currLon, int& newSign) {
    int prevSign = (int)(prevLon / 30) + 1;
    newSign = (int)(currLon / 30) + 1;
    
    return prevSign != newSign;
}

// 检查推运行星是否进入新宫位
bool TransitCalculator::checkHouseChange(double planetLon, 
                                     const HouseData& natalHouses,
                                     int& newHouse) {
    newHouse = HouseCalculator::getPlanetHouse(planetLon, natalHouses);
    // 这里应该检查是否与前一个宫位不同
    // 简化处理，总是返回当前宫位
    return true;
}

// 计算推运时的行星位置
void TransitCalculator::calculateTransitPlanets(double jd, std::vector<PlanetData>& transitPlanets) {
    const int planetIds[] = {
        SE_SUN, SE_MOON, SE_MERCURY, SE_VENUS, SE_MARS, 
        SE_JUPITER, SE_SATURN, SE_URANUS, SE_NEPTUNE, SE_PLUTO
    };
    
    transitPlanets.clear();
    transitPlanets.resize(10);
    
    for (int i = 0; i < 10; i++) {
        int ret = PlanetCalculator::calculatePlanetPosition(jd, planetIds[i], &transitPlanets[i]);
        if (ret < 0) {
            // 错误处理
        }
    }
}