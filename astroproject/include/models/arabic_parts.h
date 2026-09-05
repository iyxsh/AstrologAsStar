// arabic_parts.h — 阿拉伯点 177 点表（A5；数据 100% 转录自原版 astrolog32 A32_V3_51_Proj
// astrolog.cpp `ai[cPart]` + 内嵌 name，form 为 11 字符公式 DSL，语义见 arabic_parts.cpp）。
#pragma once

#include <wchar.h>

#ifndef cPart
#define cPart 177      /* 阿拉伯点总数（原版常量；此前散落 astrolog.cpp/config.cpp） */
#endif

typedef struct ArabicPartInfo
{
	const wchar_t* form;   /* 11 字符公式：3 组 [mod][obj10][obj1]（详见 arabic_parts.cpp 解码注释） */
	const wchar_t* name;   /* 英文名（原版 tArabicPart 语言表内容即此） */
}
ArabicPartInfo;

extern const ArabicPartInfo g_ArabicPart[cPart];

/* apFor/apSpi：公式中以 'F'/'S' 引用前序已算点的伪对象号（原版 #define apFor 0 / apSpi 1） */
#define apFor 0
#define apSpi 1

/* 计算全部阿拉伯点黄经（度）。入口前置条件：CastChart 已完成（cp0.longitude /
 * cusp_pos / house_no / isDayBirth 就绪）。rPart[cPart] 输出；被引用对象处于
 * ignore1 屏蔽或引用未算前序点 → 该槽保持 -360.0 哨兵。数值语义逐行对齐原版
 * astrolog.cpp DisplayArabic() 的数值段（golden A32_V3_51 astrolog.cpp:33295）。
 */
void ComputeArabicParts(double* rPart);
