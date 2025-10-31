#ifndef PLANET_H
#define PLANET_H

#include <iostream>
#include <string>
#include <vector>
#include "../swe/inc/swephexp.h"

// 星体常量定义，与astrolog32保持一致
#define PLANET_SUN      SE_SUN
#define PLANET_MOON     SE_MOON
#define PLANET_MERCURY  SE_MERCURY
#define PLANET_VENUS    SE_VENUS
#define PLANET_MARS     SE_MARS
#define PLANET_JUPITER  SE_JUPITER
#define PLANET_SATURN   SE_SATURN
#define PLANET_URANUS   SE_URANUS
#define PLANET_NEPTUNE  SE_NEPTUNE
#define PLANET_PLUTO    SE_PLUTO

// 行星名称数组
extern const wchar_t* planetNames[];
extern const char* planetNamesCh[];
extern const char* planetNamesEn[];

// 行星数据结构
struct PlanetData {
    double longitude;     // 黄经
    double latitude;      // 黄纬
    double speed;         // 速度
    double distance;      // 距离
    double rectAscension; // 赤经
    double declination;   // 赤纬
};

class PlanetCalculator {
public:
    // 计算单个行星位置
    static int calculatePlanetPosition(double jd, int planet, PlanetData* data);
    
    // 计算所有主要行星位置
    static void calculateAllPlanets(double jd, std::vector<PlanetData>& planets);
    
    // 格式化显示行星位置
    static std::wstring formatPlanetPosition(const PlanetData& data, int format = 0);
    
    // 计算上升点、中天等特殊点
    static int calculateHouseCusps(double jd, double lat, double lon, int hsys, double* cusps, double* ascmc);
    
private:
    // 内部辅助函数
    static void initializePlanetsVector(std::vector<PlanetData>& planets);
};

#endif // PLANET_H