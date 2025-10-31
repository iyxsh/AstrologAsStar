#include "../include/chart.h"
#include "../include/useswe.h"
#include <iostream>
#include <time.h>
#include <string.h>

// 初始化星历表库，使用默认路径
int init_sweph() {
    // 设置星历表文件路径
    const char* ephepath = "../swe/ephemeris";
    
    // 初始化星历表库
    swe_set_ephe_path(ephepath);
    
    return 0;
}

// 初始化星历表库，使用指定路径
int init_sweph_path(const char* ephepath) {
    // 初始化星历表库
    swe_set_ephe_path(ephepath);
    
    return 0;
}

// 计算天体位置
int calc_planet_position(double jd, int planet, double *xx, char *err) {
    int ret;
    
    // 计算天体位置
    ret = swe_calc_ut(jd, planet, SEFLG_SWIEPH, xx, err);
    
    if (ret < 0) {
        // 错误信息已经在err中
        return -1;
    }
    
    return 0;
}

// 获取当前时间的儒略日
double get_current_jd() {
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    // 转换为儒略日
    double jd = swe_julday(
        timeinfo->tm_year + 1900,
        timeinfo->tm_mon + 1,
        timeinfo->tm_mday,
        timeinfo->tm_hour + timeinfo->tm_min/60.0 + timeinfo->tm_sec/3600.0,
        SE_GREG_CAL
    );
    
    return jd;
}

// 从日期时间获取儒略日
double get_jd_from_date(int year, int month, int day, int hour, int minute, int second, bool is_ut) {
    double jd;
    double time = hour + minute/60.0 + second/3600.0;
    
    if (is_ut) {
        jd = swe_julday(year, month, day, time, SE_GREG_CAL);
    } else {
        // 如果是本地时间，需要转换为UT
        // 这里简化处理，实际应用中需要考虑时区
        jd = swe_julday(year, month, day, time, SE_GREG_CAL);
    }
    
    return jd;
}