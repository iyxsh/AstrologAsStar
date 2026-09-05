#include "../../include/core/aspects.h"
#include "../../include/core/synastry.h"
#include "../../include/models/chart_data.h"
#include "../../include/models/settings.h"
#include "../../include/astrolog.h"
#include "../../include/utils/utils.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
extern CP cp0, cp1, cp2, cp3, cp4, cpPDs, cpBak;
extern int cSign;
extern bool UsePDsInChart;
extern bool IsPDsChartWithoutTable;

const double rAspAngle[cAspect + 1] = { 0,
	0.0, 180.0, 90.0, 120.0, 60.0, 150.0, 30.0, 45.0, 135.0, 72.0, 144.0,
	36.0, 360.0 / 7.0, 40.0, 80.0, 720.0 / 7.0, 1080.0 / 7.0, 160.0
};

/* Restriction of objects when progressing, as specified with -YRP switch.  */
/* For comparison charts only, else normal ignore1 used.						*/
/* Restriction of aspects as specified with -YRA switch. */
/* 相位屏蔽表（-RA/-RE 可改写；默认屏蔽 6..18 → us.nAsp 默认 5）。
 * 非 const：config 层 -RA/-RE 会修改后重算 nAsp。 */
byte ignoreA[cAspect + 1] = { 0,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};
/* 相位数量重算（与 initEnv 启动公式一致：nAsp = cAspect − 被屏蔽数） */
void UpdateAspectCount(void)
{
	int i;
	for (us.nAsp = cAspect, i = 1; i <= cAspect; i++)
		if (ignoreA[i])
			us.nAsp--;
}
/* This is Astrolog's memory allocation routine, returning a pointer given  */
/* a size, and if the allocation fails prints a warning, including the user */
/* passed as parameter.														*/
void *allocate(long length, char *user)
{
	void *p;

	p = malloc(length);

	if (p == NULL && user)
	{
		wprintf(L"Can't allocate memory for %s (%d bytes)", char_to_wchar(user).c_str(), length);
	}

	return p;
}

GridInfo *grid = NULL;
bool FEnsureGrid()
{
	if (grid != NULL)
		return true;
	grid = (GridInfo *) allocate(sizeof(GridInfo), (char*)"grid");
	memset(grid,0,sizeof(GridInfo));
	return grid != NULL;
}

/* 创建关系网格函数 - 用于计算两个星盘之间的相位或中点关系 */
bool FCreateGridRelation(bool fMidpoint)
{
	int i, j, k;        // 循环计数器和临时存储变量
	double l;           // 存储中点计算的临时结果
	// 确保网格数据结构已初始化
	if (!FEnsureGrid())
		return FALSE;
	// 清空网格数据，准备填充新的计算结果
	memset(&grid->n, 0, sizeof(grid->n));
	memset(&grid->v, 0, sizeof(grid->v));
	// 遍历所有天体对，计算它们之间的关系
	for (j = 0; j <= cObj; j++)
	{
		// 检查第一个天体是否需要被忽略
		if (!FIgnore(j) || !FIgnore2(j) || !FIgnore3(j))
		{
			for (i = 0; i <= cObj; i++)
			{
				// 检查第二个天体是否需要被忽略
				if (!FIgnore(i) || !FIgnore2(i) || !FIgnore3(i))
				{
					// 根据模式选择计算相位还是中点
					if (!fMidpoint)
					{
						// 计算常规相位
						GetAspect(cp1.longitude, cp2.longitude, cp1.vel_longitude, cp2.vel_longitude, i, j);
					}
					else
					{
						// 中点计算模式
						// 计算两个天体的中点位置
						l = Mod(Midpoint(cp2.longitude[i], cp1.longitude[j]));
						k = (int)l;
						// 存储中点所在的星座(1-12)
						grid->n[i][j] = k / 30 + 1;
						// 存储中点在星座内的分数位置(0-59)
						grid->v[i][j] = (int)((l - (double)(k / 30) * 30.0) * 60.0);
					}
				}
			}
		}
	}
	return TRUE;
}

/* Indicate whether some aspect between two objects should be shown. */
bool FAcceptAspect(int obj1, int asp, int obj2) 
{
	int fSupp;

	if (ignoreA[asp])			/* If the aspect restricted, reject immediately. */
		return false;

	if ( (us.nRel > rcDual && !us.fPrimDirs ) && FCusp(obj1) && FCusp(obj2))
		return false;

	if (FStar(obj1) && FStar(obj2))
		return false;

	if (us.fSmartAspects)
	{

		/* Allow only conjunctions to the minor house cusps. */

		if ((FMinor(obj1) || FMinor(obj2)) && asp > aCon)
			return false;

		/* Prevent any simultaneous aspects to opposing angle cusps,	 */
		/* e.g. if conjunct one, don't be opposite the other; if trine	 */
		/* one, don't sextile the other; don't square both at once, etc. */

		fSupp = (asp == aOpp || asp == aSex || asp == aSSx || asp == aSes);

		if ((FAngle(obj1) || FAngle(obj2)) &&
			(fSupp || (asp == aSqu && (obj1 == oDes || obj2 == oDes || obj1 == oNad || obj2 == oNad))))
			return false;

		// Prevent any simultaneous aspects to the North and South Node, or 
		// aspects between the nodes. North Node will always be given the 
		// priority for aspects. An aspect is prevented if there exists another
		// aspect defined in Astrolog, the sum of which totals 180 degrees.

		// This test is not foolproof, because if the user enables biquintiles
		// but disables semiquintiles, for example, he will miss one aspect.

		if (obj1 == oNoNode && obj2 == oSoNode || 
			((obj1 == oSoNode || obj2 == oSoNode) && 
			(asp >= aCon && asp <= aSQn && asp != aQui)))
			return false;
	}
	return true;
}

int GetAspectA(const double *longitude, const double *latitude,const double *vel_longitude, int i,int j, double *prOrb)
{
  int asp;
  double rAngle, rAngle3D, rDiff, rOrb, ret1a;

  // Compute the angle between the two planets.
  rAngle = MinDistance(longitude[i], longitude[j]);
  if (us.fAspect3D || us.fAspectLat)
    rAngle3D = SphDistance(longitude[i], latitude[i],longitude[i], latitude[j]);

  // Check each aspect angle to see if it applies.
  //us.nAsp=5;
  for (asp = 1; asp <= us.nAsp; asp++) 
  {
    if (!FAcceptAspect(i, asp, j))
      continue;
    rDiff = (!us.fAspect3D ? rAngle : rAngle3D) - rAspAngle[asp];
    rOrb = GetOrbA(i, j, asp);

    // If -ga switch in effect, then change the sign of the orb to correspond
    // to whether the aspect is applying or separating. To do this, check the
    // velocity vectors to see if the planets are moving toward, away, or are
    // overtaking each other.

    if (us.nAppSep == 1)
	{
      ret1a = us.nRel > rcTransit ? vel_longitude[i] : 0.0;
      rDiff *= RSgn2(vel_longitude[j]-ret1a) * RSgn2(MinDifference(longitude[i],longitude[j]));
    } 
	else if (us.nAppSep == 2)
	{
      // The -gx switch means make aspect orb signs reflect waxing/waning.
      ret1a = us.nRel > rcTransit ? vel_longitude[i] : 0.0;
      rDiff = RAbs(rDiff) * RSgn2(ret1a-vel_longitude[j]) * RSgn2(MinDifference(longitude[i], longitude[j]));
    }

    // If aspect within orb, return it.
    if (RAbs(rDiff) < rOrb) 
	{
      if (us.fAspectLat &&
        !(RAbs((!us.fAspect3D ? rAngle3D : rAngle) - rAspAngle[asp]) < rOrb))
        continue;
      if (prOrb != NULL)
        *prOrb = rDiff;
      return asp;
    }
  }
  return 0;
}

int GetDistance(const PT3R *space1, const PT3R *space2,
  const double *retlen1, const double *retlen2, int i, int j, double *prOrb)
{
  int asp;
  double dist1, dist2, rPct, rDiff, rOrb, retlen1a;

  // Compute the distances of and between the two planets.
  dist1 = PtLen(space1[i]); dist2 = PtLen(space2[j]);
  if (us.fSmartCusp && (dist1 <= 0.0 || dist2 <= 0.0))
    return 0;
  if (dist1 <= 0.0 && dist2 <= 0.0)
    rPct = 100.0;
  else
    rPct = (dist1 <= dist2 ? dist1 / dist2 : dist2 / dist1) * 100.0;

  // Check each distance aspect proportion to see if it applies.
  for (asp = 1; asp <= us.nAsp; asp++) {
    if (asp == aOpp || !FAcceptAspect(i, asp, j))
      continue;
    rDiff = rPct - (rAspAngle[asp == aCon ? aOpp : asp] / rDegHalf * 100.0);
    rOrb = GetOrb(i, j, asp);

    // If -ga switch in effect, then change the sign of the orb to correspond
    // to whether the aspect is applying or separating. To do this, check the
    // distance velocity vectors to see if the planets are moving toward,
    // away, or are overtaking each other.

    if (us.nAppSep == 1) {
      if ((us.fEphemFiles && !us.fPlacalcPla)) {
        retlen1a = us.nRel > rcTransit ? retlen1[i] : 0.0;
        rDiff *= RSgn2(retlen2[j]-retlen1a);
      } else {
        // If no distance velocity, make aspect separating.
        rDiff = RAbs(rDiff);
      }
    } else if (us.nAppSep == 2) {
      // "Waxing" is if nearer body applying, "waning" if nearer separating.
      rDiff = RAbs(rDiff) *
        (dist1 <= dist2 ? RSgn2(retlen1[i]) : RSgn2(retlen2[j]));
    }

    // If distance aspect within orb, return it.
    if (RAbs(rDiff) < rOrb) {
      if (prOrb != NULL)
        *prOrb = rDiff;
      return asp;
    }
  }
  return 0;
}


// Determine the influence of each cp0.longitude's position and aspects. Called from
// the ChartInfluence() routine for the -j chart, and the ChartEsoteric()
// routine for the -7 chart which also makes use of cp0.longitude influences.

/* Very similar to GetAspect(), this determines if there is a parallel or */
/* contraparallel aspect between the given two planets, and stores the	  */
/* result as above. The settings and orbs for conjunction are used for	  */
/* parallel and those for opposition are used for contraparallel.		  */
void GetParallel(double *planet1, double *planet2, double *planetalt1, double *planetalt2, double *ret1, double *ret2, double *altret1, double *altret2, int i, int j) 
{
	int k;
	double l, alt1, alt2, latdiri, latdirj;

	/* Planet 1 */
	l = Deg2Rad(planet1[j]);
	alt1 = Deg2Rad(planetalt1[j]);
	/* No conversion to Geo-equatorial in a heliocentric chart */
	if (us.objCenter != oSun)
		CoorXform(&l, &alt1, is.rObliquity);
	alt1 = Rad2Deg(alt1);

	/* Planet 2*/
	l = Deg2Rad(planet2[i]);
	alt2 = Deg2Rad(planetalt2[i]);
	/* No conversion to Geo-equatorial in a heliocentric chart */
	if (us.objCenter != oSun)
		CoorXform(&l, &alt2, is.rObliquity);
	/* */
	alt2 = Rad2Deg(alt2);

	grid->v[i][j] = grid->n[i][j] = 0;
	for (k = aOpp; k >= 1; k--)
	{
		if (!FAcceptAspect(i, k, j))
			continue;
		l = (k == aCon ? fabs(alt1 - alt2) : fabs(alt2) - 
			fabs(alt1));
		if (fabs(l) < GetOrb(i, j, k))
		{
			grid->n[i][j] = k;

			if (us.fAppSep)
			{
				latdiri = ret2[i] * RCosD(planet2[i]) * RSinD(Rad2Deg(is.rObliquity))
					+ altret2[i] * RCosD(Rad2Deg
					(is.rObliquity) - fabs(planetalt2[i] - alt2));
				latdirj = ret1[j] * RCosD(planet1[j]) * RSinD(Rad2Deg(is.rObliquity))
					+ altret1[j] * RCosD(Rad2Deg
					(is.rObliquity) - fabs(planetalt1[j] - alt1));
				l = RSgn2(latdirj - latdiri) * RSgn2
					(alt1 - alt2) * RSgn2(l) * fabs(l);
			}
			grid->v[i][j] = (int) (l * 60.0);
		}
	}
}
int GetParallelA(const double *longitude, const double *latitude,const double *vel_latitude, int i, int j, double *prOrb)
{
  int asp;
  double rDiff, rOrb, azi, alt1, alt2, retalt1a;

  // Compute the declination of the two planets.
  alt1 = latitude[j];
  alt2 = latitude[i];
  if (!us.fEquator2 && !us.fParallel2) 
  {
    // If have ecliptic latitude and want equatorial declination, convert.
    azi = longitude[j]; 
	CoorXform2(&azi, &alt1,Rad2Deg(is.rObliquity));
    azi = longitude[i]; 
	CoorXform2(&azi, &alt2,Rad2Deg(is.rObliquity));
  } 
  else if (us.fEquator2 && us.fParallel2) 
  {
    // If have equatorial declination and want ecliptic latitude, convert.
    azi = longitude[j]; 
	CoorXform2(&azi, &alt1,-Rad2Deg(is.rObliquity));
    azi = longitude[i]; 
	CoorXform2(&azi, &alt2,-Rad2Deg(is.rObliquity));
  }

  // Check each vertical aspect type to see if it applies.
  for (asp = 1; asp <= (!us.fDistance ? aOpp : us.nAsp); asp++)
  {
    if (!FAcceptAspect(i, asp, j))
      continue;
    if (asp == aCon)
      rDiff = alt1 - alt2;
    else if (asp == aOpp)
      rDiff = alt1 + alt2;
    else 
	{
      retalt1a = rAspAngle[asp] / rDegHalf;
      if (RAbs(alt1) > RAbs(alt2))
        alt2 /= retalt1a;
      else
        alt1 /= retalt1a;
      rDiff = ((alt1 >= 0.0) == (alt2 >= 0.0) ? alt1 - alt2 : alt1 + alt2);
    }
    rOrb = GetOrbA(i, j, asp);
    if (us.nAppSep == 1) 
	{
      if ((us.fEphemFiles && !us.fPlacalcPla)) 
	  {
        retalt1a = us.nRel > rcTransit ? vel_latitude[j] : 0.0;
        rDiff *= RSgn2(retalt1a - vel_latitude[i]);
      } 
	  else 
	  {
        // If no declination velocity, make aspect separating.
        rDiff = RAbs(rDiff);
      }
    } 
	else if (us.nAppSep == 2) 
	{
      // "Waxing" is if bodies on same side, "waning" if on different sides.
      // (This means Parallel is always waxing, Contraparallel always waning.)
      rDiff = RAbs(rDiff) * ((alt1 >= 0.0) == (alt2 >= 0.0) ? -1.0 : 1.0);
    }

    // If vertical aspect within orb, return it.
    if (RAbs(rDiff) < rOrb) 
	{
      if (prOrb != NULL)
        *prOrb = rDiff;
      return asp;
    }
  }
  return 0;
}

// Convert 3D rectangular to spherical coordinates.
void RecToSph3(double rx, double ry, double rz, double *azi, double *alt)
{
  double ang, rad;

  RecToPol(rx, ry, &ang, &rad);
  *azi = DFromR(ang);
  ang = RAngleD(rad, rz);
  // Ensure latitude is from -90 to +90 degrees.
  if (ang > rDegHalf)
    ang -= rDegMax;
  *alt = ang;
}

void SphRatio(double lon1, double lat1, double lon2, double lat2, double rRatio,double *lon, double *lat)
{
  double x1, y1, z1, x2, y2, z2, x, y, z, len, ang, adj, ang2;

  SphToRec(1.0, lon1, lat1, &x1, &y1, &z1);
  SphToRec(1.0, lon2, lat2, &x2, &y2, &z2);
  if (rRatio != 0.5){
    // Bisecting an arc is easy, however other proportions require extra math.
    len = RLength3(x2 - x1, y2 - y1, z2 - z1) / 2.0;
    ang = RAsinD(len);
    adj = 1.0 / RTanD(ang);
    rRatio = (rRatio - 0.5) / 0.5;
    ang2 = rRatio * ang;
    rRatio = adj * RTanD(ang2);
    rRatio = (rRatio / 2.0) + 0.5;
  }
  x = x1 + (x2 - x1) * rRatio;
  y = y1 + (y2 - y1) * rRatio;
  z = z1 + (z2 - z1) * rRatio;
  RecToSph3(x, y, z, lon, lat);
}

bool FCreateGridA(bool fFlip)
{
  int x, y, k, asp;
  double l, rOrb=0.0, rT=0.0;

  is.nObj=42;
  if (!FEnsureGrid())
    return false;
  ClearB((pbyte)grid, sizeof(GridInfo));

  for (y = 0; y <= is.nObj; y++) if (!ignore1[y])
  {
    for (x = 0; x <= is.nObj; x++) if (!ignore1[x])
	{
      if (fFlip ? x > y : x < y) 
	  {
        if (us.fParallel)
          asp = GetParallelA(cp0.longitude, cp0.latitude, cp0.vel_latitude, x, y, &rOrb);
        else if (us.fDistance)   
          asp = GetDistance(cp0.pt, cp0.pt, cp0.vel_distance, cp0.vel_distance, x, y, &rOrb);
        else
          asp = GetAspectA(cp0.longitude, cp0.latitude, cp0.vel_longitude, x, y,&rOrb);
        grid->n[x][y] = asp;
        grid->v[x][y] = asp > 0 ? rOrb : 0.0;
      } 
	  else if (fFlip ? x < y : x > y)
	  {
        // Calculate midpoint in 2D or 3D.
        if (!us.fHouse3D)
          l = Mod(Midpoint(cp0.longitude[x], cp0.longitude[y]));
        else
          SphRatio(cp0.longitude[x], cp0.latitude[x], cp0.longitude[y], cp0.latitude[y], 0.5,
            &l, &rT);
        k = SFromZ(l);
        grid->n[x][y] = k;
        grid->v[x][y] = l - (double)((k-1)*30);
      } 
	  else 
	  {
        l = cp0.longitude[y];
        k = SFromZ(l);
        grid->n[x][y] = k;
        grid->v[x][y] = l - (double)((k-1)*30);
      }
	}
  }
  return true;
}


/* Fill in the aspect grid based on the aspects taking place among the */
/* planets in the present chart. Also fill in the midpoint grid.	   */
bool FCreateGrid(bool fFlip)
{
	int i, j, k;
	double l;

	if (!FEnsureGrid())
		return false;

	CP cpBak;

	if( UsePDsInChart && IsPDsChartWithoutTable )
	{
		cpBak = cp0;
		cp0=cpPDs;
	}

	memset(&grid->n, 0, sizeof(grid->n));

	for (j = 0; j <= cObj; j++)
	{
		if (!FIgnore(j))
		{
			for (i = 0; i <= cObj; i++)
			{
				if (!FIgnore(i))
				{

					/* The parameter 'flip' determines what half of the grid is filled in */
					/* with the aspects and what half is filled in with the midpoints.	  */

					if (fFlip ? i > j : i < j)
					{
						if (us.fParallel)
							GetParallel(cp0.longitude, cp0.longitude, cp0.latitude, cp0.latitude, cp0.vel_longitude, cp0.vel_longitude, 
								cp0.vel_latitude, cp0.vel_latitude, i, j);
						else
							GetAspect(cp0.longitude, cp0.longitude, cp0.vel_longitude, cp0.vel_longitude, i, j);
					}
					else if (fFlip ? i < j : i > j)
					{
						l = Mod(Midpoint(cp0.longitude[i], cp0.longitude[j]));
						k = (int) l;				/* Calculate */
						grid->n[i][j] = k / 30 + 1; /* midpoint. */
						grid->v[i][j] = (int) ((l - (double) (k / 30) * 30.0) * 60.0);
					}
					else
					{
						grid->n[i][j] = Z2Sign(cp0.longitude[j]);
						grid->v[i][j] = (int) (cp0.longitude[j] - (double) (grid->n[i][j] - 1) * 30.0);
					}
				}
			}
		}
	}

    //if( (UsePDsInChart && IsPDsChartWithoutTable) || UsePDsInChart )
	//	cp1=cpBak;
    if( UsePDsInChart && IsPDsChartWithoutTable )
		cp0=cpBak;

	return true;
}

/* Set up the aspect/midpoint grid. Allocate memory for this array, if not */
/* already done. Allocation is only done once, first time this is called.  */
/* This is a subprocedure of FCreateGrid() and FCreateGridRelation().	*/
/* Given two planets, determine what aspect, if any, is present between */
/* them, and save the aspect name and orb in the specified grid cell.	*/
void GetAspect(double *planet1, double *planet2, double *ret1, double *ret2, int i, int j) 
{
	int k;
	double l, m;

	grid->v[i][j] = grid->n[i][j] = 0;
	l = MinDistance(planet2[i], planet1[j]);

	for (k = cAspect; k >= 1; k--)
	{
		if (!FAcceptAspect(i, k, j))
			continue;
		m = l - rAspAngle[k];

		if (fabs(m) < GetOrb(i, j, k))
		{
			grid->n[i][j] = k;

			/* If -ga switch in effect, then change the sign of the orb to	  */
			/* correspond to whether the aspect is applying or separating.	  */
			/* To do this, we check the velocity vectors to see if the		  */
			/* planets are moving toward, away, or are overtaking each other. */

			if (us.fAppSep)
				m = RSgn2(ret1[j] - ret2[i]) * RSgn2(MinDifference(planet2[i], 
				planet1[j])) * RSgn2(m) * fabs(m);
			grid->v[i][j] = (int) (m * 60.0);
		}
	}
}
void ComputeAspects(void)
{
	int i, j;
	bool fFlip = false; // 根据原始代码中的逻辑设定
	
	// 确认当前生成的网格是否存在，如果不存在则创建
	if (!FEnsureGrid())
		return;
	
	// 网格初始化
	ClearB((pbyte)grid, sizeof(GridInfo)); // 这个函数在其他地方定义
	
	for (j = 0; j <= is.nObj; j++) {
		if (!FIgnore(j)) {
			for (i = 0; i <= is.nObj; i++) {
				if (!FIgnore(i)) {
					// 参数 'flip' 决定网格的哪一半存储相位关系
					// 哪一半存储中点(midpoint)
					if (fFlip ? i > j : i < j) {
						if (us.fParallel)
							GetParallel(cp0.longitude, cp0.longitude, cp0.latitude, cp0.latitude,
								cp0.vel_longitude, cp0.vel_longitude,
								cp0.vel_latitude, cp0.vel_latitude, i, j);
						else
							GetAspect(cp0.longitude, cp0.longitude, cp0.vel_longitude, cp0.vel_longitude, i, j);
					}
					else if (fFlip ? i < j : i > j) {
						double l = Mod(Midpoint(cp0.longitude[i], cp0.longitude[j]));
						int k = (int)l;              // 中点计算
						grid->n[i][j] = k / 30 + 1;  // 中点
						grid->v[i][j] = (int)((l - (double)(k / 30) * 30.0) * 60.0);
					}
					else {
						grid->n[i][j] = Z2Sign(cp0.longitude[j]);
						grid->v[i][j] = (int)(cp0.longitude[j] - (double)(grid->n[i][j] - 1) * 30.0);
					}
				}
			}
		}
	}
}