#include "../include/transcoder.h"
#include <cstdlib>
#include <cstring>
#include <vector>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

// 跨平台转码函数
std::string wchar_to_char(const wchar_t* wstr)
{
    if (wstr == nullptr) return "";

    // 设置系统区域设置（处理多平台兼容性）
    setlocale(LC_ALL, "");

    // 计算转换后字符串所需的空间大小
    size_t len = wcstombs(nullptr, wstr, 0);
    if (len == (size_t)-1) return "";

    // 分配内存并执行转换
    char* buffer = new char[len + 1];
    wcstombs(buffer, wstr, len);
    buffer[len] = '\0';

    std::string result(buffer);
    delete[] buffer;
    return result;
}

std::wstring char_to_wchar(const char* str, const char* locale) {
    if (str == nullptr || strlen(str) == 0) return L"";

    std::wstring result;
    
#if defined(_WIN32)
    // Windows平台使用MultiByteToWideChar
    int wlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (wlen <= 0) return L"";
    std::vector<wchar_t> buffer(wlen);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, buffer.data(), wlen);
    result = buffer.data();
#else
    // Linux/macOS平台使用区域设置
    if (strlen(locale) > 0) {
        setlocale(LC_ALL, locale);
    } else {
        setlocale(LC_ALL, "en_US.UTF-8");
    }
    size_t wlen = mbstowcs(nullptr, str, 0);
    if (wlen == (size_t)-1) return L"";
    std::vector<wchar_t> buffer(wlen + 1);
    mbstowcs(buffer.data(), str, buffer.size());
    buffer[wlen] = L'\0';
    result = buffer.data();
#endif
    return result;
}

const char* get_system_timezone() {
    static char tz[128] = {0};

#ifdef _WIN32
    // Windows实现
    TIME_ZONE_INFORMATION tzInfo;
    if (GetTimeZoneInformation(&tzInfo) != TIME_ZONE_ID_INVALID) {
        wcstombs(tz, tzInfo.StandardName, sizeof(tz)-1);
        return tz;
    }
#elif defined(__linux__)
    // Linux实现
    const char* tzfile = "/etc/localtime";
    char buffer[256];
    ssize_t len = readlink(tzfile, buffer, sizeof(buffer)-1);
    if (len != -1) {
        buffer[len] = '\0';
        char* pos = strstr(buffer, "zoneinfo/");
        if (pos) {
            strncpy(tz, pos + 9, sizeof(tz)-1);
            return tz;
        }
    }
    // 备用方案：读取时区文件
    FILE* fp = fopen("/etc/timezone", "r");
    if (fp) {
        if (fgets(tz, sizeof(tz), fp)) {
            char* end = strchr(tz, '\n');
            if (end) *end = '\0';
            fclose(fp);
            return tz;
        }
        fclose(fp);
    }
#elif defined(__APPLE__)
    // macOS实现
    // 简化处理，实际项目中可以使用CoreFoundation框架
    strcpy(tz, "America/New_York");
    return tz;
#endif

    return "UTC";
}