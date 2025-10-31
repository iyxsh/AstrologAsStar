#include "../../include/core/ephemeris.h"
#include "../../swe/inc/swephexp.h"
#include <iostream>
#include <ctime>

// 初始化星历表库
void EphemerisCalculator::initializeEphemeris() {
    // 设置星历表文件路径
    swe_set_ephe_path("../swe/ephemeris");
}

// 获取当前儒略日
double EphemerisCalculator::getCurrentJulianDay() {
    // 获取当前时间
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    
    // 转换为儒略日
    int year = now->tm_year + 1900;
    int month = now->tm_mon + 1;
    int day = now->tm_mday;
    double hour = now->tm_hour + now->tm_min / 60.0 + now->tm_sec / 3600.0;
    
    return calculateJulianDay(year, month, day, hour);
}

// 计算儒略日
double EphemerisCalculator::calculateJulianDay(int year, int month, int day, double hour, bool gregorian) {
    int calendar_type = gregorian ? SE_GREG_CAL : SE_JUL_CAL;
    double jd = swe_julday(year, month, day, hour, calendar_type);
    return jd;
}

// 从儒略日转换为日期时间
void EphemerisCalculator::julianDayToDateTime(double jd, int& year, int& month, int& day, double& hour, bool gregorian) {
    int calendar_type = gregorian ? SE_GREG_CAL : SE_JUL_CAL;
    swe_revjul(jd, calendar_type, &year, &month, &day, &hour);
}

// 设置星历表路径
void EphemerisCalculator::setEphemerisPath(const std::string& path) {
    swe_set_ephe_path(path.c_str());
}

// 设置计算标志
int EphemerisCalculator::getCalculationFlags(bool useSwissEphemeris, bool includeSpeed, 
                                           bool topocentric, bool heliocentric) {
    int flags = 0;
    
    // 设置星历表类型
    if (useSwissEphemeris) {
        flags |= SEFLG_SWIEPH;
    } else {
        flags |= SEFLG_JPLEPH;
    }
    
    // 是否包含速度
    if (includeSpeed) {
        flags |= SEFLG_SPEED;
    }
    
    // 是否为视差修正
    if (topocentric) {
        flags |= SEFLG_TOPOCTR;
    }
    
    // 是否为日心制
    if (heliocentric) {
        flags |= SEFLG_HELCTR;
    }
    
    return flags;
}

