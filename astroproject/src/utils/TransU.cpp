// ��ƽ̨ת������ �¼�ת������
#include "TransU.h"
// ��ƽ̨ת������
std::string wchar_to_char(const wchar_t* wstr)
{
	if (wstr == nullptr) return "";

	// ����ϵͳ���ػ����루�����ƽ̨������
	setlocale(LC_ALL, ""); // �Զ�����ϵͳĬ�ϱ���

	// �������軺������С
	size_t len = wcstombs(nullptr, wstr, 0);
	if (len == (size_t)-1) return ""; // ת��ʧ�ܴ���

	// �����ڴ沢ִ��ת��
	char* buffer = new char[len + 1];
	wcstombs(buffer, wstr, len);
	buffer[len] = '\0';

	std::string result(buffer);
	delete[] buffer;
	return result;
}

// 平台判断宏
#if defined(_LINUX)
    #define PLATFORM_LINUX
	#include <unistd.h>
#elif defined(_WIN32)
    #define PLATFORM_WINDOWS
    #include <windows.h>
#elif defined(_APPLE)
    #define PLATFORM_MACOS
    #include <CoreFoundation/CoreFoundation.h>
#elif defined(_ANDROID)
    #define PLATFORM_ANDROID
    #include <sys/system_properties.h>
#endif

std::wstring char_to_wchar(const char* str, const char* locale) {
	if (str == nullptr || strlen(str) == 0) return L"";

	std::wstring result;
#if defined(_WIN32)
	//UTF-8 或者 CP_ACP(默认的windows兼容强)
	int wlen = MultiByteToWideChar(CP_ACP, 0, str,-1, nullptr, 0);
	if (wlen <= 0) return L"";
	std::vector<wchar_t> buffer(wlen);
	MultiByteToWideChar(CP_ACP, 0, str, -1, buffer.data(), wlen);
	result = buffer.data();
#else
	// Linux/macOS  locale
	setlocale(LC_ALL, locale);
	size_t wlen = mbstowcs(nullptr, str, 0);
	if (wlen == (size_t)-1) return L"";
	std::vector<wchar_t> buffer(wlen + 1);
	mbstowcs(buffer.data(), str, buffer.size());
	buffer[wlen] = L'\0';
	result = buffer.data();
#endif
	return result;
}



// 获取系统时区的核心函数
const char* get_system_timezone() {
    static char tz[128] = {0};

    // Linux 实现
    #ifdef PLATFORM_LINUX
    {
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
    }
    #endif

    // Windows 实现
    #ifdef PLATFORM_WINDOWS
    {
        TIME_ZONE_INFORMATION tzInfo;
        if (GetTimeZoneInformation(&tzInfo) != TIME_ZONE_ID_INVALID) {
            wcstombs(tz, tzInfo.StandardName, sizeof(tz)-1);
            return tz;
        }
    }
    #endif

    // macOS 实现
    #ifdef PLATFORM_MACOS
    {
        CFTimeZoneRef tzRef = CFTimeZoneCopySystem();
        CFStringRef tzName = CFTimeZoneGetName(tzRef);
        CFStringGetCString(tzName, tz, sizeof(tz)-1, kCFStringEncodingUTF8);
        CFRelease(tzRef);
        return tz;
    }
    #endif

    // Android 实现
    #ifdef PLATFORM_ANDROID
    {
        char value[PROP_VALUE_MAX];
        if (__system_property_get("persist.sys.timezone", value) > 0) {
            strncpy(tz, value, sizeof(tz)-1);
            return tz;
        }
    }
    #endif

    return "Unknown";
}
