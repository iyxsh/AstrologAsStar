/*
 * swe_sidtime_args —— AstrologAsStar 自定义 SWE 扩展函数
 *
 * 背景：原项目在本地 swisseph 仓库中直接修改了 swephlib.c 添加此函数，
 * 并随预编译 libswe.a 分发。为使 SWE 保持 GitHub 官方原版（submodule），
 * 此函数以独立补丁形式实现，编译进 astrolog32 库，对外行为与旧版完全一致。
 *
 * 行为（与旧自定义实现一致）：
 *   计算恒星时，同时通过出参返回黄赤交角 eps、章动 nutlo[2] 与动力学时 tjde。
 *   - *tjde   = tjd_ut + swe_deltat(tjd_ut)   （动力学时）
 *   - *epsPtr = 交角（度）
 *   - *nutlo  = 章动 [0]=交角章动 [1]=黄经章动（度）
 *   返回值 = swe_sidtime0(tjd_ut, eps+nutlo[1], nutlo[0])，即恒星时（度）。
 *
 * 头文件：swe_sidtime_args.h（本目录），houses.cpp 已改为包含该头。
 */
#define _GNU_SOURCE
#include <stdio.h>
#include "swephexp.h"
#include "sweph.h"
#include "swephlib.h"
#include "swe_sidtime_args.h"

double CALL_CONV swe_sidtime_args(double tjd_ut, double *epsPtr, double *nutloPtr, double *tjde)
{
  int i;
  double tsid;
  /* delta t adjusted to default tidal acceleration of the moon */
  *tjde = tjd_ut + swe_deltat(tjd_ut);
  swi_init_swed_if_start();
  *epsPtr = swi_epsiln(*tjde, 0) * RADTODEG;
  swi_nutation(*tjde, 0, nutloPtr);
  for (i = 0; i < 2; i++)
    nutloPtr[i] *= RADTODEG;
  tsid = swe_sidtime0(tjd_ut, *epsPtr + nutloPtr[1], nutloPtr[0]);
  return tsid;
}
