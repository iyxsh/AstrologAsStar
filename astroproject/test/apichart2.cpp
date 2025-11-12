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

// 验证并设置星盘数据的函数
void SetChartInput(ChartInput& chartInput, int yea, int mon, int day, 
                   double tim, double dst, double zon, double lon, double lat, 
                   double alt, const char* name, const char* loc) {
    if(FValidYea(yea))
        chartInput.yea = yea;
    if(FValidMon(mon))
        chartInput.mon = mon;
    if(FValidDay(day,mon,yea))
        chartInput.day = day;
    if(FValidTim(tim))
        chartInput.tim = tim;
    if(FValidDst(dst))
        chartInput.dst = dst;
    if(FValidZon(zon))
        chartInput.zon = zon;
    if(FValidLon(lon))
        chartInput.lon = lon;
    if(FValidLat(lat))
        chartInput.lat = lat;
    chartInput.alt = alt;
    swprintf(chartInput.nam, sizeof(chartInput.nam) / sizeof(wchar_t), L"%ls", char_to_wchar(name).c_str());
    swprintf(chartInput.loc, sizeof(chartInput.loc) / sizeof(wchar_t), L"%ls", char_to_wchar(loc).c_str());
}

int main(int argc,char* argv[])
{
    // 检查参数个数
    if (argc != 23) {
        printf("使用示例: astrolog32_apichart2 2025 3 22 \"12:30:00\" \"No\" \"-8:00:00\" \"116:23'50E\" \"39:54'27N\" 39.00 \"此时此刻\" \"北京, 中国\" 1990 5 15 \"08:45:00\" \"No\" \"-8:00:00\" \"120:10'30E\" \"30:15'45N\" 25.00 \"合作伙伴\" \"上海, 中国\"\n");
        printf("参数说明:\n");
        printf("  第一组星盘参数 (11个):\n");
        printf("    1. 年份 (例如: 2025)\n");
        printf("    2. 月份 (例如: 3)\n");
        printf("    3. 日期 (例如: 22)\n");
        printf("    4. 时间 (例如: \"12:30:00\")\n");
        printf("    5. 夏令时 (例如: \"No\" 或具体时间)\n");
        printf("    6. 时区 (例如: \"-8:00:00\")\n");
        printf("    7. 经度 (例如: \"116:23'50E\")\n");
        printf("    8. 纬度 (例如: \"39:54'27N\")\n");
        printf("    9. 海拔 (例如: 39.00)\n");
        printf("    10. 名称 (例如: \"此时此刻\")\n");
        printf("    11. 地点 (例如: \"北京, 中国\")\n");
        printf("\n");
        printf("  第二组星盘参数 (11个):\n");
        printf("    12. 年份 (例如: 1990)\n");
        printf("    13. 月份 (例如: 5)\n");
        printf("    14. 日期 (例如: 15)\n");
        printf("    15. 时间 (例如: \"08:45:00\")\n");
        printf("    16. 夏令时 (例如: \"No\" 或具体时间)\n");
        printf("    17. 时区 (例如: \"-8:00:00\")\n");
        printf("    18. 经度 (例如: \"120:10'30E\")\n");
        printf("    19. 纬度 (例如: \"30:15'45N\")\n");
        printf("    20. 海拔 (例如: 25.00)\n");
        printf("    21. 名称 (例如: \"合作伙伴\")\n");
        printf("    22. 地点 (例如: \"上海, 中国\")\n");
        return 1;
    }

    #if defined(_WIN32) || defined(_WIN64)
        ////设置输入输出编码为 UTF-8
	    //SetConsoleOutputCP(65001);
	    //SetConsoleCP(65001);
        // 设置区域为中文环境
        //std::locale::global(std::locale("chs"));
        //std::wcout.imbue(std::locale("chs"));
	    //设置控制台为宽字符输出模式
	    int seno = _setmode(_fileno(stdout), _O_U16TEXT);  // 关键步骤！
	#endif
	initEnv();
	
	// 使用新的ChartInput结构体
	ChartInput chartInput1 = {0};
	ChartInput chartInput2 = {0};
	
	// 解析第一组星盘参数
	int yea1 = atoi(argv[1]);
	int mon1 = atoi(argv[2]);
	int day1 = atoi(argv[3]);
	double tim1 = RParseSz(argv[4],pmTim);
	double dst1 = RParseSz(argv[5],pmDst);
	double zon1 = RParseSz(argv[6],pmZon);
	double lon1 = RParseSz(argv[7],pmLon);
	double lat1 = RParseSz(argv[8],pmLat);
	double alt1 = atof(argv[9]);
	
	// 解析第二组星盘参数
	int yea2 = atoi(argv[12]);
	int mon2 = atoi(argv[13]);
	int day2 = atoi(argv[14]);
	double tim2 = RParseSz(argv[15],pmTim);
	double dst2 = RParseSz(argv[16],pmDst);
	double zon2 = RParseSz(argv[17],pmZon);
	double lon2 = RParseSz(argv[18],pmLon);
	double lat2 = RParseSz(argv[19],pmLat);
	double alt2 = atof(argv[20]);
	
	// 设置第一组星盘数据
	SetChartInput(chartInput1, yea1, mon1, day1, tim1, dst1, zon1, lon1, lat1, alt1, argv[10], argv[11]);
	
	// 设置第二组星盘数据
	SetChartInput(chartInput2, yea2, mon2, day2, tim2, dst2, zon2, lon2, lat2, alt2, argv[21], argv[22]);
	
	// 为合盘比较设置数据
	// 第一个星盘作为主要星盘
	SetChartData(1,chartInput1);
	// 第二个星盘作为比较星盘
	SetChartData(2,chartInput2);
	
    std::wstring result = GetChartAspectRelation();
    //std::wstring result 使用 wprintf 输出 使用 std::wcout 输出 可能没有内容输出
	wprintf(L">>>=== Chart2 最终结果 ===>>> ：\n");
    wprintf(L"%ls\n", result.c_str());

	return 0;
}