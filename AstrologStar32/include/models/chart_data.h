#ifndef CHART_DATA_H
#define CHART_DATA_H

#include <string>
#include <cwchar>
#include <vector>
#include "celestial_body.h"
#include "../core/planet.h"
#include "../core/houses.h"
#include "../core/aspects.h"

// 出生数据结构，与astrolog32保持一致
struct BirthData {
    int year;
    int month;
    int day;
    double time;        // 时间格式为小数小时
    double dst;         // 夏令时调整
    double timezone;    // 时区
    double longitude;   // 经度
    double latitude;    // 纬度
    double altitude;    // 海拔
    wchar_t name[256];  // 姓名
    wchar_t location[256]; // 地点
    char country[256];   // 国家
    char city[256];     // 城市
};

// 计算选项设置
struct CalculationSettings {
    int houseSystem;        // 宫位系统
    bool sidereal;          // 是否为恒星制
    bool topocentric;       // 是否为视差修正
    bool heliocentric;      // 是否为日心制
    bool progression;       // 是否为推进
    double progressionFactor; // 推进因子
};

// 星盘数据结构
struct ChartData {
    std::vector<PlanetData> planets;     // 行星数据
    HouseData houses;                    // 宫位数据
    std::vector<Aspect> aspects;         // 相位数据
    double julianDay;                    // 儒略日
};

#endif // CHART_DATA_H