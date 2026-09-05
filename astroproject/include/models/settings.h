#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdio.h>
// User settings structure
typedef struct UserSettings
{
	/* Chart types */
	bool fListing;				/* -v */
	bool fWheel;				/* -w */
	bool fGrid;					/* -g */
	bool fAspList;				/* -a */
	bool fMidpoint;				/* -m */
	bool fHorizon;				/* -Z */
	bool fOrbit;				/* -S */
	bool fSector;				/* -l */
	bool fInfluence;			/* -j */
	bool fAstroGraph;			/* -L */
	bool fCalendar;				/* -K */
	bool fInDay;				/* -d */
	bool fInDayInf;				/* -D */
	bool fEphemeris;			/* -E */
	bool fTransit;				/* -t */
	bool fTransitInf;			/* -T 16*/
	bool fFirdaria;
	bool fPlanetaryHours;
	bool fSquareChart;
	bool fAlmtsPoint;
	bool fAlmtsChart;
	bool fPrimDirs;
	bool fProfection;
	bool fJyotish;
	bool fPrimDirs2;
	bool fPrimDirs3;
	/* Chart suboptions */
	bool fVelocity;				/* -v0 27*/
	bool fWheelReverse;			/* -w0 */
	bool fGridConfig;			/* -g0 */
	bool fAppSep;				/* -ga */
	bool fParallel;				/* -gp */
	bool fAspSummary;			/* -a0 */
	bool fMidSummary;			/* -m0 */
	bool fMidAspect;			/* -ma */
	bool fPrimeVert;			/* -Z0 */
	bool fHorizonSearch;		/* -Zd */
	bool fSectorApprox;			/* -l0 */
	bool fInfluenceSign;		/*38 -j0 */
	bool fLatitudeCross;		/* -L0 */
	bool fCalendarYear;			/* -K  */
	bool fInDayMonth;			/* -dm */
	bool fArabicFlip;			/* -P0 42*/

	/* Table chart types */
	bool fSwitch;				/* -H  */
	bool fSwitchRare;			/* -Y  */
	bool fKeyGraph;				/* -HX */
	bool fSign;					/* -HC */
	bool fObject;				/* -HO */
	bool fAspect;				/* -HA */
	bool fConstel;				/* -HF */
	bool fOrbitData;			/* -HS */
	bool fMeaning;				/* -HI */
	bool fPrintProgramStatus;
	bool fDebugFont;
	bool fSolarEclipse;
	bool fLunarEclipse;
	bool fLunarPhases;
	bool f12parts;
	bool f12parts2;
	bool fAnti;
	bool fAnti2;
	bool fNavamsas2;

	/* Main flags */
	bool fSidereal;			/* -s 57*/
	bool fProgressUS;		/* Are we doing a -p progressed chart?           */
	bool fInterpret;		/* Is -I interpretation switch in effect?        */
	bool fDecan;			/* -3 */
	bool fFlip;				/* -f */
	bool fGeodetic;			/* -G */
	bool fVedic;			/* -J */
	bool fNavamsa;			/* -9 */
	bool fWriteFile;		/* -o */
	bool fAnsiColor;		/* -k */
	bool fGraphics;			/* -X 67*/

	/* Main subflags */
	bool fNoSwitches;
	bool fSeconds;			/* -b0 */
	bool fEquator;			/* -sr                      */
	bool fSolarArc;			/* -p0, etc */
	bool fWritePos;			/* -o0 */
	bool fAnsiChar;			/* -k0 */

	/* Rare flags */
	bool fTrueNode;		/* -U 恒星显示 (A4)：启用真实恒星计算与输出 */			/* -Yn */
	bool fEuroDate;			/* -Yd */
	bool fEuroTime;			/* -Yt */
	bool fSmartAspects;		/* -YC */
	bool fClip80;			/* -Y8 */
	bool fHouseAngle;		/* -Yc */
	bool fIgnoreSign;		/* -YR0 */
	bool fIgnoreDir;		/* -YR0 */
	bool fNoWrite;			/* -0o */
	bool fNoRead;			/* -0i */
	bool fNoQuit;			/* -0q */
	bool fNoGraphics;		/* -0X */

	/* Value settings */
	int nEphemYears;			/* -Ey */
	int nArabic;				/* -P */
	int nRel;					/* What relationship chart are we doing, if any? */
	int nHouseSystem;			/* -c */
	int nHouse3D;				/* -c3 */
	int objCenter;				/* -h */
	int nStar;					/* -U */

	//////////////////////////////////////////////////////////
	int nTerms;
	int nFace;
	int nTriplicities;
	int nDode;
	//////////////////////////////////////////////////////////

	double nHarmonic;			/* Harmonic chart value passed to -x switch.     */
	int objOnAsc;				/* House value passed to -1 or -2 switch.        */
	int dayDelta;				/* -+, -- */
	int nDegForm;				/* -s */
	int nDivision;				/* -d */
	int nScreenWidth;			/* -I */
	int nSiderealMode;			/* -sm */
	double dstDef;				/* -z0 */
	double zonDef;				/* -z  */
	double lonDef;				/* -zl  */
	double latDef;				/* -zl  */
	wchar_t szLocNameDef[120];	/* -zn */
	bool useInternationalAtlas;	/*#UseInternationalAtlas*/
	bool advancedMode;			/* #AdvancedMode or #SimplifiedMode*/
	bool omitWizard;			/* #OmitWizard */
	bool omitRegionalWizard;	/* #OmitRegionalWizard */

	/* Value subsettings */
	int nWheelRows;				/* Number of rows per house to use for -w wheel. */
	int nAstroGraphStep;		/* Latitude step rate passed to -L switch.       */
	int nArabicParts;			/* Arabic parts to include value passed to -P.   */
	double rSiderealCorrection;	/* Position shifting value passed to -s switch
								   used in sidereal charts only.				 */
	double rProgDay;			/* Progression day value passed to -pd switch.   */
	int nRatio1;				/* Chart ratio factors passed to -rc or -rm.     */
	int nRatio2;
	int nScrollRow;				/* -YQ */
	int nArabicNight;			/* -YP */
	int nBioday;				/* -Yb */
	int fSmartSave;
	int fHouse3D;				/* -c3 */
	bool fDifference;
	bool fSphere;
	double rZodiacOffset;		/* Position shifting value passed to -s switch.  */
	int nTriplicities2;
	bool fTopocentric;
	bool fEphemFiles;
	bool fPlacalcPla;
	double altDef;
	double Nutation;
	int nAtlasList;				/* Number of rows to display value passed to -N. */
	int fEuroDist;				/* -Yv */
	int nAstroGraphDist;		/* Maximum crossing distance passed to -L0 switch. */
	bool fGridMidpoint;
	bool fGraphAll;
	bool fIndian;
	bool fSmartCusp;
	int objRequire;
	bool fEclipse;
	bool fEclipseAny;
	bool fDistance;
	int nAppSep;
	bool fZodRel;
	bool fDecennials;
	bool fHouseLord;
	bool fEmptyASP;
	bool fListDecan;
	bool fEquator2;
	bool fParallel2;
	int nAsp;
	bool fAspect3D;
	bool fAspectLat;
	int nAspectSort;
	int nSignDiv;
	int nCharset;
	bool fMoonMove;
	
	// 添加缺失的字段
	int objNode;        /* -Yn */
	int objOnLilith;    /* -Yl */
	bool fAllStar;		/* -U 恒星显示 (A4)：启用真实恒星计算与输出（默认 false = 占位语义） */
} US;

// Internal settings structure
typedef struct InternalSettings
{
	bool fProgressIS;			/* Are we doing a chart involving progression?       */
	bool fReturn;				/* Are we doing a transit chart for returns?         */
	int fMult;					/* Have we already printed at least one text chart?  */
	bool fSeconds;				/* Do we print locations to nearest second?          */
	bool fSzInteract;			/* Are we in middle of chart so some setting fixed?  */
	bool fNoEphFile;			/* Have we already had a ephem file not found error? */
	char *szProgName;			/* The name and path of the executable running.      */
	wchar_t *szFileScreen;		/* The file to send text output to as passed to -os. */
	wchar_t *szFileOut;			/* The output chart filename string as passed to -o. */
	char **rgszComment;			/* Points to any comment strings after -o filename.  */
	int cszComment;				/* The number of strings after -o that are comments. */
	int cchCol;					/* The current column text charts are printing at.   */
	int cchRow;					/* The current row text charts have scrolled to.     */
	double rSid;				/* Sidereal offset degrees to be added to locations. */
	double JD;					/* Fractional Julian day for current chart.          */
	double JDp;					/* Julian day that a progressed chart indicates.     */
	FILE *S;					/* File to write text to.   */
	double T;					/* Julian time for chart (expressed as number of	 */
								/* Julian centuries since noon GMT 1/Jan/1900)		 */
	double MC;					/* Midheaven at chart time. */
	double Asc;					/* Ascendant at chart time. */
	double RAa;					/* Right ascension at time. */
	double rObliquity;			/* Obliquity of ecliptic.   */
	double latMC;
	double lonMC;
	double rSidPD;
	double OB;					/* Obliquity of ecliptic.   */
	double Vtx;					/* Vertex at chart time. */
	int nObj;
} IS;

typedef struct _GraphicsSettings
{
	bool fBitmap;				/* Are we creating a bitmap file (-Xb set).         */
	bool fPS;					/* Are we generating a PostScript file (-Xp set).   */
	bool fMeta;					/* Are we generating a metafile graphic (-XM set).  */
	bool fColor;				/* Are we drawing a color chart (-Xm not set).      */
	bool fInverse;				/* Are we drawing in reverse video (-Xr set).       */
	bool fRoot;					/* Are we drawing on the X11 background (-XB set).  */
	bool fText;					/* Are we printing chart info on bottom of graphics chart (-Xt set).   */
	bool fFont;					/* Are we simulating fonts in charts (-XM0 set).    */
	bool fBonusMode;			/* Are we drawing in alternate mode (-Xi set).      */
	bool fBorder;				/* Are we drawing borders around charts (-Xu set).  */
	bool fLabel;				/* Are we labeling objects in charts (-Xl not set). */
	bool fJetTrail;				/* Are we not clearing screen on updates (-Xj set). */
	bool fMouse;				/* Are we not considering PC mouse inputs.          */
	bool fConstel;				/* Are we drawing maps as constellations (-XF set). */
	bool fMollewide;			/* Are we drawing maps scaled correctly (-XW0 set). */
	bool fPrintMap;				/* Are we printing globe names on draw (-XP0 set).  */
	int xWin;					/* Current size of graphic chart (-Xw).      */
	int yWin;
	int nAnimationJumpRate;		/* Current animation rate (-Xn).			*/
	int nScale;					/* Current character scale factor (-Xs).     */
	int objLeft;				/* Current object to place on Asc (-X1).     */
	int nTextRows;				/* Numb. of rows to set text screen to (-V). */
	int nRot;					/* Current rotation degree of globe.         */
	double rTilt;					/* Current vertical tilt of rotating globe.  */
	char chBmpMode;				/* Current bitmap file type (-Xb).           */
	int nOrient;				/* PostScript paper orientation indicator.   */
	double xInch;					/* PostScript horizontal paper size inches.  */
	double yInch;					/* PostScript vertical paper size inches.    */
	char *szDisplay;			/* Current X11 display name (-Xd).           */
	int nGridCell;				/* Number of cells in -g grids (-Yg).        */
	int nGlyphs;				/* Settings for what gylphs to use (-YXG).    */
	bool fEcliptic;
	double rRot;        /* Current rotation degree of globe.         */
	int nDecaType;    /* Type of wheel chart decoration (-YXv).    */
	int nDecaSize;    /* Size of wheel chart decoration (-YXv).    */
	int nDecaLine;    /* Lines in wheel chart decoration (-YXv).   */
	double rspace;      /* Radius in AU of -S orbit chart (-YXS).    */
	int cspace;       /* Number of -S orbit trails allowed (-YXj). */
	int zspace;       /* Height diff of each orbit trail (-YXj0).  */
	int nRayWidth;    /* Column width in -7 esoteric chart (-YX7). */
	int fColorSign;  /* More color for sign boundaries. (-YXk).   */
	int fColorHouse; /* More color for house boundaries. (-YXk0). */
	int nDashMax;     /* Maximum dash allowed for lines (-YXA).    */
	char *szStarsLin; /* Names of extra stars for linking (-YXU).  */
	char *szStarsLnk; /* Indexes of star pairs to link up (-YXU).  */
	int fSouth;      /* Are we focus on south hemisphere (-XX0/XP0 set). */
	int fAlt;        /* Are we drawing in alternate mode (-Xi set).      */
	int fHouseExtra; /* Are we showing additional house info (-XC set).  */
	int fEquator;     /* -sr */
	int charSet;					/* character set for TTF fonts */
	int nDecaFill;     // Fill method for wheel chart sections (-Xv).
	bool fLabelAsp;
	bool fAllStar;
	int nFontHou;
	int rBackPct;
	bool fIndianWheel;
}
GS;

typedef struct _WindowInternal
{
	//HINSTANCE hinst;			/* Instance of the Astrolog window class.    */
	//HWND hwndMain;				/* The outer created frame window.           */
	//HWND hwnd;					/* The current window being dealt with.      */
	//HWND hwnd2;					/* The current window being dealt with.      */
	//HMENU hmenu;				/* The Astrolog main menu bar.               */
	//HACCEL haccel;				/* Keyboard accelerator or shortcut table.   */
	//HDC hdc;					/* The current DC bring drawn upon.          */
	//HDC hdc2;					/* The current DC bring drawn upon.          */
	//HDC hdcPrint;				/* The current DC being printed upon.        */
	//HWND hwndAbort;				/* Window of the printing abort dialog.      */
	//HPEN hpen;					/* Pen with the current line color.          */
	//HBRUSH hbrush;				/* Fill if any with the current color.       */
	//HFONT hfont;				/* Font of current text size being printed.  */
	int nTimer;				/* Identifier for the animation timer.       */
	int xScroll;				/* Horizontal & vertical scrollbar position. */
	double yScroll;
	int xClient;				/* Horizontal & vertical window size.		 */
	int yClient;
	int xChar;					/* Horizontal & vertical font character size.*/
	int yChar;
	int xMouse;					/* Horitontal & vertical mouse position.	 */
	int yMouse;
	unsigned short wCmd;					/* The currently invoked menu command.        */
	int nMode;					/* New chart type to switch to if any.        */
	bool fMenu;					/* Do we need to repaint the menu bar?        */
	bool fMenuAll;				/* Do we need to redetermine all menu checks? */
	bool fRedraw;				/* Do we need to redraw the screen?           */
	bool fCast;					/* Do we need to recast the chart positions?  */
	bool fAbort;				/* Did the user cancel printing in progress?  */
	int nDlgChart;				/* Which chart to set in Open or Info dialog. */
	bool fInitProgression;		/* hack to set chart data at beginning of progression,
									to make animation possible */

									/* Window User settings. */
	bool fAnimate;				/* Animation runs if true */
	bool fDelayScreenRedraw;	/* Are we drawing updates off screen?     */
	bool fHourglass;			/* Bring up hourglass cursor on redraws?  */
	bool fChartWindow;			/* Does chart change cause window resize? */
	bool fWindowChart;			/* Does window resize cause chart change? */
	bool fNoUpdate;				/* Do we not automatically update screen? */
	//KI kiPen;					/* The current pen scribble color.        */
	int nAnimationJumpFactor;	/* Time increases if positive, decreases if negative  */
	int nTimerDelay;			/* Milliseconds between animation draws.  */
	int xPrint;					/* Hor. and ver. resolution for printing  */
	int yPrint;
	bool fPrinting;				/* true if we are printing				  */
	bool fDisableMacroShortcuts; /*true if menu shortcuts for macros should be hidden on update*/
	//HFONT h;
	//HFONT m_hFont;
	//HFONT m_hFont2;
	unsigned short lid;
	bool chs;
	//HFONT m_hFontv;
	long lParamRC; /* Coordinates where right click originated.  */
	bool fMoved;
	bool fSaverRun;
}
WI;
#endif // SETTINGS_H