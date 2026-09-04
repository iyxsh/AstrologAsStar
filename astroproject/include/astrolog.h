// astrolog32.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。
#ifndef ASTROLOG_H
#define ASTROLOG_H

#define NOMINMAX
#include <iostream>
#include <cstring>
#include <cstdlib>

// 包含核心模块
#include "core/aspects.h"
#include "core/chart.h"
#include "core/ephemeris.h"
#include "core/fixed_stars.h"
#include "core/houses.h"
#include "core/lunar_nodes.h"
#include "core/planet.h"
#include "core/progressions.h"
#include "core/synastry.h"
#include "core/transits.h"

// 包含模型
#include "models/settings.h"
#include "models/chart_data.h"
#include "data/signs.h"

// 包含工具
#include "utils/parser.h"
#include "utils/formatter.h"
#include "utils/utils.h"
#include "utils/TransU.h"

// 包含库接口
#include "astrolog_lib.h"

// TODO: 在此处引用程序需要的其他标头。
#define ChDst(dst)    (dst == 0.0 ? 'S' : (dst == 1.0 ? 'D' : 'A'))
#define ChDstW(dst)    (dst == 0.0 ? L'S' : (dst == 1.0 ? L'D' : L'A'))

#endif
