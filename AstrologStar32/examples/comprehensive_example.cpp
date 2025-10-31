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
#include "../include/core/planet.h"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

void printBanner() {
    wcout << L"========================================" << endl;
    wcout << L"    AstrologStar32 综合功能演示" << endl;
    wcout << L"========================================" << endl << endl;
}

void demonstrateNatalChart() {
    wcout << L"1. 本命盘计算演示" << endl;
    wcout << L"------------------------" << endl;
    
    // 创建示例出生数据
    BirthData birthData = {0};
    birthData.year = 1981;
    birthData.month = 12;
    birthData.day = 22;
    birthData.time = 6.25;  // 14:30
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
    
    // 计算行星位置
    vector<PlanetData> planets;
    PlanetCalculator::calculateAllPlanets(jd, planets);
    
    wcout << L"\n行星位置:" << endl;
    for (size_t i = 0; i < planets.size() && i < 10; i++) {
        wcout << setw(4) << left << planetNamesEn[i] << L": " 
              << PlanetCalculator::formatPlanetPosition(planets[i], 0) << endl;
    }
    
    // 计算宫位
    HouseData houses;
    if (HouseCalculator::calculateHouses(jd, birthData.latitude, birthData.longitude, 0, &houses) == 0) {
        wcout << L"\n宫位:" << endl;
        for (int i = 1; i <= 12; i++) {
            wcout << L"第" << i << L"宫: " 
                  << HouseCalculator::formatHouseCusp(houses.cusps[i], 0) << endl;
        }
        
        wcout << L"\n特殊点:" << endl;
        wcout << L"上升点: " << HouseCalculator::formatHouseCusp(houses.ascendant, 0) << endl;
        wcout << L"中天: " << HouseCalculator::formatHouseCusp(houses.mc, 0) << endl;
        wcout << L"顶点: " << HouseCalculator::formatHouseCusp(houses.vertex, 0) << endl;
    }
    
    // 计算相位
    AspectSettings settings;
    AspectCalculator::initializeDefaultSettings(settings);
    vector<Aspect> aspects;
    AspectCalculator::calculateAspects(planets, settings, aspects);
    
    wcout << L"\n主要相位:" << endl;
    int aspectCount = 0;
    for (const auto& aspect : aspects) {
        // 只显示主要相位（重要性较高的相位）
        if (aspect.aspectType <= 5 && aspectCount < 15) {
            wcout << planetNamesEn[aspect.planet1] << L" 与 " 
                  << planetNamesEn[aspect.planet2] << L" 形成 "
                  << AspectCalculator::formatAspect(aspect.aspectType)
                  << L"，偏离: " << Formatter::formatAngle(aspect.orb, 2) << L"度" << endl;
            aspectCount++;
        }
    }
    
    wcout << endl;
}

void demonstrateProgressions() {
    wcout << L"2. 推进计算演示" << endl;
    wcout << L"------------------------" << endl;
    
    BirthData birthData = {0};
    birthData.year = 1990;
    birthData.month = 6;
    birthData.day = 15;
    birthData.time = 14.5;
    birthData.dst = 0.0;
    birthData.timezone = 8.0;
    birthData.longitude = 116.4074;
    birthData.latitude = 39.9042;
    
    double birthJD = EphemerisCalculator::calculateJulianDay(
        birthData.year, birthData.month, birthData.day, 
        birthData.time - birthData.timezone + birthData.dst);
    
    // 计算当前时间的儒略日（假设现在是2023年）
    double currentJD = EphemerisCalculator::calculateJulianDay(2023, 6, 15, 12.0);
    
    vector<PlanetData> natalPlanets;
    PlanetCalculator::calculateAllPlanets(birthJD, natalPlanets);
    
    vector<PlanetData> progressedPlanets;
    HouseData progressedHouses;
    ProgressionCalculator::calculateSecondaryProgressions(birthData, currentJD, progressedPlanets, progressedHouses);
    
    wcout << L"33年后的次限推进行星位置:" << endl;
    for (size_t i = 0; i < progressedPlanets.size() && i < 10; i++) {
        wcout << setw(4) << left << planetNamesEn[i] << L": " 
              << PlanetCalculator::formatPlanetPosition(progressedPlanets[i], 0) << endl;
    }
    
    wcout << L"\n推进宫位:" << endl;
    for (int i = 1; i <= 12; i++) {
        wcout << L"第" << i << L"宫: " 
              << HouseCalculator::formatHouseCusp(progressedHouses.cusps[i], 0) << endl;
    }
    
    wcout << endl;
}

void demonstrateTransits() {
    wcout << L"3. 推运计算演示" << endl;
    wcout << L"------------------------" << endl;
    
    double jd = EphemerisCalculator::getCurrentJulianDay();
    
    vector<PlanetData> natalPlanets;
    PlanetCalculator::calculateAllPlanets(jd - 365.25 * 33, natalPlanets); // 33年前的行星位置作为本命盘
    
    vector<TransitEvent> transitEvents;
    AspectSettings settings;
    AspectCalculator::initializeDefaultSettings(settings);
    TransitCalculator::calculateTransitAspects(natalPlanets, jd, settings, transitEvents);
    
    wcout << L"当前重要推运:" << endl;
    int eventCount = 0;
    for (const auto& event : transitEvents) {
        if (eventCount++ >= 10) break;
        if (event.orb < 2.0) { // 只显示接近精确的相位
            wcout << planetNamesEn[event.transitingPlanet] << L" 与本命"
                  << planetNamesEn[event.natalPlanet] << L"形成"
                  << AspectCalculator::formatAspect(event.aspectType)
                  << L"，偏离: " << Formatter::formatAngle(event.orb, 2) << L"度" << endl;
        }
    }
    
    wcout << endl;
}

void demonstrateArabicParts() {
    wcout << L"4. 阿拉伯点计算演示" << endl;
    wcout << L"------------------------" << endl;
    
    double jd = EphemerisCalculator::getCurrentJulianDay();
    vector<PlanetData> planets;
    PlanetCalculator::calculateAllPlanets(jd, planets);
    
    HouseData houses;
    HouseCalculator::calculateHouses(jd, 39.9042, 116.4074, 0, &houses);
    
    vector<ArabicPart> arabicParts;
    ArabicPartCalculator::calculateAllArabicParts(planets, houses, arabicParts);
    
    wcout << L"主要阿拉伯点:" << endl;
    const int majorParts[] = {
        ARABIC_PART_FORTUNE, ARABIC_PART_SPIRIT, ARABIC_PART_EROS,
        ARABIC_PART_VICTORY, ARABIC_PART_CAREER, ARABIC_PART_MARRIAGE
    };
    
    const wchar_t* partNames[] = {
        L"财运点", L"精神点", L"爱情点", L"胜利点", L"事业点", L"婚姻点"
    };
    
    for (int i = 0; i < 6; i++) {
        int index = majorParts[i];
        if (index < (int)arabicParts.size()) {
            wcout << setw(6) << left << partNames[i] << L": " 
                  << ArabicPartCalculator::formatArabicPart(arabicParts[index], 0) << endl;
        }
    }
    
    wcout << endl;
}

void demonstrateFixedStars() {
    wcout << L"5. 恒星计算演示" << endl;
    wcout << L"------------------------" << endl;
    
    double jd = EphemerisCalculator::getCurrentJulianDay();
    vector<FixedStar> fixedStars;
    FixedStarCalculator::calculateMajorFixedStars(jd, fixedStars);
    
    wcout << L"主要恒星位置:" << endl;
    int starCount = 0;
    for (const auto& star : fixedStars) {
        if (starCount++ >= 8) break;
        wcout << FixedStarCalculator::formatFixedStar(star, 0) << endl;
    }
    
    wcout << endl;
}

void demonstrateSynastry() {
    wcout << L"6. 合盘分析演示" << endl;
    wcout << L"------------------------" << endl;
    
    // 第一个人
    BirthData person1 = {0};
    person1.year = 1990;
    person1.month = 6;
    person1.day = 15;
    person1.time = 14.5;
    person1.dst = 0.0;
    person1.timezone = 8.0;
    person1.longitude = 116.4074;
    person1.latitude = 39.9042;
    
    double jd1 = EphemerisCalculator::calculateJulianDay(
        person1.year, person1.month, person1.day, 
        person1.time - person1.timezone + person1.dst);
    
    vector<PlanetData> planets1;
    PlanetCalculator::calculateAllPlanets(jd1, planets1);
    
    HouseData houses1;
    HouseCalculator::calculateHouses(jd1, person1.latitude, person1.longitude, 0, &houses1);
    
    // 第二个人
    BirthData person2 = {0};
    person2.year = 1992;
    person2.month = 3;
    person2.day = 22;
    person2.time = 9.0;
    person2.dst = 0.0;
    person2.timezone = 8.0;
    person2.longitude = 116.4074;
    person2.latitude = 39.9042;
    
    double jd2 = EphemerisCalculator::calculateJulianDay(
        person2.year, person2.month, person2.day, 
        person2.time - person2.timezone + person2.dst);
    
    vector<PlanetData> planets2;
    PlanetCalculator::calculateAllPlanets(jd2, planets2);
    
    HouseData houses2;
    HouseCalculator::calculateHouses(jd2, person2.latitude, person2.longitude, 0, &houses2);
    
    // 合盘分析
    SynastryReport report;
    SynastryAnalyzer::analyzeSynastry(planets1, planets2, houses1, houses2, report);
    
    wcout << L"整体兼容性评分: " << fixed << setprecision(1) << report.overallCompatibility << L"/100" << endl;
    
    wcout << L"\n重要合盘相位:" << endl;
    int aspectCount = 0;
    for (const auto& aspect : report.aspects) {
        if (aspectCount++ >= 8) break;
        if (aspect.importance >= IMPORTANCE_SIGNIFICANT) {
            wcout << SynastryAnalyzer::formatSynastryAspect(aspect, 
                                                          vector<wstring>(planetNamesEn, planetNamesEn + 10),
                                                          vector<wstring>(planetNamesEn, planetNamesEn + 10))
                  << endl;
        }
    }
    
    wcout << endl;
}

int main() {
    printBanner();
    
    try {
        demonstrateNatalChart();
        demonstrateProgressions();
        demonstrateTransits();
        demonstrateArabicParts();
        demonstrateFixedStars();
        demonstrateSynastry();
        
        wcout << L"演示完成！" << endl;
    } catch (const exception& e) {
        wcout << L"发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}