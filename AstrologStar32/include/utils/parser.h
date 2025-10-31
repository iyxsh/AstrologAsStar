#ifndef PARSER_H
#define PARSER_H

#include <string>

// 解析模式枚举，与astrolog32保持一致
#define pmMon     1
#define pmDay     2
#define pmYea     3
#define pmTim     4
#define pmDst     5
#define pmZon     6
#define pmLon     7
#define pmLat     8
#define pmObject  9
#define pmAspect  10
#define pmHouseSystem 11
#define pmSign    12
#define pmColor   13
#define pmMonEn   14
#define pmSignEn  15
#define pmObjectEn 16
#define pmWeek    18

class Parser {
public:
    // 解析字符串为浮点数
    static double parseStringToDouble(const char* str, int parseMode);
    
    // 解析宽字符串为整数
    static int parseStringToInteger(const wchar_t* str, int parseMode);
    
    // 判断字符是否为数字
    static bool isNumericChar(char ch);
    
    // 判断字符是否为大写字符
    static bool isCapitalChar(char ch);
    
private:
    // 内部辅助函数
};

#endif // PARSER_H