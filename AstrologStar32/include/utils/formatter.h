#ifndef FORMATTER_H
#define FORMATTER_H

#include <string>
#include "../models/chart_data.h"

class Formatter {
public:
    // 格式化经度显示
    static std::wstring formatLongitude(double deg, int format = 0);
    
    // 格式化纬度显示
    static std::wstring formatLatitude(double deg, int format = 0);
    
    // 格式化时间显示
    static std::wstring formatTime(double t, int format = 0);
    
    // 格式化角度显示
    static std::wstring formatAngle(double angle, int format = 0);
    
    // 格式化出生数据
    static std::wstring formatBirthData(const BirthData& data);
    
    // 解析字符串为数值
    static double parseStringToDouble(const char* str, int parseMode = 0);
    
    // 解析字符串为整数
    static int parseStringToInt(const wchar_t* str, int parseMode = 0);

private:
    // 内部辅助函数
    static bool stringsMatch(const wchar_t* s1, const wchar_t* s2, int n);
};

// 解析模式常量
#define PARSE_MONTH      1
#define PARSE_MONTH_EN   2
#define PARSE_OBJECT     3
#define PARSE_OBJECT_EN  4
#define PARSE_ASPECT     5
#define PARSE_HOUSE_SYSTEM 6
#define PARSE_SIGN       7
#define PARSE_SIGN_EN    8
#define PARSE_COLOR      9
#define PARSE_TIME       10
#define PARSE_DST        11
#define PARSE_ZONE       12
#define PARSE_LONGITUDE  13
#define PARSE_LATITUDE   14
#define PARSE_YEAR       15

#endif // FORMATTER_H