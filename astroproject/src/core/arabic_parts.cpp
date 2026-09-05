// arabic_parts.cpp — 阿拉伯点 177 点计算引擎（A5）。
//
// 数值核心逐行对齐原版 astrolog32（A32_V3_51_Proj，E:\IT\astrolog\A32_V3_51_Proj）
// astrolog.cpp `DisplayArabic()` 的取位段（原版 ~33295-33375）：
//
//   每条公式 form 为 11 字符 = 3 组 [mod][obj10][obj1]：
//     组内中/末字符决定“参照”：
//       ' ' → oAsc；'F' → 福点（-apFor，递归 rPart）；'S' → 精神点（-apSpi）
//       数字 → (obj10-'0')*10 + (obj1-'0') 的两位数对象/宫号 k
//     组首字符为“修饰符”：
//       'h' 宫头 cusp_pos[k]；'r' 宫头星座的守护星 longitude[rules[Z2Sign(cusp_pos[k])]]
//       'j' 宫头+10°；'H' 行星落宫宫头 cusp_pos[house_no[k]]
//       'R' 行星落宫星座守护星 longitude[rules[Z2Sign(cusp_pos[house_no[k]])]]
//       'D' 所落星座守护星 longitude[rules[Z2Sign(longitude[k])]]
//       '0'-'3' 常量度数 (ch-'0')*100 + k；' ' 普通对象/递归
//   最终：rCur = rBit[1] - rBit[2]；form[9]=='F'（福点族）且夜间翻转条件成立则取负；
//         rPart[i] = Mod(rCur + rBit[0])
//   夜间翻转条件与原版一致：(us.nArabicNight<0 && 日生) || (us.nArabicNight==0 && 夜生)
//
// 依赖（均为 CastChart 后就绪）：cp0.longitude/cusp_pos/house_no、rules（行星守护
// 星表）、ignore1（对象屏蔽）、isDayBirth、us.nArabicParts/nArabicNight。
#include "../../include/models/arabic_parts.h"
#include "../../include/models/settings.h"
#include "../../include/models/chart_data.h"
#include "../../include/core/planet.h"
#include "../../include/utils/utils.h"

extern US us;
extern CP cp0;
extern bool isDayBirth;             /* astrolog.cpp:188 全局 */
extern char rules[];                /* planet.cpp 行星守护星表（index=星座号） */

/* FCusp 宏在 planet.h 展开依赖缺失的 cSign，此处用具名枚举常量等价判定 */
#define FCuspObj(k) ((k) >= cuspLo && (k) <= oCusp12th)

void ComputeArabicParts(double* rPart)
{
	int i, j, k;
	double rCur, rBit[3];
	wchar_t ch;

	for (i = 0; i < cPart; i++)
	{
		rPart[i] = -360.0;
		if (i >= us.nArabicParts)
			continue;

		for (j = 0; j < 3; j++)
		{
			const wchar_t* pch = g_ArabicPart[i].form + j * 3;
			ch = pch[1];
			if (ch == L' ')
				k = oAsc;
			else if (ch == L'F')
				k = -apFor;
			else if (ch == L'S')
				k = -apSpi;
			else
				k = (ch - L'0') * 10 + (pch[2] - L'0');
			ch = pch[0];
			if (ch == L'h')                                     /* House cusp */
				rCur = cp0.cusp_pos[k];
			else if (ch == L'r')                                /* Ruler of house-cusp sign */
				rCur = cp0.longitude[rules[Z2Sign(cp0.cusp_pos[k])]];
			else if (ch == L'j')                                /* 10 degrees of house cusp */
				rCur = cp0.cusp_pos[k] + 10.0;
			else if (ch == L'H')                                /* Planet's house cusp */
				rCur = cp0.cusp_pos[cp0.house_no[k]];
			else if (ch == L'R')                                /* Ruler of planet's house sign */
				rCur = cp0.longitude[rules[Z2Sign(cp0.cusp_pos[cp0.house_no[k]])]];
			else if (ch == L'D')                                /* Dispositor of longitude[k] */
				rCur = cp0.longitude[rules[Z2Sign(cp0.longitude[k])]];
			else if (ch >= L'0' && ch <= L'3')                  /* Fixed-degree constant */
				rCur = (double)((ch - L'0') * 100 + k);
			else
			{
				if (k < 1)
				{
					rCur = rPart[-k];
					if (rCur < 0.0)
						goto NextPart;          /* 引用前序未算点 → 该点跳过 */
				}
				else
				{
					if (ignore1[k] && !FCuspObj(k))
						goto NextPart;          /* 引用对象被屏蔽 → 跳过 */
					else
						rCur = cp0.longitude[k];
				}
			}
			rBit[j] = rCur;
		}

		rCur = rBit[1] - rBit[2];

		if (g_ArabicPart[i].form[9] == L'F')    /* 福点族：日夜翻转语义 */
		{
			if ((us.nArabicNight < 0 && isDayBirth) ||
			    (us.nArabicNight == 0 && !isDayBirth))
				rCur = -rCur;
		}

		rCur = Mod(rCur + rBit[0]);
		rPart[i] = rCur;

	NextPart:;
	}
}

/* 177 点表数据（form+name，转录自原版 astrolog.cpp ai[cPart]） */
#include "arabic_parts_data.inc"
