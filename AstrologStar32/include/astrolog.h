#ifndef ASTROLOG_H
#define ASTROLOG_H

// 主要包含文件，整合所有核心模块
#include "core/planet.h"
#include "core/houses.h"
#include "core/aspects.h"
#include "core/ephemeris.h"
#include "utils/parser.h"
#include "utils/formatter.h"
#include "models/chart_data.h"
#include "models/celestial_body.h"

// 主要的占星计算类
class Astrolog {
public:
    // 初始化系统
    static int initialize(const char* ephePath = nullptr);
    
    // 计算完整星盘
    static int calculateChart(const BirthData& birthTime, 
                             double latitude, 
                             double longitude,
                             int houseSystem,
                             ChartData& chartData);
    
    // 计算天体位置
    static int calculateCelestialPositions(double jd, 
                                         std::vector<PlanetData>& planets);
    
    // 计算宫位
    static int calculateHouseCusps(double jd, 
                                  double lat, 
                                  double lon, 
                                  int houseSystem,
                                  HouseData& houses);
    
    // 计算相位
    static int calculateAspects(const std::vector<PlanetData>& planets,
                               std::vector<Aspect>& aspects);
    
    // 设置相位配置
    static void setAspectSettings(const AspectSettings& settings);
    
    // 获取默认相位配置
    static void getDefaultAspectSettings(AspectSettings& settings);

private:
    static AspectSettings currentAspectSettings;
};

#endif // ASTROLOG_H