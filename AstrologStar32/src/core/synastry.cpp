#include "../../include/core/synastry.h"
#include <algorithm>
#include <cmath>
#include <cwchar>

// 行星对应元素 (0=火, 1=土, 2=风, 3=水)
const int planetElements[] = {
    ELEMENT_FIRE,   // 太阳
    ELEMENT_WATER,  // 月亮
    ELEMENT_EARTH,  // 水星
    ELEMENT_WATER,  // 金星
    ELEMENT_FIRE,   // 火星
    ELEMENT_AIR,    // 木星
    ELEMENT_EARTH,  // 土星
    ELEMENT_AIR,    // 天王星
    ELEMENT_WATER,  // 海王星
    ELEMENT_WATER   // 冥王星
};

// 行星对应模式 (0=主动, 1=固定, 2=变动)
const int planetModes[] = {
    MODE_FIXED,     // 太阳
    MODE_CARDINAL,  // 月亮
    MODE_MUTABLE,   // 水星
    MODE_FIXED,     // 金星
    MODE_CARDINAL,  // 火星
    MODE_MUTABLE,   // 木星
    MODE_CARDINAL,  // 土星
    MODE_FIXED,     // 天王星
    MODE_CARDINAL,  // 海王星
    MODE_MUTABLE    // 冥王星
};

// 分析两个人的星盘合成
void SynastryAnalyzer::analyzeSynastry(const std::vector<PlanetData>& person1Planets,
                                    const std::vector<PlanetData>& person2Planets,
                                    const HouseData& person1Houses,
                                    const HouseData& person2Houses,
                                    SynastryReport& report) {
    // 计算合盘相位
    calculateSynastryAspects(person1Planets, person2Planets, report.aspects);
    
    // 分析宫位重叠
    std::vector<SynastryAspect> houseAspects;
    analyzeHouseOverlaps(person1Houses, person2Houses, houseAspects);
    report.aspects.insert(report.aspects.end(), houseAspects.begin(), houseAspects.end());
    
    // 评估元素和谐度
    evaluateElementHarmony(person1Planets, person2Planets, report.elementHarmony);
    
    // 评估模式和谐度
    evaluateModeHarmony(person1Planets, person2Planets, report.modeHarmony);
    
    // 计算整体兼容性评分
    report.overallCompatibility = calculateOverallCompatibility(report.aspects, 
                                                             report.elementHarmony, 
                                                             report.modeHarmony);
}

// 计算合盘相位
void SynastryAnalyzer::calculateSynastryAspects(const std::vector<PlanetData>& person1Planets,
                                             const std::vector<PlanetData>& person2Planets,
                                             std::vector<SynastryAspect>& aspects) {
    aspects.clear();
    
    // 检查每个人的所有行星与其他人的所有行星的相位
    for (size_t i = 0; i < person1Planets.size() && i < 10; i++) {
        for (size_t j = 0; j < person2Planets.size() && j < 10; j++) {
            double lon1 = person1Planets[i].longitude;
            double lon2 = person2Planets[j].longitude;
            
            // 检查主要相位类型
            for (int aspectType = 1; aspectType <= 5; aspectType++) { // 只检查主要相位
                double orbLimit;
                switch (aspectType) {
                    case ASPECT_CONJUNCTION:
                    case ASPECT_OPPOSITION:
                    case ASPECT_SQUARE:
                    case ASPECT_TRINE:
                        orbLimit = 8.0;
                        break;
                    case ASPECT_SEXTILE:
                        orbLimit = 5.0;
                        break;
                    default:
                        orbLimit = 3.0;
                        break;
                }
                
                double orb;
                if (AspectCalculator::checkAspect(lon1, lon2, aspectType, orbLimit, &orb)) {
                    SynastryAspect aspect;
                    aspect.planet1 = i;
                    aspect.planet2 = j;
                    aspect.aspectType = aspectType;
                    aspect.orb = orb;
                    aspect.applying = AspectCalculator::isAspectApplying(lon1, lon2, 
                                                                       person1Planets[i].speed, 
                                                                       person2Planets[j].speed, 
                                                                       aspectType);
                    aspect.importance = getAspectImportance(aspectType);
                    aspect.description = getAspectDescription(i, j, aspectType);
                    
                    aspects.push_back(aspect);
                }
            }
        }
    }
    
    // 按重要性排序
    std::sort(aspects.begin(), aspects.end(), 
              [](const SynastryAspect& a, const SynastryAspect& b) {
                  return a.importance > b.importance;
              });
}

// 分析宫位重叠
void SynastryAnalyzer::analyzeHouseOverlaps(const HouseData& person1Houses,
                                         const HouseData& person2Houses,
                                         std::vector<SynastryAspect>& houseAspects) {
    houseAspects.clear();
    
    // 比较宫位主星
    for (int i = 1; i <= 12; i++) {
        double cusp1 = person1Houses.cusps[i];
        double cusp2 = person2Houses.cusps[i];
        
        double diff = fabs(cusp1 - cusp2);
        if (diff > 180.0) diff = 360.0 - diff;
        
        // 如果宫位相差小于5度，认为有重叠
        if (diff < 5.0) {
            SynastryAspect aspect;
            aspect.planet1 = -1; // 用-1表示宫位
            aspect.planet2 = -1; // 用-1表示宫位
            aspect.aspectType = ASPECT_CONJUNCTION;
            aspect.orb = diff;
            aspect.applying = false;
            aspect.importance = IMPORTANCE_SIGNIFICANT;
            aspect.description = L"宫位重叠";
            
            houseAspects.push_back(aspect);
        }
    }
}

// 评估元素和谐度
void SynastryAnalyzer::evaluateElementHarmony(const std::vector<PlanetData>& person1Planets,
                                           const std::vector<PlanetData>& person2Planets,
                                           std::vector<int>& elementHarmony) {
    elementHarmony.clear();
    elementHarmony.resize(4, 0); // 火、土、风、水
    
    // 统计每个人的行星元素分布
    int person1Elements[4] = {0};
    int person2Elements[4] = {0};
    
    for (size_t i = 0; i < person1Planets.size() && i < 10; i++) {
        int element = getPlanetElement(i);
        person1Elements[element]++;
    }
    
    for (size_t i = 0; i < person2Planets.size() && i < 10; i++) {
        int element = getPlanetElement(i);
        person2Elements[element]++;
    }
    
    // 计算元素和谐度
    for (int i = 0; i < 4; i++) {
        // 简化计算：基于元素分布的互补性
        int diff = abs(person1Elements[i] - person2Elements[(i + 2) % 4]); // 对立元素
        elementHarmony[i] = 5 - diff; // 差距越小，和谐度越高
        if (elementHarmony[i] < 0) elementHarmony[i] = 0;
    }
}

// 评估模式和谐度
void SynastryAnalyzer::evaluateModeHarmony(const std::vector<PlanetData>& person1Planets,
                                        const std::vector<PlanetData>& person2Planets,
                                        std::vector<int>& modeHarmony) {
    modeHarmony.clear();
    modeHarmony.resize(3, 0); // 主动、固定、变动
    
    // 统计每个人的行星模式分布
    int person1Modes[3] = {0};
    int person2Modes[3] = {0};
    
    for (size_t i = 0; i < person1Planets.size() && i < 10; i++) {
        int mode = getPlanetMode(i);
        person1Modes[mode]++;
    }
    
    for (size_t i = 0; i < person2Planets.size() && i < 10; i++) {
        int mode = getPlanetMode(i);
        person2Modes[mode]++;
    }
    
    // 计算模式和谐度
    for (int i = 0; i < 3; i++) {
        // 简化计算：基于模式分布的相似性
        int diff = abs(person1Modes[i] - person2Modes[i]);
        modeHarmony[i] = 5 - diff; // 差距越小，和谐度越高
        if (modeHarmony[i] < 0) modeHarmony[i] = 0;
    }
}

// 计算整体兼容性评分
double SynastryAnalyzer::calculateOverallCompatibility(const std::vector<SynastryAspect>& aspects,
                                                    const std::vector<int>& elementHarmony,
                                                    const std::vector<int>& modeHarmony) {
    double score = 50.0; // 基础分50
    
    // 根据重要相位调整分数
    for (const auto& aspect : aspects) {
        switch (aspect.importance) {
            case IMPORTANCE_CRITICAL:
                score += (aspect.aspectType == ASPECT_CONJUNCTION || 
                         aspect.aspectType == ASPECT_TRINE || 
                         aspect.aspectType == ASPECT_SEXTILE) ? 5.0 : -5.0;
                break;
            case IMPORTANCE_MAJOR:
                score += (aspect.aspectType == ASPECT_CONJUNCTION || 
                         aspect.aspectType == ASPECT_TRINE || 
                         aspect.aspectType == ASPECT_SEXTILE) ? 3.0 : -3.0;
                break;
            case IMPORTANCE_SIGNIFICANT:
                score += (aspect.aspectType == ASPECT_CONJUNCTION || 
                         aspect.aspectType == ASPECT_TRINE || 
                         aspect.aspectType == ASPECT_SEXTILE) ? 2.0 : -2.0;
                break;
            case IMPORTANCE_MODERATE:
                score += (aspect.aspectType == ASPECT_CONJUNCTION || 
                         aspect.aspectType == ASPECT_TRINE || 
                         aspect.aspectType == ASPECT_SEXTILE) ? 1.0 : -1.0;
                break;
        }
    }
    
    // 根据元素和谐度调整分数
    for (int harmony : elementHarmony) {
        score += harmony * 1.5;
    }
    
    // 根据模式和谐度调整分数
    for (int harmony : modeHarmony) {
        score += harmony * 1.0;
    }
    
    // 限制分数在0-100之间
    if (score < 0) score = 0;
    if (score > 100) score = 100;
    
    return score;
}

// 格式化合盘相位显示
std::wstring SynastryAnalyzer::formatSynastryAspect(const SynastryAspect& aspect,
                                                 const std::vector<std::wstring>& person1PlanetNames,
                                                 const std::vector<std::wstring>& person2PlanetNames) {
    static wchar_t szFormatted[256];
    
    const wchar_t* planet1Name = (aspect.planet1 >= 0) ? 
                                person1PlanetNames[aspect.planet1].c_str() : L"宫位";
    const wchar_t* planet2Name = (aspect.planet2 >= 0) ? 
                                person2PlanetNames[aspect.planet2].c_str() : L"宫位";
    
    swprintf(szFormatted, 256, L"%ls 与 %ls 形成 %ls，偏离 %.2f度%s",
            planet1Name, planet2Name,
            AspectCalculator::formatAspect(aspect.aspectType).c_str(),
            aspect.orb,
            aspect.applying ? L" (入相)" : L"");
    
    return std::wstring(szFormatted);
}

// 获取行星对应元素
int SynastryAnalyzer::getPlanetElement(int planet) {
    if (planet >= 0 && planet < 10) {
        return planetElements[planet];
    }
    return ELEMENT_FIRE; // 默认
}

// 获取行星对应模式
int SynastryAnalyzer::getPlanetMode(int planet) {
    if (planet >= 0 && planet < 10) {
        return planetModes[planet];
    }
    return MODE_CARDINAL; // 默认
}

// 获取相位重要性等级
int SynastryAnalyzer::getAspectImportance(int aspectType) {
    switch (aspectType) {
        case ASPECT_CONJUNCTION:
        case ASPECT_OPPOSITION:
        case ASPECT_SQUARE:
        case ASPECT_TRINE:
            return IMPORTANCE_MAJOR;
        case ASPECT_SEXTILE:
            return IMPORTANCE_SIGNIFICANT;
        default:
            return IMPORTANCE_MODERATE;
    }
}

// 获取相位描述
std::wstring SynastryAnalyzer::getAspectDescription(int planet1, int planet2, int aspectType) {
    // 简化实现，实际应用中可以提供更详细的解释
    return L"合盘相位";
}