#ifndef USESWE_H
#define USESWE_H

// 根据平台包含swiss ephemeris头文件，与原项目astrolog32保持一致
#if defined(_WIN32)
    #include "windows.h"
    #include "../swe/inc/swedll.h"
    #define EXP32_DLL __declspec(dllimport)
#else
    #include "../swe/inc/swephexp.h"
    #include "../swe/inc/sweph.h"
    #include "../swe/inc/swephlib.h"
    #include "../swe/inc/swehouse.h"
    #include "../swe/inc/sweodef.h"
#endif

// 函数声明
int init_sweph();
int init_sweph_path(const char* ephepath);
int calc_planet_position(double jd, int planet, double *xx, char *err);
double get_current_jd();
double get_jd_from_date(int year, int month, int day, int hour, int minute, int second, bool is_ut=false);

#endif // USESWE_H