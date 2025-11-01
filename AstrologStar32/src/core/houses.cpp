#include "../../include/core/houses.h"
#include "../../include/data/signs.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cwchar>

// 计算宫位
int HouseCalculator::calculateHouses(double jd, double lat, double lon, int houseSystem, HouseData* houses) {
    char hsys = convertHouseSystem(houseSystem);
    double cusps[13];
    double ascmc[10];
    
    // 初始化数组
    for (int i = 0; i < 13; i++) {
        cusps[i] = 0.0;
    }
    for (int i = 0; i < 10; i++) {
        ascmc[i] = 0.0;
    }
    
    // 计算宫位
    int ret = swe_houses(jd, lat, lon, hsys, cusps, ascmc);
    if (ret >= 0) {  // 修复条件判断，0表示成功计算了12个宫位
        // 复制宫位点数据
        for (int i = 0; i < 13; i++) {
            houses->cusps[i] = cusps[i];
        }
        
        // 复制特殊点数据
        houses->ascendant = ascmc[0];
        houses->mc = ascmc[1];
        houses->armc = ascmc[2];
        houses->vertex = ascmc[3];
        houses->equatorialAsc = ascmc[4];
        houses->coAscendant1 = ascmc[5];
        houses->coAscendant2 = ascmc[6];
        houses->polarAsc = ascmc[7];
        
        return 0; // 成功
    }
    
    std::cerr << "Error calculating houses, return code: " << ret << std::endl;
    return ret;
}

// 获取行星所在的宫位
int HouseCalculator::getPlanetHouse(double planetLongitude, const HouseData& houses) {
    // 确保行星经度在0-360范围内
    planetLongitude = fmod(planetLongitude, 360.0);
    if (planetLongitude < 0) planetLongitude += 360.0;
    
    // 查找行星所在的宫位
    for (int i = 1; i <= 12; i++) {
        double nextCusp = (i == 12) ? houses.cusps[1] : houses.cusps[i+1];
        double currentCusp = houses.cusps[i];
        
        // 处理宫位跨越360度的情况
        if (currentCusp > nextCusp) {
            // 宫位跨越360度
            if ((planetLongitude >= currentCusp && planetLongitude < 360.0) ||
                (planetLongitude >= 0.0 && planetLongitude < nextCusp)) {
                return i;
            }
        } else {
            // 普通情况
            if (planetLongitude >= currentCusp && planetLongitude < nextCusp) {
                return i;
            }
        }
    }
    
    // 默认返回第1宫
    return 1;
}

// 转换宫位系统
char HouseCalculator::convertHouseSystem(int system) {
    switch (system) {
        case 0: return HOUSE_SYSTEM_PLACIDUS;      // 'P'
        case 1: return HOUSE_SYSTEM_KOCH;          // 'K'
        case 2: return HOUSE_SYSTEM_CAMPANUS;      // 'C'
        case 3: return HOUSE_SYSTEM_EQUAL;         // 'E'
        case 4: return HOUSE_SYSTEM_VEHLOW;        // 'V'
        case 5: return HOUSE_SYSTEM_MERIDIAN;      // 'X'
        case 6: return HOUSE_SYSTEM_AZIMUTHAL;     // 'H'
        case 7: return HOUSE_SYSTEM_POLICH;        // 'T'
        case 8: return HOUSE_SYSTEM_ALCABITIUS;    // 'B'
        case 9: return HOUSE_SYSTEM_MORINUS;       // 'M'
        case 10: return HOUSE_SYSTEM_PORPHYRY;     // 'O'
        case 11: return HOUSE_SYSTEM_REGIOMONTANUS; // 'R'
        default: return HOUSE_SYSTEM_PLACIDUS;
    }
}

// 格式化宫位显示
std::wstring HouseCalculator::formatHouseCusp(double cuspLongitude, int format) {
    static wchar_t szFormatted[64];
    
    switch (format) {
    case 0:
        // 常规格式化：度数、星座、分
        {
            // 确保经度在0-360度范围内
            double deg = fmod(cuspLongitude, 360.0);
            if (deg < 0) deg += 360.0;
            int sign = (int)(deg / 30.0);  // 修正：确保正确计算星座索引
            int d = (int)(deg - sign * 30);
            int m = (int)((deg - sign * 30 - d) * 60);
            
            // 修正：确保使用正确的星座名称索引（signNames索引从1开始）
            // 将char*转换为wchar_t*
            size_t len = strlen(signNamesEn[sign+1]) + 1;
            wchar_t* wideName = new wchar_t[len];
            mbstowcs(wideName, signNamesEn[sign+1], len);
            swprintf(szFormatted, 64, L"%2d°%ls%02d′", d, wideName, m);
            delete[] wideName;
        }
        break;
        
    case 1:
        // 以小时/分钟格式显示
        {
            double deg = fmod(cuspLongitude + 0.5 / 4.0, 360.0);
            if (deg < 0) deg += 360.0;
            int d = (int)deg / 15;
            int m = (int)((deg - (double)d * 15.0) * 4.0);
            swprintf(szFormatted, 64, L"%2dh%02dm", d, m);
        }
        break;
        
    default:
        // 以小数度数格式显示
        swprintf(szFormatted, 64, L"%.6f", cuspLongitude);
        break;
    }
    
    return std::wstring(szFormatted);
}

// 计算宫位主星
int HouseCalculator::getHouseRuler(int house, const std::vector<PlanetData>& planets) {
    // 每个星座的主星（按传统占星学）
    // 0=白羊(火星), 1=金牛(金星), 2=双子(水星), 3=巨蟹(月亮), 4=狮子(太阳), 5=处女(水星), 
    // 6=天秤(金星), 7=天蝎(火星), 8=射手(木星), 9=摩羯(土星), 10=水瓶(土星), 11=双鱼(木星)
    static const int rulers[] = {
        PLANET_MARS, PLANET_VENUS, PLANET_MERCURY, PLANET_MOON, 
        PLANET_SUN, PLANET_MERCURY, PLANET_VENUS, PLANET_MARS, 
        PLANET_JUPITER, PLANET_SATURN, PLANET_SATURN, PLANET_JUPITER
    };
    
    // 实际实现需要根据宫位对应的星座来确定主星
    // 这里只是示例实现，返回对应行星的索引
    int rulerPlanet = rulers[house % 12];
    
    // 在行星列表中查找主星
    for (size_t i = 0; i < planets.size(); i++) {
        // 这里简化处理，实际应该根据行星ID匹配
        if (i == (size_t)rulerPlanet) {
            return (int)i;
        }
    }
    
    // 默认返回太阳
    return 0;
}

// 计算宫位强弱
int HouseCalculator::getHouseCondition(int house, const HouseData& houses, double lat) {
    // 简化的宫位强弱判断
    // 实际实现会考虑宫位的角度、地平线位置等因素
    switch (house) {
        case 1:  // 第1宫 上升点
        case 4:  // 第4宫 IC
        case 7:  // 第7宫 下降点
        case 10: // 第10宫 MC
            return 3; // 角宫（最强）
        case 2:
        case 5:
        case 8:
        case 11:
            return 2; // 续宫
        case 3:
        case 6:
        case 9:
        case 12:
            return 1; // 果宫
        default:
            return 0;
    }
}