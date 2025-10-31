#ifndef CELESTIAL_BODY_H
#define CELESTIAL_BODY_H

#include <string>

// 天体数据结构
struct CelestialBody {
    int id;                      // 天体ID
    std::wstring name;           // 天体名称
    std::wstring symbol;         // 天体符号
    double longitude;            // 黄经
    double latitude;             // 黄纬
    double speed;                // 速度
    double distance;             // 距离
    double rectAscension;        // 赤经
    double declination;          // 赤纬
    int house;                   // 所在宫位
};

#endif // CELESTIAL_BODY_H