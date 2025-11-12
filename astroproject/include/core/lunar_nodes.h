#ifndef LUNAR_NODES_H
#define LUNAR_NODES_H

#include "../utils/utils.h"


// 月交点相关常量和函数声明

// 注意：这些值在planet.h枚举中已定义，这里仅作注释参考
// oNoNode = 16
// oSoNode = 17 
// oLil = 18

#define FNodal(obj)   ((obj) == 17 || (obj) == 16)  // oSoNode=17, oNoNode=16
#define FGeo(obj)     ((obj) == oMoo || FNodal(obj))
#define FCust(obj)    FBetween(obj, custLo, custHi)
// 函数声明
void ComputeLunarNodes(void);

#endif // LUNAR_NODES_H