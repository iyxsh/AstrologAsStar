#include "../../include/utils/parser.h"
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// 判断字符是否为数字
bool Parser::isNumericChar(char ch) {
    return (ch >= '0' && ch <= '9');
}

// 判断字符是否为大写字符
bool Parser::isCapitalChar(char ch) {
    return (ch >= 'A' && ch <= 'Z');
}

// 解析字符串为浮点数
double Parser::parseStringToDouble(const char* szEntry, int pm)
{
    if (!szEntry) return 0.0;
    
    char szLocal[512], *sz, *pch, ch;
    int cch, i, f = FALSE;
    double r;

    // 首先去除前导和尾随空格
    for (cch = 0; (szLocal[cch] = szEntry[cch]) != 0; cch++)
        ;
    while (cch && szLocal[cch - 1] == ' ')
        szLocal[--cch] = '\0';
    for (sz = szLocal; *sz && *sz == ' '; sz++, cch--)
        ;

    // 将冒号、撇号、双引号转换为点号，大写所有字母
    for (pch = sz; *pch; pch++)
    {
        ch = *pch;
        if (ch == ':' || ch == '\'' || ch == '\"')
            ch = '.';
        else
            ch = toupper(ch);
        *pch = ch;
    }
    ch = sz[0];

    if (pm == pmTim) // 时间处理
    {
        // 处理"Noon"和"Midnight"（或"N"和"M"）
        if (ch == 'N')
            return 12.0;
        else if (ch == 'M')
            return 0.0;
    }
    else if (pm == pmDst) // 夏令时标志
    {
        // 处理"Daylight", "Yes", "True"（或首字母）表示提前一小时
        if ((strcmp(sz, "Yes") == 0) || (sz[0] == 'Y' && sz[1] == 0) || (strcmp(sz, "DT") == 0)) 
            return 1.0;
        // "Standard", "No", "False"表示正常零偏移
        else if ((strcmp(sz, "No") == 0) || (sz[0] == 'N' && sz[1] == 0) || (strcmp(sz, "ST") == 0)) 
            return 0.0;
        else 
            return atof(sz);
    }
    else if (pm == pmZon)
    {
        // 时区处理
        // 查找预定义的时区缩写
        // 简化处理，实际项目中需要完整实现
    }
    else if (pm == pmLon || pm == pmLat)
    {
        // 对于位置，如果有"E"或"S"则取负值
        for (i = 0; i < cch; i++)
        {
            ch = sz[i];
            if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
            {
                if (ch == 'E' || ch == 'S' || ch == 'e' || ch == 's')
                    f = TRUE;
                sz[i] = '\0'; // 移除字符
                break;
            }
        }
    }

    // 此时应该已经是数字格式
    if (!isNumericChar(ch) && ch != '+' && ch != '-' && ch != '.')
        return 1e30; // 错误值
        
    r = (f ? -1.0 : 1.0) * atof(sz);

    if (pm == pmTim) // 时间后处理
    {
        // 处理AM/PM后缀
        i = std::max(cch - 1, 0);
        if (i && sz[i] == '.')
            i--;
        if (i && (sz[i] == 'M' || sz[i] == 'm'))
            i--;
        if (i && sz[i] == '.')
            i--;
        if (i && (sz[i + 1] == 'M' || sz[i + 1] == 'm'))
        {
            ch = sz[i];
            if (ch == 'A' || ch == 'a') {        // 调整值
                r = r >= 12.0 ? r - 12.0 : r;  // AM后缀
            }
            else if (ch == 'P' || ch == 'p') {
                r = r >= 12.0 ? r : r + 12.0;   // PM后缀
            }
        }
    }
    
    return r;
}

// 解析宽字符串为整数
int Parser::parseStringToInteger(const wchar_t* szEntry, int pm)
{
    if (!szEntry) return 0;
    
    wchar_t szLocal[512], *sz;
    int cch, n, i;
    int returnflag = 0;

    // 首先去除前导和尾随空格
    for (cch = 0; (szLocal[cch] = szEntry[cch]) != 0; cch++)
        ;
    szLocal[cch] = L'\0';
    while (cch && (unsigned char)szLocal[cch - 1] <= ' ')
        szLocal[--cch] = L'\0';
    for (sz = szLocal; *sz && (unsigned char)(*sz) <= ' '; sz++, cch--)
        ;

    // IC名称特殊处理
    if (sz[0] == L'I' && sz[1] == L'C')
    {
        sz[2] = L' ';
        sz[3] = 0;
        cch++;
    }
    
    if (cch >= 3) // 至少需要3个字符
    {
        // 根据解析模式处理
        switch (pm)
        {
            // 月份解析
            case pmMon: // pmMon
                // 简化处理，实际项目中需要完整实现
                break;
                
            // 行星解析
            case pmObject: // pmObject
                // 简化处理，实际项目中需要完整实现
                break;
        }

        if(returnflag) return returnflag;
    }

    if (sz == NULL || sz[0] == L'\0') {
        return 0;
    }

    try {
        n = std::stoi(sz);
    } catch (...) {
        n = 0;
    }

    if (pm == pmYea) // 年份处理
    {
        // 处理"BC"（或"B.C.", "b.c"等）并转换
        // 例如"5BC"转换为-4
        i = std::max(cch - 1, 0);
        if (i && sz[i] == L'.')
            i--;
        if (i && towupper(sz[i]) == L'C')
            i--;
        if (i && sz[i] == L'.')
            i--;
        if (i && towupper(sz[i]) == L'B')
            n = 1 - n;
    }
    return n;
}