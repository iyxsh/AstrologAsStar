#include "../../include/core/lunar_nodes.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/utils/utils.h"
#include <math.h>
extern US us;
extern CP cp0;
extern byte oscLilith;
void ComputeLunarNodes(void)
{
	double x, y, z;
	int i, j;
	bool fSouth = false; // 是否计算南交点
	
	// 确保月球和节点没有被忽略
	if (!ignore1[16] || !ignore1[17]) {  // oNoNode=16, oSoNode=17
		// 计算月球轨道的升交点和降交点
		x = cp0.longitude[16]; // 北交点经度 oNoNode=16
		y = cp0.latitude[16];  // 北交点纬度 oNoNode=16（通常接近0）
		
		// 根据北交点计算南交点
		if (!ignore1[17]) {  // oSoNode=17
			cp0.longitude[17] = Mod(x + 180.0);  // oSoNode=17
			cp0.latitude[17] = -y;               // oSoNode=17
			cp0.vel_longitude[17] = cp0.vel_longitude[16];  // oSoNode=17, oNoNode=16
			cp0.vel_latitude[17] = -cp0.vel_latitude[16];   // oSoNode=17, oNoNode=16
		}
		
		// 如果使用真北交点而非平均北交点
		if (us.objNode == 1) { // 真节点
			// 这里应该添加真节点的计算逻辑
			// 当前为占位实现，需要根据实际需求补充完整
		}
		
		// 如果使用南交点作为主要计算对象
		if (fSouth) {
			// 可以在这里添加南交点为主的相关计算
		}
	}
	
	// 计算 Lilith（月球远地点）
	if (!ignore1[18]) {  // oLil=18
		// 根据设置决定使用平均Lilith还是真实Lilith
		if (us.objOnLilith == 0) { // 平均Lilith
			// 使用已计算的平均远地点数据
			// 数据应该已经在主计算过程中填充
		} else { // 真实Lilith
			// 这里应该添加真实Lilith的计算逻辑
			// 当前为占位实现，需要根据实际需求补充完整
			oscLilith = 18;  // oLil=18
		}
	}
}