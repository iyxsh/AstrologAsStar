﻿// astrolog32.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <cstring>

// TODO: 在此处引用程序需要的其他标头。
#define ChDst(dst)    (dst == 0.0 ? 'S' : (dst == 1.0 ? 'D' : 'A'))
#define ChDstW(dst)    (dst == 0.0 ? L'S' : (dst == 1.0 ? L'D' : L'A'))
typedef int BOOL;
#define FALSE 0
#define TRUE 1

#if defined(_WIN32)
	#include "windows.h"
	#include "swe/inc/swedll.h"
    #define STR_CMPI _stricmp
	#define MEM_CCPY _memccpy
	#define STR_DUP _strdup
	#define X_MEMSET x_memset_Intel
#else
	#include "swe/inc/swephexp.h"
	#include "swe/inc/sweph.h"
	#include "swe/inc/swephlib.h"
	#include "swe/inc/swehouse.h"
	#include "swe/inc/sweodef.h"

	#include <string.h>
	#define STR_CMPI strcasecmp
	#define MEM_CCPY memccpy
	#define STR_DUP strdup
	#define X_MEMSET x_memset_GCC_CLANG
#endif