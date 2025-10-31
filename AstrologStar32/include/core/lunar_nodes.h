#ifndef LUNAR_NODES_H
#define LUNAR_NODES_H

#include "../models/chart_data.h"
#include <vector>

// 月亮交点类型
enum LunarNodeType {
    LUNAR_NODE_MEAN = 0,
    LUNAR_NODE_TRUE = 1
};

// 月亮交点数据结构
struct LunarNode {
    double longitude;     // 黄经
    double latitude;      // 黄纬
    double speed;         // 速度
    LunarNodeType type;   // 类型（平/真）
    const wchar_t* name;  // 名称
};

// 月亮交点计算器类
class LunarNodeCalculator {
public:
    // 计算月亮交点
    static void calculateLunarNodes(double jd, LunarNodeType nodeType, LunarNode& ascendingNode, LunarNode& descendingNode);
    
    // 计算所有月亮交点（包括平交点和真交点）
    static void calculateAllLunarNodes(double jd, std::vector<LunarNode>& nodes);
    
    // 格式化交点位置
    static std::wstring formatLunarNode(const LunarNode& node, int precision = 2);
    
    // 获取交点所在的星座
    static int getSign(double longitude);
    
    // 获取交点所在的宫位
    static int getHouse(double longitude, const HouseData& houses);
};

#endif // LUNAR_NODES_H