#include "../../include/core/lunar_nodes.h"
#include "../../include/core/planet.h"
#include "../../include/useswe.h"
#include "../../include/utils/formatter.h"
#include "../../include/data/signs.h"
#include <swephexp.h>
#include <vector>
#include <cmath>

using namespace std;

// 计算月亮交点
void LunarNodeCalculator::calculateLunarNodes(double jd, LunarNodeType nodeType, LunarNode& ascendingNode, LunarNode& descendingNode) {
    int32 iflag = SEFLG_SPEED | SEFLG_SWIEPH;
    double xp[6];
    char serr[256];
    
    int32 ipl = (nodeType == LUNAR_NODE_MEAN) ? SE_MEAN_NODE : SE_TRUE_NODE;
    
    // 计算月亮北交点
    int result = swe_calc_ut(jd, ipl, iflag, xp, serr);
    if (result < 0) {
        // 如果计算失败，设置默认值
        ascendingNode.longitude = 0.0;
        ascendingNode.latitude = 0.0;
        ascendingNode.speed = 0.0;
    } else {
        ascendingNode.longitude = xp[0];
        ascendingNode.latitude = xp[1];
        ascendingNode.speed = xp[3];
    }
    
    ascendingNode.type = nodeType;
    ascendingNode.name = (nodeType == LUNAR_NODE_MEAN) ? L"月亮北交点(平)" : L"月亮北交点(真)";
    
    // 计算月亮南交点（北交点+180度）
    descendingNode.longitude = fmod(ascendingNode.longitude + 180.0, 360.0);
    descendingNode.latitude = -ascendingNode.latitude;
    descendingNode.speed = ascendingNode.speed;
    descendingNode.type = nodeType;
    descendingNode.name = (nodeType == LUNAR_NODE_MEAN) ? L"月亮南交点(平)" : L"月亮南交点(真)";
}

// 计算所有月亮交点（包括平交点和真交点）
void LunarNodeCalculator::calculateAllLunarNodes(double jd, std::vector<LunarNode>& nodes) {
    nodes.clear();
    nodes.reserve(4); // 预留空间
    
    LunarNode ascendingMeanNode, descendingMeanNode;
    calculateLunarNodes(jd, LUNAR_NODE_MEAN, ascendingMeanNode, descendingMeanNode);
    nodes.push_back(ascendingMeanNode);
    nodes.push_back(descendingMeanNode);
    
    LunarNode ascendingTrueNode, descendingTrueNode;
    calculateLunarNodes(jd, LUNAR_NODE_TRUE, ascendingTrueNode, descendingTrueNode);
    nodes.push_back(ascendingTrueNode);
    nodes.push_back(descendingTrueNode);
}

// 格式化交点位置
std::wstring LunarNodeCalculator::formatLunarNode(const LunarNode& node, int precision) {
    // 修复：使用已存在的方法和函数
    int sign = (int)(node.longitude / 30);
    
    // 将char*转换为wchar_t*
    size_t len = strlen(signNamesEn[sign+1]) + 1;
    wchar_t* wideName = new wchar_t[len];
    mbstowcs(wideName, signNamesEn[sign+1], len);
    std::wstring signName(wideName);
    delete[] wideName;
    
    return Formatter::formatAngle(node.longitude, precision) + L" " + signName;
}

// 获取交点所在的星座
int LunarNodeCalculator::getSign(double longitude) {
    // 修复：直接实现该功能而不是调用不存在的方法
    return (int)(longitude / 30);
}

// 获取交点所在的宫位
int LunarNodeCalculator::getHouse(double longitude, const HouseData& houses) {
    // 简化实现，实际应该进行宫位判断
    for (int i = 1; i <= 12; i++) {
        double nextCusp = (i == 12) ? houses.cusps[1] : houses.cusps[i + 1];
        double currentCusp = houses.cusps[i];
        
        // 处理跨0度的情况
        if (currentCusp > nextCusp) {
            if ((longitude >= currentCusp && longitude < 360.0) || 
                (longitude >= 0.0 && longitude < nextCusp)) {
                return i;
            }
        } else {
            if (longitude >= currentCusp && longitude < nextCusp) {
                return i;
            }
        }
    }
    return 1; // 默认返回第一宫
}