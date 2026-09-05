#include "../../include/core/config.h"
#include "../../include/core/planet.h"    /* oVesta/uranLo/oMoo 等对象常量 */
#include "../../include/core/houses.h"    /* NUMBER_OF_HOUSE_SYSTEMS 枚举 */
#include "../../include/core/aspects.h"   /* cAspect/ignoreA/UpdateAspectCount (A6) */
#include "../../include/models/settings.h"
#include "../../include/utils/utils.h"
#include "../../swe/swisseph/swephexp.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* cPart(177) 目前仅 astrolog.cpp:156 定义（无头文件）。P1.5 阿拉伯点表头化后
 * 移入共享头；此处先行镜像，避免 utils.h FValidPart 展开失败。 */
#ifndef cPart
#define cPart 177
#endif

/* ---------------------------------------------------------------------------
 * 外部全局（与 astrolog.cpp 定义一致；未在头文件导出的在此补 extern）
 * ------------------------------------------------------------------------- */
extern US us;          /* settings.h 未必导出 extern，此处显式声明 */
extern byte oscLilith; /* astrolog.cpp:374 */
extern byte PolarMCflip;/* astrolog.cpp:373 */
extern double rAspOrb[];   /* utils.cpp 相位容差表（默认同原版 {7,7,7,7,6,3,…}） */
extern double rObjOrb[];   /* utils.cpp 对象容差表 */
extern double rAspAngle[]; /* aspects.cpp 相位角度表（-Aa 自定义） */
extern double rObjAdd[];   /* utils.cpp 对象容差增量表（-Ad 自定义） */

/* 原版开关机常量（astrolog.cpp:290-291） */
#define FIRST_SIDEREAL_MODE SE_SIDM_FAGAN_BRADLEY /* 必须恒为 0 */
#define LAST_SIDEREAL_MODE  SE_SIDM_GALCENT_0SAG

/* ---------------------------------------------------------------------------
 * 原版开关机制移植（astrolog.cpp:21853-21871 语义）
 *   '-' 默认: 取反切换    '=' : 强制开(fOr)   '_': 强制关(fAnd)   ':': 复位 no-op
 * ------------------------------------------------------------------------- */
static int FChSwitchLocal(char ch)
{
	return (ch == '-' || ch == '=' || ch == '_' || ch == ':') ? 1 : 0;
}

static void SetErr(char* errtxt, size_t errsz, const char* fmt, const char* sw, const char* arg)
{
	if (errtxt && errsz) {
		if (arg) snprintf(errtxt, errsz, "switch -%s: invalid argument '%s'", sw, arg);
		else     snprintf(errtxt, errsz, "switch -%s: %s", sw, fmt ? fmt : "error");
	}
}

/* ---------------------------------------------------------------------------
 * 解释器主循环（原版 FProcessSwitchesMain 子集）
 * ------------------------------------------------------------------------- */
int ConfigProcessTokens(const char* const* argv, int argc,
                        char* errtxt, size_t errsz)
{
	int i = 0;
	int fOr = 0, fAnd = 0, fNot = 0;
	char ch1, ch2;

	if (errtxt && errsz) errtxt[0] = '\0';

	while (i < argc) {
		const char* tok = argv[i];
		if (!tok || !tok[0]) { i++; continue; }

		/* 前缀：'-'/'='/'_'/':' */
		ch1 = tok[0];
		fOr = fAnd = fNot = 0;
		switch (ch1) {
		case '=': fOr = 1; break;
		case '_': fAnd = 1; break;
		case ':': break; /* 复位语义：FSwitchF 结果为原值 → no-op */
		default:  fNot = 1; break;
		}
		if (!FChSwitchLocal(tok[0])) {
			/* 非开关 token：忽略（dat 文件可能含图表数据行，P1 不做输入解析） */
			i++;
			continue;
		}

		ch1 = tok[1];          /* 开关字母（如 -c 5 的 'c'） */
		ch2 = (ch1 == '\0') ? '\0' : tok[2];

#define FSwitchF(f) ((((f) | fOr) & !fAnd) ^ fNot)
#define SwitchF(f)  f = FSwitchF(f)

		switch (ch1) {
		/* ---- -c <n> 宫位系统 16 系（A2） ---- */
		case 'c': {
			if (i + 1 >= argc) { SetErr(errtxt, errsz, "needs a house-system number", "c", NULL); return 0; }
			int n = atoi(argv[i + 1]);
			if (!FValidSystem(n)) { SetErr(errtxt, errsz, NULL, "c", argv[i + 1]); return 0; }
			us.nHouseSystem = n;
			i++; /* 消费数字参数 */
			break;
		}

		/* ---- -s 恒星黄道族：[-s <offset>] [-sr|-sh|-sd|-sz|-sm <n>|bare] ---- */
		case 's': {
			if (ch2 == 'm') {
				/* -sm <n>：分点模式（需独立数字参数） */
				if (i + 1 >= argc) { SetErr(errtxt, errsz, "-sm needs a sidereal-mode number", "sm", NULL); return 0; }
				int j = atoi(argv[i + 1]);
				if (j < FIRST_SIDEREAL_MODE || j > LAST_SIDEREAL_MODE) {
					SetErr(errtxt, errsz, NULL, "sm", argv[i + 1]); return 0;
				}
				if (us.nSiderealMode != j) {
					us.nSiderealMode = j;
					swe_set_sid_mode(us.nSiderealMode, 0, 0);
				}
				i++;
			} else {
				/* 可选前置数字 = 岁差偏移（原版：除 -sm 外任何子开关先试消费数字参数；
				 * 门控 atof!=0 或首字符为数字，避免吞掉后续开关 token） */
				if (i + 1 < argc) {
					const char* a = argv[i + 1];
					double rT = atof(a);
					if (rT != 0.0 || (a[0] >= '0' && a[0] <= '9')) {
						if (!FValidOffset(rT)) { SetErr(errtxt, errsz, NULL, "s", a); return 0; }
						us.rSiderealCorrection = rT;
						i++;
					}
				}
				if (ch2 == 'r')      SwitchF(us.fEquator);
				else if (ch2 == 'h') us.nDegForm = 1;
				else if (ch2 == 'd') us.nDegForm = 2;
				else if (ch2 == 'z') us.nDegForm = 0;
				else { /* bare -s：切换恒星黄道 */
					SwitchF(us.fSidereal);
				}
			}
			break;
		}

		/* ---- -h <n> 中心天体（A4/后续日心） ---- */
		case 'h': {
			if (i + 1 >= argc) { SetErr(errtxt, errsz, "needs a center-object number", "h", NULL); return 0; }
			int n = atoi(argv[i + 1]);
			if (!FValidCenter(n)) { SetErr(errtxt, errsz, NULL, "h", argv[i + 1]); return 0; }
			us.objCenter = n;
			i++;
			break;
		}

		/* ---- -P 阿拉伯点族（A5）：[-P <n>] [-Pz|-Pn|-Pf] [-P0] ---- */
		case 'P': {
			if (i + 1 < argc) {
				int n = atoi(argv[i + 1]);
				if (n != 0) {
					if (!FValidPart(n)) { SetErr(errtxt, errsz, NULL, "P", argv[i + 1]); return 0; }
					us.nArabicParts = n;
					i++;
				}
			}
			if (ch2 == 'z' || ch2 == 'n' || ch2 == 'f') {
				us.nArabic = ch2;
				/* 原版继续处理第三字母（ch1=ch2; ch2=ch3），此处 '0' 翻转 */
				if (tok[3] == '0') SwitchF(us.fArabicFlip);
			} else {
				SwitchF(us.nArabic);
			}
			if (ch2 == '0') SwitchF(us.fArabicFlip);
			break;
		}

		/* ---- -A 族 相位自定义（A6 余项；镜像原版 -Ao/-Am）：
		 *    -Ao <asp> <deg> 设相位容差（负数=旧版语义→忽略该相）；-Am <obj> <deg> 对象容差 ---- */
		case 'A': {
			/* -Ao <asp> <deg> 相位容差 / -Am <obj> <deg> 对象容差 /
			 * -Aa <asp> <deg> 相位角度 / -Ad <obj> <deg> 对象容差增量
			 * （镜像原版 case 'A' 子开关：o/m/d → 对应表，default(含 a) → rAspAngle）。
			 * -An/-AA 等未实现：安全 no-op。 */
			if (ch2 != 'o' && ch2 != 'm' && ch2 != 'a' && ch2 != 'd') break;
			const char* swName = (ch2 == 'o') ? "Ao" : (ch2 == 'm') ? "Am" : (ch2 == 'a') ? "Aa" : "Ad";
			if (i + 2 >= argc) { SetErr(errtxt, errsz, "-Ao/-Am/-Aa/-Ad need index and degrees", swName, NULL); return 0; }
			const char* aIdx = argv[i + 1];
			const char* aVal = argv[i + 2];
			int idx = atoi(aIdx);
			double val = atof(aVal);
			switch (ch2) {
			case 'o':
				if (idx < 1 || idx > cAspect) { SetErr(errtxt, errsz, NULL, "Ao", aIdx); return 0; }
				if (val < 0.0) { val = 0.0; ignoreA[idx] = 1; }   /* 原版兼容：负 orb → 忽略该相 */
				rAspOrb[idx] = val;
				UpdateAspectCount();
				break;
			case 'm':
				if (idx < 1 || idx > cLastMoving) { SetErr(errtxt, errsz, NULL, "Am", aIdx); return 0; }
				rObjOrb[idx] = val;
				break;
			case 'a':
				if (idx < 1 || idx > cAspect) { SetErr(errtxt, errsz, NULL, "Aa", aIdx); return 0; }
				if (val < -360.0 || val > 360.0) { SetErr(errtxt, errsz, "Aa angle out of range", aVal, NULL); return 0; }
				rAspAngle[idx] = val;
				break;
			case 'd':
				if (idx < 1 || idx > cLastMoving) { SetErr(errtxt, errsz, NULL, "Ad", aIdx); return 0; }
				rObjAdd[idx] = val;
				break;
			}
			i += 2;
			break;
		}

		/* ---- -Y 族（原版 process_switches_Y 子集；P1 特性域） ---- */
		case 'Y': {
			switch (ch2) {
			case 'n': SwitchF(us.fTrueNode); break;               /* 真交点（A3） */
			case 'L': SwitchF(oscLilith);    break;               /* 真莉莉丝远地点（A3） */
			case 'c': SwitchF(us.fHouseAngle); break;             /* 宫位角 */
			case 'd': SwitchF(us.fEuroDate); break;               /* 欧式日期 */
			case 't': SwitchF(us.fEuroTime); break;               /* 欧式时间 */
			case 'C': SwitchF(us.fSmartAspects); break;           /* 智能相位 */
			case 'H': SwitchF(PolarMCflip);  break;               /* 极区 MC 翻转 */
			default:
				/* 其余 -Y 子开关未实现：安全 no-op（原版未知亦不报错） */
				break;
			}
			break;
		}

		/* ---- -U 恒星显示（A4：补齐默认占位槽位的真实恒星计算） ---- */
		case 'U': {
			us.fAllStar = 1;
			if (ch2 == 'z')      us.nStar = 'z';
			else if (ch2 == 'n') us.nStar = 'n';
			else if (ch2 == 'b') us.nStar = 'b';
			else if (ch2 == 'l') us.nStar = 'l';
			else if (ch2 == 'p') us.nStar = 'p';
			/* 其余 -U 子开关：保持 fAllStar=1，排序留默认 */
			break;
		}

		/* ---- -RA/-RE 相位屏蔽（A6，镜像原版 -RA/-RE 语义）：
		 *    -RA <asp>…  屏蔽（ignoreA=1）；-RE <asp>…  启用（ignoreA=0）；
		 *    随后按启动公式重算 us.nAsp。数字外 token 停止消费。 ---- */
		case 'R': {
			int fRestrict;
			if (ch2 == 'A')      fRestrict = 1;
			else if (ch2 == 'E') fRestrict = 0;
			else break;                          /* 其余 -R 子开关未实现：安全 no-op */
			while (i + 1 < argc) {
				const char* a = argv[i + 1];
				if (!a || !isdigit((unsigned char)a[0])) break;
				int asp = atoi(a);
				if (asp < 1 || asp > cAspect) { SetErr(errtxt, errsz, NULL, ch2 == 'A' ? "RA" : "RE", a); return 0; }
				ignoreA[asp] = fRestrict ? 1 : 0;
				i++;
			}
			UpdateAspectCount();
			break;
		}

		/* ---- 其余字母（图表类型/输入/输出等）：CLI 自管或未实现 → no-op ---- */
		default:
			break;
		}

#undef SwitchF
#undef FSwitchF
		i++;
	}
	return 1;
}

/* ---------------------------------------------------------------------------
 * 单行/单 token 快捷
 * ------------------------------------------------------------------------- */
int ConfigApply(const char* token, char* errtxt, size_t errsz)
{
	const char* v[2] = { token, NULL };
	return ConfigProcessTokens(v, 1, errtxt, errsz);
}

/* ---------------------------------------------------------------------------
 * dat 文件加载：逐行切词（空白分隔，双引号分组），'#'/';' 行首注释
 * ------------------------------------------------------------------------- */
int ConfigLoadFile(const char* path, char* errtxt, size_t errsz)
{
	FILE* f = fopen(path, "r");
	if (!f) {
		if (errtxt && errsz) snprintf(errtxt, errsz, "cannot open config file '%s'", path);
		return 0;
	}

	char line[1024];
	char toks[256][128];
	int lineNo = 0;
	int ret = 1;

	while (ret && fgets(line, sizeof(line), f)) {
		lineNo++;
		/* 去行尾 CR/LF */
		size_t len = strlen(line);
		while (len && (line[len - 1] == '\n' || line[len - 1] == '\r'))
			line[--len] = '\0';
		/* 去前导空白 */
		char* p = line;
		while (*p == ' ' || *p == '\t') p++;
		if (!*p || *p == '#' || *p == ';') continue;

		/* 切词：空白分隔，双引号组为一个 token */
		int ntok = 0;
		char* q = p;
		while (*q && ntok < 256) {
			while (*q == ' ' || *q == '\t' || *q == '\r') q++;
			if (!*q) break;
			if (*q == '"') {
				q++;
				int c = 0;
				while (*q && *q != '"' && c < 127) toks[ntok][c++] = *q++;
				toks[ntok][c] = '\0';
				if (*q == '"') q++;
			} else {
				int c = 0;
				while (*q && *q != ' ' && *q != '\t' && c < 127) toks[ntok][c++] = *q++;
				toks[ntok][c] = '\0';
			}
			ntok++;
		}
		if (!ntok) continue;

		const char* tokv[256];
		for (int t = 0; t < ntok; t++) tokv[t] = toks[t];
		if (!ConfigProcessTokens(tokv, ntok, errtxt, errsz)) {
			if (errtxt && errsz) {
				char add[200];
				snprintf(add, sizeof(add), " (file %s line %d)", path, lineNo);
				strncat(errtxt, add, errsz - strlen(errtxt) - 1);
			}
			ret = 0;
		}
	}
	fclose(f);
	return ret;
}
