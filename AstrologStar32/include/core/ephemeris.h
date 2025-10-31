#ifndef EPHEMERIS_H
#define EPHEMERIS_H

#include <string>
#include <sstream>  // 添加缺失的头文件包含
#include "../models/chart_data.h"  // 添加BirthData的包含

class EphemerisCalculator {
public:
    // 初始化星历表库
    static void initializeEphemeris();
    
    // 获取当前儒略日
    static double getCurrentJulianDay();
    
    // 计算儒略日
    static double calculateJulianDay(int year, int month, int day, double hour, bool gregorian = true);
    
    // 从儒略日转换为日期时间
    static void julianDayToDateTime(double jd, int& year, int& month, int& day, double& hour, bool gregorian = true);
    
    // 设置星历表路径
    static void setEphemerisPath(const std::string& path);
    
    // 设置计算标志
    static int getCalculationFlags(bool useSwissEphemeris = true, bool includeSpeed = true, 
                                  bool topocentric = false, bool heliocentric = false);
};

#endif // EPHEMERIS_H