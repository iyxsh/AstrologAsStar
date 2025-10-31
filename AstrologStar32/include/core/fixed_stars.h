#ifndef FIXED_STARS_H
#define FIXED_STARS_H

#include "planet.h"
#include "aspects.h"
#include <vector>
#include <string>

// 恒星数据结构
struct FixedStar {
    std::wstring name;           // 恒星名称
    std::wstring shortName;      // 简称
    double longitude;            // 黄经
    double latitude;             // 黄纬
    double magnitude;            // 星等（亮度）
    double distance;             // 距离（光年）
    double speed;                // 速度
    int house;                   // 所在宫位
    bool isBenefic;              // 是否为吉星
    bool isMalefic;              // 是否为凶星
};

class FixedStarCalculator {
public:
    // 计算特定恒星的位置
    static int calculateFixedStarPosition(const char* starName, double jd, FixedStar* star);
    
    // 计算多个恒星的位置
    static void calculateMultipleFixedStars(const std::vector<std::string>& starNames, 
                                          double jd, 
                                          std::vector<FixedStar>& stars);
    
    // 计算主要恒星的位置
    static void calculateMajorFixedStars(double jd, std::vector<FixedStar>& stars);
    
    // 获取恒星列表
    static void getFixedStarList(std::vector<std::wstring>& starNames);
    
    // 格式化恒星显示
    static std::wstring formatFixedStar(const FixedStar& star, int format = 0);
    
    // 检查恒星与行星的相位
    static bool checkAspectWithPlanet(const FixedStar& star, 
                                    const PlanetData& planet, 
                                    int aspectType,
                                    double orbLimit, 
                                    double* orb);
    
    // 计算恒星与行星的相位
    static void calculateAspectsWithPlanets(const FixedStar& star,
                                          const std::vector<PlanetData>& planets,
                                          const double* orbLimits,
                                          std::vector<Aspect>& aspects);

private:
    // 内部辅助函数
    static void initializeFixedStars(std::vector<FixedStar>& stars);
    static void addFixedStar(std::vector<FixedStar>& stars, 
                           const wchar_t* name, 
                           const wchar_t* shortName,
                           double longitude,
                           double latitude,
                           double magnitude);
};

// 常用恒星索引
#define FIXED_STAR_ALGENIB       0   // 壁宿一
#define FIXED_STAR_ALCYONE       1   // 昴宿六
#define FIXED_STAR_ALDEBARAN     2   // 毕宿五
#define FIXED_STAR_ALGENIB2      3   // 壁宿一(重复)
#define FIXED_STAR_ALGOL         4   // 大陵五
#define FIXED_STAR_ALHENA        5   // 参宿三
#define FIXED_STAR_ALNITAK       6   // 参宿一
#define FIXED_STAR_ALNILAM       7   // 参宿二
#define FIXED_STAR_MINTAKA       8   // 参宿三
#define FIXED_STAR_ATHENAR       9   // 河鼓二
#define FIXED_STAR_BETELGEUSE    10  // 参宿四
#define FIXED_STAR_CANOPUS       11  // 老人星
#define FIXED_STAR_CASTOR        12  // 北河二
#define FIXED_STAR_POLLUX        13  // 北河三
#define FIXED_STAR_PROCYON       14  // 南河三
#define FIXED_STAR_REGULUS       15  // 轩辕十四
#define FIXED_STAR_DENEB        16   // 天津四
#define FIXED_STAR_FOMALHAUT    17   // 北落师门
#define FIXED_STAR_RIGEL        18   // 参宿七
#define FIXED_STAR_SIRIUS       19   // 天狼星
#define FIXED_STAR_SPICA        20   // 角宿一
#define FIXED_STAR_VEGA         21   // 织女一

#endif // FIXED_STARS_H