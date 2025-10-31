#include "../../include/core/aspects.h"
#include <iostream>
#include <cmath>
#include <cwchar>

// 默认相位角度
const double defaultAspectAngles[] = {
    0.0,     // 无相位
    0.0,     // 合相
    180.0,   // 冲相
    90.0,    // 四分相
    120.0,   // 三分相
    60.0,    // 六分相
    150.0,   // 梅花相
    45.0,    // 半四分相
    135.0,   // 一度半四分相
    30.0,    // 半六分相
    72.0,    // 五分相
    144.0    // 双五分相
};

// 默认相位容许度
const double defaultOrbLimits[] = {
    0.0,     // 无相位
    8.0,     // 合相
    8.0,     // 冲相
    8.0,     // 四分相
    8.0,     // 三分相
    6.0,     // 六分相
    3.0,     // 梅花相
    3.0,     // 半四分相
    3.0,     // 一度半四分相
    3.0,     // 半六分相
    2.0,     // 五分相
    2.0      // 双五分相
};

// 初始化默认相位设置
void AspectCalculator::initializeDefaultSettings(AspectSettings& settings) {
    for (int i = 0; i < 12; i++) {
        settings.orbLimits[i] = defaultOrbLimits[i];
        settings.enabled[i] = (i > 0 && i <= 5); // 默认启用主要相位
    }
}

// 计算行星间相位
void AspectCalculator::calculateAspects(const std::vector<PlanetData>& planets, 
                                      const AspectSettings& settings,
                                      std::vector<Aspect>& aspects) {
    aspects.clear();
    
    for (size_t i = 0; i < planets.size(); i++) {
        for (size_t j = i + 1; j < planets.size(); j++) {
            double lon1 = planets[i].longitude;
            double lon2 = planets[j].longitude;
            double speed1 = planets[i].speed;
            double speed2 = planets[j].speed;
            
            // 检查所有启用的相位类型
            for (int aspectType = 1; aspectType <= 11; aspectType++) {
                if (!settings.enabled[aspectType]) continue;
                
                double orb;
                if (checkAspect(lon1, lon2, aspectType, settings.orbLimits[aspectType], &orb)) {
                    Aspect aspect;
                    aspect.planet1 = i;
                    aspect.planet2 = j;
                    aspect.aspectType = aspectType;
                    aspect.orb = orb;
                    aspect.applying = isAspectApplying(lon1, lon2, speed1, speed2, aspectType);
                    
                    aspects.push_back(aspect);
                }
            }
        }
    }
}

// 检查两个行星是否形成特定相位
bool AspectCalculator::checkAspect(double lon1, double lon2, int aspectType, 
                                 double orbLimit, double* orb) {
    double aspectAngle = getAspectAngle(aspectType);
    double angle = calculateAspectAngle(lon1, lon2);
    double diff = fabs(angle - aspectAngle);
    
    // 处理360度循环
    if (diff > 180.0) {
        diff = 360.0 - diff;
    }
    
    if (diff <= orbLimit) {
        *orb = diff;
        return true;
    }
    
    return false;
}

// 计算相位角度
double AspectCalculator::calculateAspectAngle(double lon1, double lon2) {
    double diff = fabs(lon1 - lon2);
    if (diff > 180.0) {
        diff = 360.0 - diff;
    }
    return diff;
}

// 获取相位角度
double AspectCalculator::getAspectAngle(int aspectType) {
    if (aspectType >= 0 && aspectType <= 11) {
        return defaultAspectAngles[aspectType];
    }
    return 0.0;
}

// 判断相位是入相还是出相
bool AspectCalculator::isAspectApplying(double lon1, double lon2, double speed1, double speed2, int aspectType) {
    // 计算相对速度
    double relativeSpeed = speed1 - speed2;
    
    // 如果相对速度为0，则无法判断
    if (relativeSpeed == 0) return false;
    
    // 简化判断：如果相对速度为正，则为入相，否则为出相
    return relativeSpeed > 0;
}

// 格式化相位显示
std::wstring AspectCalculator::formatAspect(int aspectType) {
    switch (aspectType) {
        case 0:                     return L"";
        case ASPECT_CONJUNCTION:    return L"合相(0°)";
        case ASPECT_OPPOSITION:     return L"冲相(180°)";
        case ASPECT_SQUARE:         return L"四分相(90°)";
        case ASPECT_TRINE:          return L"三分相(120°)";
        case ASPECT_SEXTILE:        return L"六分相(60°)";
        case ASPECT_QUINCUNX:       return L"梅花相(150°)";
        case ASPECT_SEMISQUARE:     return L"半四分相(45°)";
        case ASPECT_SESQUIQUADRATE: return L"一度半四分相(135°)";
        case ASPECT_SEMISEXTILE:    return L"半六分相(30°)";
        case ASPECT_QUINTILE:       return L"五分相(72°)";
        case ASPECT_BIQUINTILE:     return L"双五分相(144°)";
        default:                    return L"未知相位";
    }
}