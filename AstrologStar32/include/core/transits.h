#ifndef TRANSITS_H
#define TRANSITS_H

#include "planet.h"
#include "aspects.h"
#include "../models/chart_data.h"
#include <vector>

// 推运事件结构
struct TransitEvent {
    int transitingPlanet;     // 推运行星
    int natalPlanet;          // 本命行星
    int aspectType;           // 相位类型
    double orb;               // 偏离度数
    double exactTime;         // 精确时间（儒略日）
    bool applying;            // 是否入相
};

class TransitCalculator {
public:
    // 计算行星对本命盘的推运相位
    static void calculateTransitAspects(const std::vector<PlanetData>& natalPlanets,
                                      double transitTime,
                                      const AspectSettings& settings,
                                      std::vector<TransitEvent>& transitEvents);
    
    // 计算特定行星的推运
    static void calculateSpecificTransits(const std::vector<PlanetData>& natalPlanets,
                                        int transitPlanet,
                                        double startTime,
                                        double endTime,
                                        double stepDays,
                                        const AspectSettings& settings,
                                        std::vector<TransitEvent>& transitEvents);
    
    // 查找重要的推运事件
    static void findSignificantTransits(const std::vector<PlanetData>& natalPlanets,
                                      double startTime,
                                      double endTime,
                                      const AspectSettings& settings,
                                      std::vector<TransitEvent>& transitEvents);
    
    // 检查推运行星是否进入新星座
    static bool checkSignChange(int planet, double prevLon, double currLon, int& newSign);
    
    // 检查推运行星是否进入新宫位
    static bool checkHouseChange(double planetLon, 
                               const HouseData& natalHouses,
                               int& newHouse);

private:
    // 内部辅助函数
    static void calculateTransitPlanets(double jd, std::vector<PlanetData>& transitPlanets);
};

#endif // TRANSITS_H