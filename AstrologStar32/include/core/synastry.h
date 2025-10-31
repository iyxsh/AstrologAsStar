#ifndef SYNASTRY_H
#define SYNASTRY_H

#include "planet.h"
#include "aspects.h"
#include "houses.h"
#include <vector>

// 合盘比较结果结构
struct SynastryAspect {
    int planet1;              // 第一个人的行星
    int planet2;              // 第二个人的行星
    int aspectType;           // 相位类型
    double orb;               // 偏离度数
    bool applying;            // 是否入相
    int importance;           // 重要性等级 (1-5, 5最重要)
    std::wstring description; // 相位描述
};

// 合盘分析结果
struct SynastryReport {
    std::vector<SynastryAspect> aspects;  // 所有相位
    std::vector<int> elementHarmony;      // 四元素和谐度 (火、土、风、水)
    std::vector<int> modeHarmony;         // 三模式和谐度 (主动、固定、变动)
    double overallCompatibility;          // 整体兼容性评分 (0-100)
};

class SynastryAnalyzer {
public:
    // 分析两个人的星盘合成
    static void analyzeSynastry(const std::vector<PlanetData>& person1Planets,
                              const std::vector<PlanetData>& person2Planets,
                              const HouseData& person1Houses,
                              const HouseData& person2Houses,
                              SynastryReport& report);
    
    // 计算合盘相位
    static void calculateSynastryAspects(const std::vector<PlanetData>& person1Planets,
                                       const std::vector<PlanetData>& person2Planets,
                                       std::vector<SynastryAspect>& aspects);
    
    // 分析宫位重叠
    static void analyzeHouseOverlaps(const HouseData& person1Houses,
                                   const HouseData& person2Houses,
                                   std::vector<SynastryAspect>& houseAspects);
    
    // 评估元素和谐度
    static void evaluateElementHarmony(const std::vector<PlanetData>& person1Planets,
                                     const std::vector<PlanetData>& person2Planets,
                                     std::vector<int>& elementHarmony);
    
    // 评估模式和谐度
    static void evaluateModeHarmony(const std::vector<PlanetData>& person1Planets,
                                  const std::vector<PlanetData>& person2Planets,
                                  std::vector<int>& modeHarmony);
    
    // 计算整体兼容性评分
    static double calculateOverallCompatibility(const std::vector<SynastryAspect>& aspects,
                                              const std::vector<int>& elementHarmony,
                                              const std::vector<int>& modeHarmony);
    
    // 格式化合盘相位显示
    static std::wstring formatSynastryAspect(const SynastryAspect& aspect,
                                           const std::vector<std::wstring>& person1PlanetNames,
                                           const std::vector<std::wstring>& person2PlanetNames);

private:
    // 内部辅助函数
    static int getPlanetElement(int planet);  // 获取行星对应元素 (0=火, 1=土, 2=风, 3=水)
    static int getPlanetMode(int planet);     // 获取行星对应模式 (0=主动, 1=固定, 2=变动)
    static int getAspectImportance(int aspectType); // 获取相位重要性等级
    static std::wstring getAspectDescription(int planet1, int planet2, int aspectType); // 获取相位描述
};

// 元素定义
#define ELEMENT_FIRE    0
#define ELEMENT_EARTH   1
#define ELEMENT_AIR     2
#define ELEMENT_WATER   3

// 模式定义
#define MODE_CARDINAL   0
#define MODE_FIXED      1
#define MODE_MUTABLE    2

// 重要性等级
#define IMPORTANCE_MINOR    1
#define IMPORTANCE_MODERATE 2
#define IMPORTANCE_SIGNIFICANT 3
#define IMPORTANCE_MAJOR    4
#define IMPORTANCE_CRITICAL 5

#endif // SYNASTRY_H