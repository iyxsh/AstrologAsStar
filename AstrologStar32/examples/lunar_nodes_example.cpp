#include "../include/astrolog.h"
#include "../include/core/planet.h"
#include "../include/core/houses.h"
#include "../include/core/ephemeris.h"
#include "../include/core/lunar_nodes.h"
#include "../include/utils/formatter.h"
#include "../include/models/chart_data.h"
#include "../include/data/signs.h"
#include "../include/core/planet.h"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

void printBanner() {
    wcout << L"========================================" << endl;
    wcout << L"    AstrologStar32 月亮交点功能演示" << endl;
    wcout << L"========================================" << endl << endl;
}

void demonstrateLunarNodes() {
    wcout << L"月亮交点计算演示" << endl;
    wcout << L"------------------------" << endl;
    
    // 创建示例出生数据
    BirthData birthData = {0};
    birthData.year = 1990;
    birthData.month = 6;
    birthData.day = 15;
    birthData.time = 14.5;  // 14:30
    birthData.dst = 0.0;    // 无夏令时
    birthData.timezone = 8.0; // 东八区
    birthData.longitude = 116.4074; // 北京经度
    birthData.latitude = 39.9042;   // 北京纬度
    birthData.altitude = 0.0;
    swprintf(birthData.name, sizeof(birthData.name) / sizeof(wchar_t), L"张三");
    swprintf(birthData.location, sizeof(birthData.location) / sizeof(wchar_t), L"北京");
    
    wcout << L"出生数据: " << Formatter::formatBirthData(birthData) << endl;
    
    // 初始化星历表
    EphemerisCalculator::initializeEphemeris();
    
    // 计算儒略日
    double jd = EphemerisCalculator::calculateJulianDay(
        birthData.year, birthData.month, birthData.day, 
        birthData.time - birthData.timezone + birthData.dst);
    
    wcout << L"儒略日: " << jd << endl;
    
    // 计算所有月亮交点
    vector<LunarNode> lunarNodes;
    LunarNodeCalculator::calculateAllLunarNodes(jd, lunarNodes);
    
    wcout << L"\n月亮交点位置:" << endl;
    for (const auto& node : lunarNodes) {
        wcout << setw(15) << left << node.name << L": " 
              << LunarNodeCalculator::formatLunarNode(node, 2) << endl;
    }
    
    // 计算宫位
    HouseData houses;
    if (HouseCalculator::calculateHouses(jd, birthData.latitude, birthData.longitude, 0, &houses) == 0) {
        wcout << L"\n交点所在宫位:" << endl;
        for (const auto& node : lunarNodes) {
            int house = LunarNodeCalculator::getHouse(node.longitude, houses);
            wcout << setw(15) << left << node.name << L": 第" << house << L"宫" << endl;
        }
    }
    
    wcout << endl;
}

int main() {
    printBanner();
    
    try {
        demonstrateLunarNodes();
        wcout << L"月亮交点演示完成！" << endl;
    } catch (const exception& e) {
        wcout << L"发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}