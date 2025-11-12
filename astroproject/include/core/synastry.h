#ifndef SYNASTRY_H
#define SYNASTRY_H

#include "../utils/utils.h"

/* Relationship chart modes */
typedef enum
{
	rcMarks2 = -12,
	rcMarks1,
	rcHarm,
	rcHarm2,
	rcSolarret,
	rcLunarret,
	rcProgTran,
	rcProgress,
	rcTransit,
	rcQuadWheel,
	rcTriWheel,
	rcDual,
	rcNone,			// 0
	rcSynastry,
	rcComposite,
	rcMidpoint,
	rcDifference,
	rcBiorhythm,	// 5
} relat_chart_ty;

void CastSynastry(bool fDate);

#endif // SYNASTRY_H