#ifndef HOUSES_H
#define HOUSES_H

#include <iostream>
#include <vector>
#include "../swe/inc/swephexp.h"
#include "planet.h"

// 宫位系统常量定义，与astrolog32保持一致
#define HOUSE_SYSTEM_PLACIDUS    'P'
#define HOUSE_SYSTEM_KOCH        'K'
#define HOUSE_SYSTEM_CAMPANUS    'C'
#define HOUSE_SYSTEM_EQUAL       'E'
#define HOUSE_SYSTEM_VEHLOW      'V'
#define HOUSE_SYSTEM_MERIDIAN    'X'
#define HOUSE_SYSTEM_AZIMUTHAL   'H'
#define HOUSE_SYSTEM_POLICH     'T'
#define HOUSE_SYSTEM_ALCABITIUS 'B'
#define HOUSE_SYSTEM_MORINUS    'M'
#define HOUSE_SYSTEM_PORPHYRY   'O'
#define HOUSE_SYSTEM_REGIOMONTANUS 'R'

// 宫位数据结构
struct HouseData {
    double cusps[13];        // 12个宫位点，索引0未使用
    double ascendant;        // 上升点
    double mc;               // 中天
    double armc;             // 日间半弧
    double vertex;           // 顶点
    double equatorialAsc;    // 赤道上升点
    double coAscendant1;     // 协调上升点1
    double coAscendant2;     // 协调上升点2
    double polarAsc;         // 极上升点
};

class HouseCalculator {
public:
    // 计算宫位
    static int calculateHouses(double jd, double lat, double lon, int houseSystem, HouseData* houses);
    
    // 获取行星所在的宫位
    static int getPlanetHouse(double planetLongitude, const HouseData& houses);
    
    // 格式化宫位显示
    static std::wstring formatHouseCusp(double cuspLongitude, int format = 0);
    
    // 计算宫位主星
    static int getHouseRuler(int house, const std::vector<PlanetData>& planets);
    
    // 计算宫位强弱
    static int getHouseCondition(int house, const HouseData& houses, double lat);
    
private:
    // 内部辅助函数
    static char convertHouseSystem(int system);
};

#endif // HOUSES_H