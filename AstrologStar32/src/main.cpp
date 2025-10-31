#include "../include/astrolog.h"
#include "../include/core/planet.h"
#include "../include/core/houses.h"
#include "../include/core/aspects.h"
#include "../include/core/ephemeris.h"
#include "../include/core/progressions.h"
#include "../include/core/transits.h"
#include "../include/core/arabic_parts.h"
#include "../include/core/fixed_stars.h"
#include "../include/core/synastry.h"
#include "../include/core/lunar_nodes.h"
#include "../include/utils/formatter.h"
#include "../include/models/chart_data.h"
#include "../include/data/signs.h"
#include <iostream>
#include <vector>
#include <string>
#include <cwchar>
#include <exception>
#include <locale>
#include <iomanip>
#include <cstdlib>

using namespace std;

// 显示程序信息
void printBanner() {
    wcout << L"AstrologStar32 天体计算程序 v1.0" << endl;
    wcout << L"使用 Swiss Ephemeris 库进行天体位置计算" << endl << endl;
    wcout.flush();
}

// 初始化星历表库
bool initializeEphemeris() {
    try {
        wcout << L"正在初始化星历表库..." << endl;
        wcout.flush();
        EphemerisCalculator::initializeEphemeris();
        wcout << L"星历表库初始化完成" << endl;
        wcout.flush();
        return true;
    } catch (const exception& e) {
        wcerr << L"初始化星历表库时发生异常: " << e.what() << endl;
        wcerr.flush();
        return false;
    } catch (...) {
        wcerr << L"初始化星历表库时发生未知错误" << endl;
        wcerr.flush();
        return false;
    }
}

// 解析命令行参数 (调整参数顺序以匹配原项目)
bool parseCommandLineArguments(int argc, char* argv[], BirthData& birthData) {
    if (argc >= 12) {
        wcout << L"处理命令行参数..." << endl;
        wcout.flush();
        
        // 调整参数顺序以匹配原项目: 月 日 年 时间 夏令时 时区 经度 纬度 海拔 姓名 地点
        birthData.month = atoi(argv[1]);
        birthData.day = atoi(argv[2]);
        birthData.year = atoi(argv[3]);
        
        // 验证日期参数是否有效
        if (birthData.month < 1 || birthData.month > 12) {
            wcout << L"错误: 月份必须在1-12之间，当前输入为 " << birthData.month << endl;
            wcout.flush();
            return false;
        }
        
        // 检查每个月的实际天数
        int daysInMonth = 31;  // 默认31天
        switch (birthData.month) {
            case 4: case 6: case 9: case 11:
                daysInMonth = 30;
                break;
            case 2:
                // 闰年判断: 能被4整除但不能被100整除，或者能被400整除
                if ((birthData.year % 4 == 0 && birthData.year % 100 != 0) || (birthData.year % 400 == 0)) {
                    daysInMonth = 29;
                } else {
                    daysInMonth = 28;
                }
                break;
        }
        
        if (birthData.day < 1 || birthData.day > daysInMonth) {
            wcout << L"错误: " << birthData.year << L"年" << birthData.month << L"月的日期必须在1-" << daysInMonth << L"之间，当前输入为 " << birthData.day << endl;
            wcout.flush();
            return false;
        }
        
        birthData.time = Formatter::parseStringToDouble(argv[4], PARSE_TIME);
        birthData.dst = Formatter::parseStringToDouble(argv[5], PARSE_DST);
        birthData.timezone = Formatter::parseStringToDouble(argv[6], PARSE_ZONE);
        birthData.longitude = Formatter::parseStringToDouble(argv[7], PARSE_LONGITUDE);
        birthData.latitude = Formatter::parseStringToDouble(argv[8], PARSE_LATITUDE);
        birthData.altitude = atof(argv[9]);
        
        // 将argv[10]转换为宽字符并存储到birthData.name中
        size_t len = strlen(argv[10]) + 1;
        wchar_t* wname = new wchar_t[len];
        size_t converted = mbstowcs(wname, argv[10], len);
        if (converted != static_cast<size_t>(-1)) {
            swprintf(birthData.name, sizeof(birthData.name) / sizeof(wchar_t), L"%ls", wname);
        } else {
            swprintf(birthData.name, sizeof(birthData.name) / sizeof(wchar_t), L"未知");
        }
        delete[] wname;
        
        // 将argv[11]转换为宽字符并存储到birthData.location中
        size_t len2 = strlen(argv[11]) + 1;
        wchar_t* wlocation = new wchar_t[len2];
        converted = mbstowcs(wlocation, argv[11], len2);
        if (converted != static_cast<size_t>(-1)) {
            swprintf(birthData.location, sizeof(birthData.location) / sizeof(wchar_t), L"%ls", wlocation);
        } else {
            swprintf(birthData.location, sizeof(birthData.location) / sizeof(wchar_t), L"未知地点");
        }
        delete[] wlocation;
        
        return true;
    } else {
        return false;
    }
}

// 设置默认示例数据
void setDefaultData(BirthData& birthData) {
    // 默认示例数据（北京时间）
    birthData.year = 1990;
    birthData.month = 1;
    birthData.day = 1;
    birthData.time = 12.0;  // 中午12点
    birthData.dst = 0.0;    // 无夏令时
    birthData.timezone = 8.0; // 东八区
    birthData.longitude = 116.4074; // 北京经度
    birthData.latitude = 39.9042;   // 北京纬度
    birthData.altitude = 0.0;
    swprintf(birthData.name, sizeof(birthData.name) / sizeof(wchar_t), L"示例");
    swprintf(birthData.location, sizeof(birthData.location) / sizeof(wchar_t), L"北京");
}

// 显示出生数据
void displayBirthData(const BirthData& birthData) {
    wcout << L"出生数据:" << endl;
    wcout << L"姓名: " << birthData.name << endl;
    wcout << L"地点: " << birthData.location << endl;
    wcout << L"年: " << birthData.year << L" 月: " << birthData.month << L" 日: " << birthData.day << endl;
    wcout << L"时间: " << birthData.time << L" 时区: " << birthData.timezone << L" 夏令时: " << birthData.dst << endl;
    wcout << L"经度: " << birthData.longitude << L" 纬度: " << birthData.latitude << L" 海拔: " << birthData.altitude << endl << endl;
    wcout.flush();
}

// 显示行星位置
void displayPlanets(const vector<PlanetData>& planets) {
    wcout << endl << L"所有行星位置:" << endl;
    
    wcout << L"行星    位置" << endl;
    for (size_t i = 0; i < planets.size() && i < 10; i++) {
        wcout << setw(8) << left << planetNamesEn[i] << L": " 
              << PlanetCalculator::formatPlanetPosition(planets[i], 0).c_str() << endl;
    }
    wcout.flush();
}

// 显示宫位
void displayHouses(const HouseData& houses) {
    wcout << endl << L"宫位:" << endl;
    for (int i = 1; i <= 12; i++) {
        wcout << L"第" << setw(2) << i << L"宫: ";
        wcout << HouseCalculator::formatHouseCusp(houses.cusps[i], 0).c_str();
        wcout << L"  \n";
    }
    
    // 显示特殊点
    wcout << L"\n\n特殊点:" << endl;
    wcout << L"上升点: ";
    wcout << HouseCalculator::formatHouseCusp(houses.ascendant, 0).c_str() << endl;
    wcout << L"中天:   ";
    wcout << HouseCalculator::formatHouseCusp(houses.mc, 0).c_str() << endl;
    wcout << L"顶点:   ";
    wcout << HouseCalculator::formatHouseCusp(houses.vertex, 0).c_str() << endl;
    wcout.flush();
}

// 显示相位
void displayAspects(const vector<Aspect>& aspects, const vector<PlanetData>& planets) {
    wcout << L"\n主要相位:" << endl;
    const wchar_t* planet_names[] = {
        L"太阳", L"月亮", L"水星", L"金星", L"火星", 
        L"木星", L"土星", L"天王星", L"海王星", L"冥王星"
    };
    
    const wchar_t* aspect_names[] = {
        L"", L"合相", L"冲相", L"四分", L"三分", L"六分", 
        L"梅花相", L"半四分", L"一度半", L"半六分", L"五分", L"双五分"
    };
    
    int aspectCount = 0;
    for (const auto& aspect : aspects) {
        // 只显示主要相位（重要性较高的相位）
        if (aspect.aspectType >= 1 && aspect.aspectType <= 6 && aspectCount < 30) {
            wcout << setw(6) << left << planet_names[aspect.planet1] << L" 与 " 
                  << setw(6) << left << planet_names[aspect.planet2] << L" 形成 "
                  << setw(8) << left << aspect_names[aspect.aspectType]
                  << L"，偏离: " << fixed << setprecision(2) << aspect.orb << L"度" 
                  << (aspect.applying ? L" (入相)" : L" (出相)") << endl;
            aspectCount++;
        }
    }
    
    if (aspectCount == 0) {
        wcout << L"未发现主要相位" << endl;
    }
    wcout.flush();
}

// 主函数
int main(int argc, char* argv[]) {
    // 设置本地化支持宽字符
    std::setlocale(LC_ALL, "");
    std::locale::global(std::locale(""));
    
    // 在某些系统上需要明确设置标准输出为宽字符模式
    ios_base::sync_with_stdio(false);
    wcout.imbue(std::locale(""));
    wcerr.imbue(std::locale(""));

    printBanner();
    
    if (!initializeEphemeris()) {
        return 1;
    }
    
    // 如果提供了命令行参数，则使用参数创建出生数据
    BirthData birthData = {0};
    if (!parseCommandLineArguments(argc, argv, birthData)) {
        setDefaultData(birthData);
    }
    
    displayBirthData(birthData);
    
    // 计算儒略日
    wcout << L"正在计算儒略日..." << endl;
    wcout.flush();
    double jd = EphemerisCalculator::calculateJulianDay(
        birthData.year, birthData.month, birthData.day, 
        birthData.time - birthData.timezone + birthData.dst);
    
    wcout << L"儒略日: " << jd << endl;
    wcout.flush();
    
    // 计算所有行星的位置
    wcout << L"正在计算行星位置..." << endl;
    wcout.flush();
    vector<PlanetData> planets;
    PlanetCalculator::calculateAllPlanets(jd, planets);
    wcout << L"行星位置计算完成，共计算 " << planets.size() << L" 颗行星" << endl;
    wcout.flush();
    
    displayPlanets(planets);
    
    // 计算宫位
    wcout << L"\n正在计算宫位..." << endl;
    wcout.flush();
    HouseData houses = {0};
    int houseResult = HouseCalculator::calculateHouses(jd, birthData.latitude, birthData.longitude, 0, &houses);
    
    if (houseResult == 0) {
        displayHouses(houses);
    } else {
        wcout << L"宫位计算失败，错误代码: " << houseResult << endl;
        wcout.flush();
    }
    
    // 计算相位
    wcout << L"\n正在计算相位..." << endl;
    wcout.flush();
    AspectSettings settings;
    AspectCalculator::initializeDefaultSettings(settings);
    vector<Aspect> aspects;
    AspectCalculator::calculateAspects(planets, settings, aspects);
    wcout << L"相位计算完成，共找到 " << aspects.size() << L" 个相位" << endl;
    wcout.flush();
    
    displayAspects(aspects, planets);
    
    wcout << L"\n程序执行完成" << endl;
    wcout.flush();
    return 0;
}