#include "../../include/utils/formatter.h"
#include "../../include/data/signs.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cwchar>
#include <string>
#include <algorithm>

// 格式化黄经度数显示
std::wstring format_longitude_deg(double deg, int format) {
    static wchar_t szZod[64];
    
    switch (format) {
    case 0:
        // 常规格式化：度数、星座、分
        {
            double deg_adj = fmod(deg + 0.5 / 60.0, 360.0);
            if (deg_adj < 0) deg_adj += 360.0;
            int sign = (int)deg_adj / 30;
            int d = (int)deg_adj - sign * 30;
            int m = (int)(fmod(deg_adj, 1.0) * 60.0);
            
            // 将char*转换为wchar_t*
            size_t len = strlen(signNamesEn[sign]) + 1;
            wchar_t* wideName = new wchar_t[len];
            mbstowcs(wideName, signNamesEn[sign], len);
            swprintf(szZod, 64, L"%2d°%ls%02d′", d, wideName, m);
            delete[] wideName;
        }
        break;

    case 1:
        // 以小时/分钟格式显示
        {
            double deg_adj = fmod(deg + 0.5 / 4.0, 360.0);
            if (deg_adj < 0) deg_adj += 360.0;
            int d = (int)deg_adj / 15;
            int m = (int)((deg_adj - (double)d * 15.0) * 4.0);
            swprintf(szZod, 64, L"%2dh%02dm", d, m);
        }
        break;

    default:
        // 以小数度数格式显示
        swprintf(szZod, 64, L"%.6f", deg);
        break;
    }
    
    return std::wstring(szZod);
}

// 格式化经度显示
std::wstring Formatter::formatLongitude(double deg, int format) {
    static wchar_t szZod[64];
    
    switch (format) {
    case 0:
        /*常规格式化：度数、星座、分*/
        deg = fmod(deg + 0.5 / 60.0, 360.0);
        if (deg < 0) deg += 360.0;
        {
            int sign = (int)deg / 30;
            int d = (int)deg - sign * 30;
            int m = (int)(fmod(deg, 1.0) * 60.0);
            
            swprintf(szZod, 64, L"%2d°%ls%02d′", d, signNames[sign], m);
        }
        break;

    case 1:
        /* 以小时/分钟格式显示 */
        deg = fmod(deg + 0.5 / 4.0, 360.0);
        if (deg < 0) deg += 360.0;
        {
            int d = (int)deg / 15;
            int m = (int)((deg - (double)d * 15.0) * 4.0);
            swprintf(szZod, 64, L"%2dh%02dm", d, m);
        }
        break;

    default:
        /* 以小数度数格式显示 */
        swprintf(szZod, 64, L"%.6f°", deg);
        break;
    }
    return std::wstring(szZod);
}

// 格式化纬度显示
std::wstring Formatter::formatLatitude(double deg, int format) {
    static wchar_t szLat[64];
    
    bool isNegative = deg < 0;
    deg = fabs(deg);
    
    switch (format) {
    case 0:
        /*常规格式化：度数、分*/
        deg = fmod(deg + 0.5 / 60.0, 180.0);
        {
            int d = (int)deg;
            int m = (int)(fmod(deg, 1.0) * 60.0);
            
            swprintf(szLat, 64, L"%2d°%02d′%ls", d, m, isNegative ? L"S" : L"N");
        }
        break;

    default:
        /* 以小数度数格式显示 */
        swprintf(szLat, 64, L"%.6f°", deg * (isNegative ? -1 : 1));
        break;
    }
    return std::wstring(szLat);
}

// 格式化时间显示
std::wstring Formatter::formatTime(double t, int format) {
    static wchar_t szTim[64];
    
    switch (format) {
    case 0:
        /*常规格式化：小时:分钟:秒*/
        {
            int h = (int)t;
            int m = (int)((t - h) * 60);
            int s = (int)(((t - h) * 60 - m) * 60);
            
            swprintf(szTim, 64, L"%02d:%02d:%02d", h, m, s);
        }
        break;

    default:
        /* 以小数小时格式显示 */
        swprintf(szTim, 64, L"%.6fh", t);
        break;
    }
    return std::wstring(szTim);
}

// 格式化角度显示
std::wstring Formatter::formatAngle(double angle, int format) {
    static wchar_t szAng[64];
    
    switch (format) {
    case 0:
        /*常规格式化：度数、分*/
        {
            angle = fmod(angle + 0.5 / 60.0, 360.0);
            if (angle < 0) angle += 360.0;
            
            int d = (int)angle;
            int m = (int)(fmod(angle, 1.0) * 60.0);
            
            swprintf(szAng, 64, L"%2d°%02d′", d, m);
        }
        break;

    default:
        /* 以小数度数格式显示 */
        swprintf(szAng, 64, L"%.6f°", angle);
        break;
    }
    return std::wstring(szAng);
}

// 格式化出生数据
std::wstring Formatter::formatBirthData(const BirthData& data) {
    static wchar_t szFormatted[512];
    swprintf(szFormatted, 512, 
        L"姓名: %ls\n地点: %ls\n时间: %04d年%02d月%02d日 %ls\n时区: %.2f\n坐标: %.6f, %.6f",
        data.name, data.location, data.year, data.month, data.day,
        formatTime(data.time).c_str(), data.timezone, data.latitude, data.longitude);
    return std::wstring(szFormatted);
}

// 字符串比较函数，忽略大小写
bool Formatter::stringsMatch(const wchar_t* s1, const wchar_t* s2, int n) {
    for (; *s1 && *s2; s1++, s2++) {
        if (towlower(*s1) != towlower(*s2)) {
            return false;
        }

        if (--n == 0) {
            break;
        }
    }
    return true;
}

// 解析字符串为整数
int Formatter::parseStringToInt(const wchar_t* szEntry, int pm) {
    wchar_t szLocal[256], * sz;
    int cch, i;
    int returnflag = 0;

    /* First strip off any leading or trailing spaces. */
    for (cch = 0; (szLocal[cch] = szEntry[cch]) != 0; cch++)
        ;
    szLocal[cch] = L'\0';
    while (cch && szLocal[cch - 1] <= L' ')
        szLocal[--cch] = L'\0';
    for (sz = szLocal; *sz && *sz <= L' '; sz++, cch--)
        ;
    // fix IC name to be "IC "
    if (sz[0] == L'I' && sz[1] == L'C') {
        sz[2] = L' ';
        sz[3] = L'\0';
        cch++;
    }
    if (cch >= 3) // ask for a minimum of 3 characters, but why? sometimes less is enough!
    {
        switch (pm) {
            /* Parse months, e.g. "February" or "Feb" -> 2 for February. */
        case PARSE_MONTH:
            for (i = 1; i <= 12; i++) {
                if (stringsMatch(sz, L"", 0)) {
                    returnflag = i;
                    break;
                }
            }
            break;

        case PARSE_MONTH_EN:
            for (i = 1; i <= 12; i++) {
                // 这里应该使用英文月份名称数组
                if (stringsMatch(sz, L"", 0)) {
                    returnflag = i;
                    break;
                }
            }
            break;

        case PARSE_SIGN:
            for (i = 1; i <= 12; i++) {
                // 将char*转换为wchar_t*进行比较
                size_t len = strlen(signNamesCh[i]) + 1;
                wchar_t* wideName = new wchar_t[len];
                mbstowcs(wideName, signNamesCh[i], len);
                if (stringsMatch(sz, wideName, 0)) {
                    delete[] wideName;
                    returnflag = i;
                    break;
                }
                delete[] wideName;
            }
            break;

        case PARSE_SIGN_EN:
            for (i = 1; i <= 12; i++) {
                // 这里应该使用英文星座名称数组
                // 将char*转换为wchar_t*进行比较
                size_t len = strlen(signNamesEn[i]) + 1;
                wchar_t* wideName = new wchar_t[len];
                mbstowcs(wideName, signNamesEn[i], len);
                if (stringsMatch(sz, wideName, 0)) {
                    delete[] wideName;
                    returnflag = i;
                    break;
                }
                delete[] wideName;
            }
            break;
        }

        if (returnflag) return returnflag;
    }

    // 默认转换为整数
    return std::stoi(sz);
}

// 解析字符串为数值
double Formatter::parseStringToDouble(const char* szEntry, int pm) {
    char szLocal[256], * sz, * pch, ch, chdot, minutes[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
    int cch, havedot, dot1, dot2, newstyle = FALSE, i, j, f = FALSE;
    double r, rMinutes, rSeconds;

    /* First strip off any leading or trailing spaces. */
    for (cch = 0; (szLocal[cch] = szEntry[cch]) != 0; cch++)
        ;
    while (cch && szLocal[cch - 1] == ' ')
        szLocal[--cch] = '\0';
    for (sz = szLocal; *sz && *sz == ' '; sz++, cch--);
    ;

    /* Capitalize all letters and make colons be periods to be like numbers. */
    for (pch = sz; *pch; pch++) {
        ch = *pch;
        if (ch == ':' || ch == '\'' || ch == '\"')
            ch = '.';
        else
            ch = toupper(ch);
        *pch = ch;
    }
    ch = sz[0];

    if (pm == PARSE_DST) {
        /* For the Daylight time flag, "Daylight", "Yes", and "True" (or just */
        /* their first characters) are all indications to be ahead one hour.  */
        if ((strcmp(sz, "YES") == 0) || (sz[0] == 'Y' && sz[1] == 0) || (strcmp(sz, "DT") == 0)) 
            return 1.0;
        else if ((strcmp(sz, "NO") == 0) || (sz[0] == 'N' && sz[1] == 0) || (strcmp(sz, "ST") == 0)) 
            return 0.0;
        else 
            return atof(sz);
    }
    else if (pm == PARSE_LONGITUDE || pm == PARSE_LATITUDE) {
        /* For locations, negate the value for an "E" or "S" in the middle    */
        /* somewhere (e.g. "105E30" or "27:40S") for eastern/southern values. */
        for (i = 0; i < cch; i++) {
            ch = sz[i];
            if (isupper(ch)) {
                if ((pm == PARSE_LONGITUDE && ch == 'W') || (pm == PARSE_LATITUDE && ch == 'S'))
                    f = TRUE;  // 需要负号的情况
                else if ((pm == PARSE_LONGITUDE && ch == 'E') || (pm == PARSE_LATITUDE && ch == 'N'))
                    f = FALSE; // 不需要负号的情况
                sz[i] = '.';   // 替换为点号以便解析
                i = cch;
            }
        }
        ch = sz[0];
    }

    /* Anything still at this point should be in a numeric format. */
    if (!isdigit(ch) && ch != '+' && ch != '-' && ch != '.') {
        return 999.0; // Error value
    }
    
    r = (f ? -1.0 : 1.0) * atof(sz);
    return r;
}
