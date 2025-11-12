#ifndef FIXED_STARS_H
#define FIXED_STARS_H
#include "../utils/utils.h"

// 恒星亮度相关常量
#define sefstarsName "sefstars.txt"
#define SwapN(n1, n2) {int  tmp = n1; n1 = n2; n2 = tmp;}

#define starMaxName 12
#define starMaxNName 8
extern const char *szStarConName[];
extern double  rStarBright[];
// 函数声明
bool FileFind(const char* szFile, char* szDir, char* path_found);
bool CalculateStarSE(int i, double t_plac, double *longitude, double *latitude);
void ComputeStars(double SD);

#endif // FIXED_STARS_H