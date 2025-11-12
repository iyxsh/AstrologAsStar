#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <locale>
#include "../include/astrolog_lib.h"
#include "../include/utils/utils.h"
#include "../include/utils/TransU.h"

int main(int argc,char* argv[])
{
    // 检查参数个数
    if (argc != 12) {
        printf("使用示例: astrolog32_oldmain 2025 3 22 \"12:30:00\" \"No\" \"-8:00:00\" \"116:23'50E\" \"39:54'27N\" 39.00 \"此时此刻\" \"北京, 中国\"\n");
        printf("参数说明:\n");
        printf("  1. 年份 (例如: 2025)\n");
        printf("  2. 月份 (例如: 3)\n");
        printf("  3. 日期 (例如: 22)\n");
        printf("  4. 时间 (例如: \"12:30:00\")\n");
        printf("  5. 夏令时 (例如: \"No\" 或具体时间)\n");
        printf("  6. 时区 (例如: \"-8:00:00\")\n");
        printf("  7. 经度 (例如: \"116:23'50E\")\n");
        printf("  8. 纬度 (例如: \"39:54'27N\")\n");
        printf("  9. 海拔 (例如: 39.00)\n");
        printf("  10. 名称 (例如: \"此时此刻\")\n");
        printf("  11. 地点 (例如: \"北京, 中国\")\n");
        return 1;
    }

	initEnv();
	CI ciInput = { -1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, L"", L"","" ,L"","" };
	ciInput.yea = atoi(argv[1]);
	ciInput.mon = atoi(argv[2]);
	ciInput.day = atoi(argv[3]);
	ciInput.tim = RParseSz(argv[4], pmTim);
	ciInput.dst = RParseSz(argv[5], pmDst);
	ciInput.zon = RParseSz(argv[6], pmZon);
	ciInput.lon = RParseSz(argv[7], pmLon);
	ciInput.lat = RParseSz(argv[8], pmLat);
	ciInput.alt = atof(argv[9]);
	swprintf(ciInput.nam, sizeof(ciInput.nam) / sizeof(wchar_t), L"%ls", char_to_wchar(argv[10]).c_str());
	swprintf(ciInput.loc, sizeof(ciInput.loc) / sizeof(wchar_t), L"%ls", char_to_wchar(argv[11]).c_str());
	//swprintf(ciInput.nam, sizeof(ciInput.nam) / sizeof(wchar_t), L"%s", argv[9]);
	//swprintf(ciInput.loc, sizeof(ciInput.loc) / sizeof(wchar_t), L"%s", argv[10]);
	//GetChartResult(ciInput,false);  调试用 
	GetChartResult(ciInput);
	//神奇的代码，把下面这行加入调试到这里会报错哦
	//printf("直接输入：° | Unicode转义：\u00B0\n");

	return 0;
}