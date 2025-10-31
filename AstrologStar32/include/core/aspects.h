#ifndef ASPECTS_H
#define ASPECTS_H

#include <vector>
#include <iostream>
#include "planet.h"

// 相位常量定义，与astrolog32保持一致
#define ASPECT_CONJUNCTION    1   // 0度合相
#define ASPECT_OPPOSITION     2   // 180度冲相
#define ASPECT_SQUARE         3   // 90度四分相
#define ASPECT_TRINE          4   // 120度三分相
#define ASPECT_SEXTILE        5   // 60度六分相
#define ASPECT_QUINCUNX       6   // 150度梅花相
#define ASPECT_SEMISQUARE     7   // 45度半四分相
#define ASPECT_SESQUIQUADRATE 8   // 135度一度半四分相
#define ASPECT_SEMISEXTILE    9   // 30度半六分相
#define ASPECT_QUINTILE      10   // 72度五分相
#define ASPECT_BIQUINTILE   11    // 144度双五分相

// 相位数据结构
struct Aspect {
    int planet1;              // 行星1索引
    int planet2;              // 行星2索引
    int aspectType;           // 相位类型
    double orb;               // 偏离度数
    bool applying;            // 是否是入相
};

// 相位设置结构
struct AspectSettings {
    double orbLimits[12];     // 各相位的容许度
    bool enabled[12];         // 各相位是否启用
};

class AspectCalculator {
public:
    // 初始化默认相位设置
    static void initializeDefaultSettings(AspectSettings& settings);
    
    // 计算行星间相位
    static void calculateAspects(const std::vector<PlanetData>& planets, 
                                const AspectSettings& settings,
                                std::vector<Aspect>& aspects);
    
    // 检查两个行星是否形成特定相位
    static bool checkAspect(double lon1, double lon2, int aspectType, 
                           double orbLimit, double* orb);
    
    // 计算相位角度
    static double calculateAspectAngle(double lon1, double lon2);
    
    // 格式化相位显示
    static std::wstring formatAspect(int aspectType);
    
    // 判断相位是入相还是出相
    static bool isAspectApplying(double lon1, double lon2, double speed1, double speed2, int aspectType);
    
private:
    // 内部辅助函数
    static double getAspectAngle(int aspectType);
};

#endif // ASPECTS_H