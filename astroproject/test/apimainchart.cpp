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
        printf("使用示例: astrolog32_apimain 2025 3 22 \"12:30:00\" \"No\" \"-8:00:00\" \"116:23'50E\" \"39:54'27N\" 39.00 \"此时此刻\" \"北京, 中国\"\n");
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
	ChartInput chartInput = {0};
	int yea = atoi(argv[1]);
	int mon = atoi(argv[2]);
	int day = atoi(argv[3]);
	double tim = RParseSz(argv[4],pmTim);
	double dst = RParseSz(argv[5],pmDst);
	double zon = RParseSz(argv[6],pmZon);
	double lon = RParseSz(argv[7],pmLon);
	double lat = RParseSz(argv[8],pmLat);
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
	chartInput.alt = atof(argv[9]);
	swprintf(chartInput.nam, sizeof(chartInput.nam) / sizeof(wchar_t), L"%ls", char_to_wchar(argv[10]).c_str());
	swprintf(chartInput.loc, sizeof(chartInput.loc) / sizeof(wchar_t), L"%ls", char_to_wchar(argv[11]).c_str());
	
	//swprintf(chartInput.nam, sizeof(chartInput.nam) / sizeof(wchar_t), L"%s", argv[9]);
	//swprintf(chartInput.loc, sizeof(chartInput.loc) / sizeof(wchar_t), L"%s", argv[10]);
	//GetChartResult(chartInput,false);  调试用 
	SetChartData(1,chartInput);
    std::wstring result = GetMainChartAspect();
    //std::wstring result 使用 wprintf 输出 使用 std::wcout 输出 可能没有内容输出
	wprintf(L">>>=== 最终结果 ===>>> ：\n");
    wprintf(L"%ls\n", result.c_str());

	return 0;
}