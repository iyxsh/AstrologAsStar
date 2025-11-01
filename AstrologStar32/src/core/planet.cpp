#include "../../include/core/planet.h"
#include "../../include/data/signs.h"

// 行星中文名称数组
const wchar_t* planetNames[] = {
    L"太阳", L"月亮", L"水星", L"金星", L"火星", 
    L"木星", L"土星", L"天王星", L"海王星", L"冥王星"
};

// 行星中文名称数组(char版本)
const char* planetNamesCh[] = {
    "太阳", "月亮", "水星", "金星", "火星", 
    "木星", "土星", "天王星", "海王星", "冥王星"
};

// 行星英文名称数组
const char* planetNamesEn[] = {
    "Sun", "Moon", "Mercury", "Venus", "Mars", 
    "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto"
};
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cwchar>

// 计算单个行星位置
int PlanetCalculator::calculatePlanetPosition(double jd, int planet, PlanetData* data) {
    double position[6];
    char err[256];
    
    // 计算行星位置，使用与老项目一致的标志确保计算精度
    int flags = SEFLG_SWIEPH | SEFLG_SPEED;
    int ret = swe_calc_ut(jd, planet, flags, position, err);
    if (ret < 0) {
        std::cerr << "Error calculating planet " << planet << " position: " << err << std::endl;
        return ret;
    }
    
    data->longitude = position[0];
    data->latitude = position[1];
    data->distance = position[2];
    data->speed = position[3];
    data->rectAscension = position[4];
    data->declination = position[5];
    
    return 0;
}

// 初始化行星向量
void PlanetCalculator::initializePlanetsVector(std::vector<PlanetData>& planets) {
    planets.clear();
    planets.resize(10); // 10个主要行星
    
    // 初始化所有行星数据为0
    for (int i = 0; i < 10; i++) {
        planets[i].longitude = 0.0;
        planets[i].latitude = 0.0;
        planets[i].distance = 0.0;
        planets[i].speed = 0.0;
        planets[i].rectAscension = 0.0;
        planets[i].declination = 0.0;
    }
}

// 计算所有主要行星位置
void PlanetCalculator::calculateAllPlanets(double jd, std::vector<PlanetData>& planets) {
    const int planetIds[] = {
        SE_SUN, SE_MOON, SE_MERCURY, SE_VENUS, SE_MARS, 
        SE_JUPITER, SE_SATURN, SE_URANUS, SE_NEPTUNE, SE_PLUTO
    };
    
    initializePlanetsVector(planets);
    
    for (int i = 0; i < 10; i++) {
        int ret = calculatePlanetPosition(jd, planetIds[i], &planets[i]);
        if (ret < 0) {
            std::wcerr << L"Failed to calculate position for planet " << i << std::endl;
        }
    }
}

// 计算宫位点
int PlanetCalculator::calculateHouseCusps(double jd, double lat, double lon, int hsys, double* cusps, double* ascmc) {
    char system = (char)hsys;
    return swe_houses(jd, lat, lon, system, cusps, ascmc);
}

// 格式化显示行星位置
std::wstring PlanetCalculator::formatPlanetPosition(const PlanetData& data, int format) {
    static wchar_t szFormatted[64];
    
    switch (format) {
    case 0:
        // 常规格式化：度数、星座、分
        {
            // 确保经度在0-360度范围内
            double deg = fmod(data.longitude, 360.0);
            if (deg < 0) deg += 360.0;
            int sign = (int)(deg / 30.0);
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
            double deg = fmod(data.longitude + 0.5 / 4.0, 360.0);
            if (deg < 0) deg += 360.0;
            int d = (int)deg / 15;
            int m = (int)((deg - (double)d * 15.0) * 4.0);
            swprintf(szFormatted, 64, L"%2dh%02dm", d, m);
        }
        break;
        
    default:
        // 以小数度数格式显示
        swprintf(szFormatted, 64, L"%.6f", data.longitude);
        break;
    }
    
    return std::wstring(szFormatted);
}