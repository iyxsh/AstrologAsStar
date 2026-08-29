/*
 * swe_sidtime_args 自定义扩展函数声明（AstrologAsStar）
 *
 * SWE 保持 GitHub 官方原版（submodule: astroproject/swe/swisseph），
 * 本函数以补丁形式存在于 astroproject/swe/swe_sidtime_args.c，
 * 此头文件供项目源码（houses.cpp 等）包含。
 */
#ifndef SWE_SIDTIME_ARGS_H
#define SWE_SIDTIME_ARGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* 计算恒星时，同时返回交角/章动/动力学时（见 .c 文件注释）
 * 返回值为恒星时（度）；*epsPtr 黄赤交角、*nutloPtr[2] 章动（度）、*tjde 动力学时 */
double swe_sidtime_args(double tjd_ut, double *epsPtr, double *nutloPtr, double *tjde);

#ifdef __cplusplus
}
#endif

#endif /* SWE_SIDTIME_ARGS_H */
