#include "../../include/core/fixed_stars.h"
#include "../../include/data/signs.h"  // 添加缺失的头文件包含
#include "../../swe/inc/swephexp.h"
#include <algorithm>
#include <cmath>

// 常用恒星名称
const wchar_t* fixedStarNames[] = {
    L"壁宿一", L"昴宿六", L"毕宿五", L"壁宿一2", L"大陵五",
    L"参宿三", L"参宿一", L"参宿二", L"参宿三", L"河鼓二",
    L"参宿四", L"老人星", L"北河二", L"北河三", L"南河三",
    L"轩辕十四", L"天津四", L"北落师门", L"参宿七", L"天狼星",
    L"角宿一", L"织女一"
};

// 常用恒星简称
const wchar_t* fixedStarShortNames[] = {
    L"壁一", L"昴六", L"毕五", L"壁一2", L"大陵",
    L"参三", L"参一", L"参二", L"参三", L"河鼓",
    L"参四", L"老人", L"北二", L"北三", L"南三",
    L"轩辕", L"天津", L"北落", L"参七", L"天狼",
    L"角宿", L"织女"
};

// 主要恒星数据（名称、星等、黄经、黄纬）
// 注意：这些是近似值，实际计算需要使用星历表
const char* majorFixedStarNames[] = {
    "Algenib", "Alcyone", "Aldebaran", "Algenib", "Algol",
    "Alhena", "Alnitak", "Alnilam", "Mintaka", "Athenar",
    "Betelgeuse", "Canopus", "Castor", "Pollux", "Procyon",
    "Regulus", "Deneb", "Fomalhaut", "Rigel", "Sirius",
    "Spica", "Vega"
};

const double majorFixedStarMagnitudes[] = {
    2.8, 2.85, 0.85, 2.8, 2.1,
    1.9, 1.7, 1.7, 2.2, 2.0,
    0.5, -0.7, 1.6, 1.1, 0.4,
    1.4, 1.3, 1.2, 0.1, -1.5,
    1.0, 0.0
};

// 计算特定恒星的位置
int FixedStarCalculator::calculateFixedStarPosition(const char* starName, double jd, FixedStar* star) {
    double position[6];
    char err[256];
    
    // 使用Swiss Ephemeris计算恒星位置
    int ret = swe_fixstar(const_cast<char*>(starName), jd, SEFLG_SWIEPH | SEFLG_SPEED, position, err);
    if (ret < 0) {
        // 如果计算失败，返回错误
        return ret;
    }
    
    // 填充恒星数据
    star->longitude = position[0];
    star->latitude = position[1];
    star->distance = position[2];
    star->speed = position[3];
    star->house = 0; // 将在稍后计算
    
    // 设置恒星名称
    // 这里应该根据starName设置对应的中文名称
    star->name = L"恒星";
    star->shortName = L"恒";
    
    // 根据星等设置吉凶属性
    star->magnitude = 0.0; // 默认值
    star->isBenefic = false;
    star->isMalefic = false;
    
    return 0;
}

// 计算多个恒星的位置
void FixedStarCalculator::calculateMultipleFixedStars(const std::vector<std::string>& starNames, 
                                                  double jd, 
                                                  std::vector<FixedStar>& stars) {
    stars.clear();
    
    for (const auto& starName : starNames) {
        FixedStar star;
        int ret = calculateFixedStarPosition(starName.c_str(), jd, &star);
        if (ret >= 0) {
            stars.push_back(star);
        }
    }
}

// 计算主要恒星的位置
void FixedStarCalculator::calculateMajorFixedStars(double jd, std::vector<FixedStar>& stars) {
    stars.clear();
    
    std::vector<std::string> majorStarNames;
    for (int i = 0; i < 22; i++) {
        majorStarNames.push_back(std::string(majorFixedStarNames[i]));
    }
    
    calculateMultipleFixedStars(majorStarNames, jd, stars);
}

// 获取恒星列表
void FixedStarCalculator::getFixedStarList(std::vector<std::wstring>& starNames) {
    starNames.clear();
    for (int i = 0; i < 22; i++) {
        starNames.push_back(std::wstring(fixedStarNames[i]));
    }
}

// 格式化恒星显示
std::wstring FixedStarCalculator::formatFixedStar(const FixedStar& star, int format) {
    static wchar_t szFormatted[128];
    
    switch (format) {
    case 0:
        // 常规格式化：度数、星座、分
        {
            double deg = fmod(star.longitude + 0.5 / 60.0, 360.0);
            if (deg < 0) deg += 360.0;
            int sign = (int)deg / 30;
            int d = (int)deg - sign * 30;
            int m = (int)(fmod(deg, 1.0) * 60.0);
            
            // 将char*转换为wchar_t*
            size_t len = strlen(signNamesEn[sign+1]) + 1;
            wchar_t* wideName = new wchar_t[len];
            mbstowcs(wideName, signNamesEn[sign+1], len);
            swprintf(szFormatted, 128, L"%ls: %2d°%ls%02d′", 
                    star.name.c_str(), d, wideName, m);
            delete[] wideName;
        }
        break;

    case 1:
        // 显示经度和纬度
        {
            double lon = fmod(star.longitude + 0.5 / 60.0, 360.0);
            if (lon < 0) lon += 360.0;
            int signLon = (int)lon / 30;
            int dLon = (int)lon - signLon * 30;
            int mLon = (int)(fmod(lon, 1.0) * 60.0);
            
            double lat = star.latitude;
            bool isNegative = lat < 0;
            lat = fabs(lat);
            lat = fmod(lat + 0.5 / 60.0, 180.0);
            int dLat = (int)lat;
            int mLat = (int)(fmod(lat, 1.0) * 60.0);
            
            // 将char*转换为wchar_t*
            size_t lenLon = strlen(signNamesEn[signLon+1]) + 1;
            wchar_t* wideNameLon = new wchar_t[lenLon];
            mbstowcs(wideNameLon, signNamesEn[signLon+1], lenLon);
            swprintf(szFormatted, 128, L"%ls: %2d°%ls%02d′, %2d°%02d′%ls", 
                    star.name.c_str(), dLon, wideNameLon, mLon,
                    dLat, mLat, isNegative ? L"S" : L"N");
            delete[] wideNameLon;
        }
        break;

    default:
        // 以小数度数格式显示
        swprintf(szFormatted, 128, L"%ls: 经度 %.6f, 纬度 %.6f", 
                star.name.c_str(), star.longitude, star.latitude);
        break;
    }
    
    return std::wstring(szFormatted);
}

// 检查恒星与行星的相位
bool FixedStarCalculator::checkAspectWithPlanet(const FixedStar& star, 
                                            const PlanetData& planet, 
                                            int aspectType,
                                            double orbLimit, 
                                            double* orb) {
    // 使用AspectCalculator中的函数来检查相位
    return AspectCalculator::checkAspect(star.longitude, planet.longitude, aspectType, orbLimit, orb);
}

// 计算恒星与行星的相位
void FixedStarCalculator::calculateAspectsWithPlanets(const FixedStar& star,
                                                  const std::vector<PlanetData>& planets,
                                                  const double* orbLimits,
                                                  std::vector<Aspect>& aspects) {
    // 检查恒星与每个行星的相位
    for (size_t i = 0; i < planets.size(); i++) {
        // 检查所有相位类型
        for (int aspectType = 1; aspectType <= 11; aspectType++) {
            double orb;
            if (checkAspectWithPlanet(star, planets[i], aspectType, orbLimits[aspectType], &orb)) {
                Aspect aspect;
                aspect.planet1 = -1; // 恒星用-1表示
                aspect.planet2 = i;  // 行星索引
                aspect.aspectType = aspectType;
                aspect.orb = orb;
                aspect.applying = false; // 简化处理
                
                aspects.push_back(aspect);
            }
        }
    }
}

// 初始化恒星向量
void FixedStarCalculator::initializeFixedStars(std::vector<FixedStar>& stars) {
    stars.clear();
}

// 添加恒星
void FixedStarCalculator::addFixedStar(std::vector<FixedStar>& stars, 
                                   const wchar_t* name, 
                                   const wchar_t* shortName,
                                   double longitude,
                                   double latitude,
                                   double magnitude) {
    FixedStar star;
    star.name = std::wstring(name);
    star.shortName = std::wstring(shortName);
    star.longitude = longitude;
    star.latitude = latitude;
    star.magnitude = magnitude;
    star.distance = 0.0;
    star.speed = 0.0;
    star.house = 0;
    star.isBenefic = false;
    star.isMalefic = false;
    stars.push_back(star);
}