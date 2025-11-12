#ifndef OPTIONS_H
#define OPTIONS_H

#define cColor     16
#define RGBVal(r, g, b) \
  ((unsigned long)(0xff000000 | ((unsigned long)(r) << 16) | ((unsigned long)(g) << 8) | (unsigned long)(b)))
extern WI wi;
extern US us;
class Options
{
public:
	int FIXSTARS;
	int ANTIS;
	int CANTIS;

	int DODECATEMORIA;

	int MOON;
	int ABOVEHOR;
	int ABOVEHORNATAL;

	bool	aspects;
	bool	aspect[11];
	bool	symbols;
	bool	traditionalaspects;
	bool	houses;
	bool	positions;
	bool	intables;
	bool	bw;
	int	theme;
	int	ascmcsize;
	double	tablesize;
	bool	planetarydayhour;
	bool	housesystem;
	bool	transcendental[3];
	bool	shownodes;
	bool	aspectstonodes;
	bool	showlof;
	bool	showaspectstolof;
	bool	showterms;
	bool	showdecans;
	int showfixstars;
	bool	showfixstarsnodes;
	bool    showfixstarshcs;
	bool showfixstarslof;
	bool topocentric;
	bool usetradfixstarnamespdlist;
	bool netbook;

	//#AppearanceII
	bool	speculums[2][14];
	bool	intime;

	//#Symbols
	bool uranus;
	int pluto;
	bool signs;

	//#Dignities(planets, domicile, exaltatio)
						//#Sun
	bool dignities[10][2][12];

	//#Minor dignities
	//#Triplicities
	int	seltrip;
	int	trips[3][4][3];

	//#Terms
	int	selterm;

	int	terms[2][12][5][2];

	//#Decans
	int	seldecan;
	int	decans[2][12][3];

	//#ChartAlmuten
	bool oneruler;
	bool usedaynightorb;
	int dignityscores[5];
	bool useaccidental;
	int housescores[12];
	int sunphases[3];
	int dayhourscores[2];
	bool  useexaltationmercury;

	//#TopicalAlmuten && Parts
	int topicals;
	//#Arabic Parts
	int	arabicpartsref;
	int	daynightorbdeg;
	int	daynightorbmin;
	int	arabicparts;

	//#Ayanamsha
//int	ayanamsha;

	//#Colors


	//#Housesystem
	int hsys;

	//#Nodes
	bool meannode;

	//#Orbis
	double	orbis[11][11];
	double	orbisplanetspar[11][2];

	//# Houses 
	double	orbisH[11];
	double	orbisparH[2];//#parallel/contraparallel
	double	orbiscuspH;

	//# Asc,MC
	double	orbisAscMC[11];
	double	orbisparAscMC[2];
	double	orbiscuspAscMC;

	double	exact;

	//#Primary Dirs
	int primarydir;// = PLACIDIANSEMIARC
	int subprimarydir;// = MUNDANE
	int subzodiacal;// = SZNEITHER
	bool bianchini;

	bool	sigascmc[2];
	bool	sighouses;
	bool	sigplanets[12];
	bool	promplanets[16];
	bool	pdaspects[11];
	bool	pdmidpoints;
	bool	pdparallels[2];
	bool	pdsecmotion;
	int	pdsecmotioniter;

	bool	zodpromsigasps[2];
	bool	ascmchcsasproms;// Asc, MC as promissors

	bool	pdfixstars;
	bool	pdfixstarssel[30];
	bool	pdlof[2];

	bool	pdsyzygy;

	bool	pdterms;

	bool	pdantiscia;
	bool pdcustomer;
	int	pdcustomerlon[3];
	int		pdcustomerlat[3];
	bool pdcustomersouthern;

	bool pdcustomer2;
	int	pdcustomer2lon[3];
	int		pdcustomer2lat[3];
	bool pdcustomer2southern;

	//#PD-keys
	bool	pdkeydyn;
	int	pdkeyd;
	int	pdkeys;
	int	pdkeydeg;
	int	pdkeymin;
	int	pdkeysec;
	bool	useregressive;

	//#Lot of Fortune
	int	lotoffortune;

	//#Syzygy
	int syzmoon;

	//#'''total30     昂宿六       毕宿五       大陵五      天大将军一     心宿二       大角星       鬼宿四      鬼宿三       摇光        参宿四      南门二       老人星      北河二      五帝座一     北落师门      贯索四      室宿一      奎宿九      北极星       北河三      鬼(宿)星团  南河三      轩辕十四     参宿七      天狼星        角宿一      天市右垣七   织女一      氐宿一         氐宿四'''
	char fixstars[30][10];// = {'etTau':1.5, 'alTau':1.5, 'bePer':1.5, 'ga-1And':1.5, 'alSco':1.5, 'alBoo':1.5, 'deCnc':1.5, 'gaCnc':1.5, 'etUMa':1.5, 'alOri':1.5, 'alCen':1.5, 'alCar':1.5, 'alGem':1.5, 'beLeo':1.5, 'alPsA':1.5, 'alCrB':1.5, 'alPeg':1.5, 'beAnd':1.5, 'alUMi':1.5, 'beGem':1.5, 'M44':1.5, 'alCMi':1.5, 'alLeo':1.5, 'beOri':1.5, 'alCMa':1.5, 'alVir':1.5, 'alSer':1.5, 'alLyr':1.5, 'al-2Lib':1.5, 'beLib':1.5}


	//#Profections
	bool zodprof;
	bool usezodprojsprof;

	bool isfirbonatti;

	char deflocname[260];
	bool deflocplus;
	int defloczhour;
	int defloczminute;
	bool deflocdst;
	int  defloclondeg;
	int  defloclonmin;
	int  defloclatdeg;
	int  defloclatmin;
	bool defloceast;
	bool deflocnorth;
	int 	 deflocalt;

	//##PDsInChart
	int  pdincharttyp;
	bool pdinchartsecmotion;

	bool pdinchartterrsecmotion;

	//#Languages
	int  langid;

	bool	autosave;
	long rgbbmp[16];
	int kAspA[19];
	int kElemA[4];

	char myBuff[64];
	int size;
	int cWeight;
	unsigned long  bItalic;

	char	optionsfilestxt[25][120];
	char	optsdirtxt[120];// = 'Opts'
	Options() {};
	bool step12;
	bool mainsigs;
	bool FROMPDSSETTING;
	bool USEBIRTH;
	bool fEquator;
	bool hasToolbar;
	bool bStatusBar;
	bool fInverse;
	bool UseAspectGrid;
	bool HarmonicChartsYearsMode;
	double ayanamsha;
	bool NoNutation;
	bool NoPrecession;
	bool AddNutation;
	bool IsChaldean;
	int Dignities;
	bool IsPlanetModern;
	bool IsPlanetTraditional;
	bool IsPlanetLuminaries;
	int orbs;
	int progdirrestric;
	int UseTheme;
	bool zregyptian;
	bool zr27cap;
	int ZodRelsign;
	int ZodRelperiod;
	int PlanetOrder;
	int StartWith;
	bool sectptolemy;
	void OptionsInit()
	{
		USEBIRTH = true;
		FROMPDSSETTING = false;
		step12 = true;
		mainsigs = false;
		FIXSTARS = 1;
		ANTIS = 2;
		CANTIS = 3;
		DODECATEMORIA = 4;
		MOON = 0;
		ABOVEHOR = 1;
		ABOVEHORNATAL = 2;
		aspects = true;

		bool aspect1[11] = { true, false, false, true, false, true, true, false, false, false, true };
		memcpy(aspect, aspect1, sizeof(aspect1));
		symbols = true;
		traditionalaspects = false;
		houses = true;
		positions = false;
		intables = false;
		bw = false;
		theme = 0;
		ascmcsize = 5;
		tablesize = 0.75;
		planetarydayhour = true;
		housesystem = true;
		transcendental[0] = transcendental[1] = transcendental[2] = true;

		shownodes = true;
		aspectstonodes = false;
		showlof = true;
		showaspectstolof = false;
		showterms = false;
		showdecans = false;
		showfixstars = 0;
		showfixstarsnodes = false;
		showfixstarshcs = false;
		showfixstarslof = false;
		topocentric = false;
		usetradfixstarnamespdlist = false;
		netbook = false;

		bool	speculums1[2][14] = {
			{true, true, true, true, false, false, true, true, false, false, true, false, false, false},
			{true, true, true, true, false, false, false, false, false, false, false,  false, true, true}
		};
		memcpy(speculums, speculums1, sizeof(speculums1));

		intime = false;
		(void)intime;
		uranus = true;
		pluto = 0;
		signs = true;
		bool dignities1[10][2][12] = {
			{{false, false, false, false, true, false, false, false, false, false, false, false}, {true, false, false, false, false, false, false, false, false, false, false, false}},
			{{false, false, false, true, false, false, false, false, false, false, false, false}, {false, true, false, false, false, false, false, false, false, false, false, false}},
			{{false, false, true, false, false, true, false, false, false, false, false, false}, {false, false, false, false, false, true, false, false, false, false, false, false}},
			{{false, true, false, false, false, false, true, false, false, false, false, false}, {false, false, false, false, false, false, false, false, false, false, false, true}},
			{{true, false, false, false, false, false, false, true, false, false, false, false}, {false, false, false, false, false, false, false, false, false, true, false, false}},
			{{false, false, false, false, false, false, false, false, true, false, false, true}, {false, false, false, true, false, false, false, false, false, false, false, false}},
			{{false, false, false, false, false, false, false, false, false, true, true, false}, {false, false, false, false, false, false, true, false, false, false, false, false}},
			{{false, false, false, false, false, false, false, false, false, false, false, false}, {false, false, false, false, false, false, false, false, false, false, false, false}},
			{{false, false, false, false, false, false, false, false, false, false, false, false}, {false, false, false, false, false, false, false, false, false, false, false, false}},
			{{false, false, false, false, false, false, false, false, false, false, false, false}, {false, false, false, false, false, false, false, false, false, false, false, false}} };

		memcpy(dignities, dignities1, sizeof(dignities1));

		seltrip = 0;

		int trips1[3][4][3] = { {{0, 5, 6},{6, 2, 5},{3, 4, 1},{3, 1, 4}},
						{{0, 5, 7},{6, 2, 7},{4, 4, 7},{3, 1, 7}},
						{{0, 4, 5},{6, 3, 2},{5, 1, 4},{2, 6, 3}} };
		memcpy(trips, trips1, sizeof(trips1));

		selterm = 0;

		int terms1[2][12][5][2] = { {{{5, 6}, {3, 6}, {2, 8}, {4, 5}, {6, 5}},
					{{3, 8}, {2, 6}, {5, 8}, {6, 5}, {4, 3}},
					{{2, 6}, {5, 6}, {3, 5}, {4, 7}, {6, 6}},
					{{4, 7}, {3, 6}, {2, 6}, {5, 7}, {6, 4}},
					{{5, 6}, {3, 5}, {6, 7}, {2, 6}, {4, 6}},
					{{2, 7}, {3, 10}, {5, 4}, {4, 7}, {6, 2}},
					{{6, 6}, {2, 8}, {5, 7}, {3, 7}, {4, 2}},
					{{4, 7}, {3, 4}, {2, 8}, {5, 5}, {6, 6}},
					{{5, 12}, {3, 5}, {2, 4}, {6, 5}, {4, 4}},
					{{2, 7}, {5, 7}, {3, 8}, {6, 4}, {4, 4}},
					{{2, 7}, {3, 6}, {5, 7}, {4, 5}, {6, 5}},
					{{3, 12}, {5, 4}, {2, 3}, {4, 9}, {6, 2}}},
					{{{5, 6}, {3, 8}, {2, 7}, {4, 5}, {6, 4}},
					{{3, 8}, {2, 7}, {5, 7}, {6, 2}, {4, 6}},
					{{2, 7}, {5, 6}, {3, 7}, {4, 6}, {6, 4}},
					{{4, 6}, {5, 7}, {2, 7}, {3, 7}, {6, 3}},
					{{5, 6}, {2, 7}, {6, 6}, {3, 6}, {4, 5}},
					{{2, 7}, {3, 6}, {5, 5}, {6, 6}, {4, 6}},
					{{6, 6}, {3, 5}, {2, 5}, {5, 8}, {4, 6}},
					{{4, 6}, {3, 7}, {5, 8}, {2, 6}, {6, 3}},
					{{5, 8}, {3, 6}, {2, 5}, {6, 6}, {4, 5}},
					{{3, 6}, {2, 6}, {5, 7}, {6, 6}, {4, 5}},
					{{6, 6}, {2, 6}, {3, 8}, {5, 5}, {4, 5}},
					{{3, 8}, {5, 6}, {2, 6}, {4, 5}, {6, 5}}} };
		memcpy(terms, terms1, sizeof(terms1));

		seldecan = 0;

		int decans1[2][12][3] = { {{4, 0, 3},
						{2, 1, 6},
						{5, 4, 0},
						{3, 2, 1},
						{6, 5, 4},
						{0, 3, 2},
						{1, 6, 5},
						{4, 0, 3},
						{2, 1, 6},
						{5, 4, 0},
						{3, 2, 1},
						{6, 5, 4}},
						{{4, 0, 5},
						{3, 2, 6},
						{2, 3, 6},
						{1, 4, 5},
						{0, 5, 4},
						{2, 6, 3},
						{3, 6, 2},
						{4, 5, 1},
						{5, 4, 0},
						{6, 3, 2},
						{6, 2, 3},
						{5, 1, 4}} };
		memcpy(decans, decans1, sizeof(decans1));

		oneruler = true;
		usedaynightorb = false;

		int dignityscores1[5] = { 5, 4, 3, 2, 1 };
		memcpy(dignityscores, dignityscores1, sizeof(dignityscores1));

		useaccidental = true;

		int housescores1[12] = { 12, 6, 3, 9, 7, 1, 10, 5, 4, 11, 8, 2 };
		memcpy(housescores, housescores1, sizeof(housescores1));

		int sunphases1[3] = { 3, 2, 1 };
		memcpy(sunphases, sunphases1, sizeof(sunphases1));

		int dayhourscores1[2] = { 7, 6 };
		memcpy(dayhourscores, dayhourscores1, sizeof(dayhourscores1));

		useexaltationmercury = false;
		topicals = -1;
		arabicpartsref = 0;
		daynightorbdeg = 0;
		daynightorbmin = 0;
		arabicparts = -1;
		//ayanamsha = 0;

		//#Housesystem
		hsys = 'P';

		//#Nodes
		meannode = true;

		double orbis1[11][11] = {
			{5.0, 1.75, 1.75, 3.0, 1.75, 4.0, 4.0, 1.75, 1.75, 1.75, 5.0},//sun
			{5.0, 1.75, 1.75, 3.0, 1.75, 4.0, 4.0, 1.75, 1.75, 1.75, 5.0},//moon
			{3.5, 1.5, 1.5, 2.5, 1.5, 3.0, 3.0, 1.5, 1.5, 1.5, 3.5},//mercury 
			{3.5, 1.5, 1.5, 2.5, 1.5, 3.0, 3.0, 1.5, 1.5, 1.5, 3.5},//venus
			{3.5, 1.5, 1.5, 2.5, 1.5, 3.0, 3.0, 1.5, 1.5, 1.5, 3.5},//mars
			{4.0, 1.5, 1.5, 3.0, 1.5, 3.5, 3.5, 1.5, 1.5, 1.5, 4.0},//jupiter
			{4.0, 1.5, 1.5, 3.0, 1.5, 3.5, 3.5, 1.5, 1.5, 1.5, 4.0},//saturn
			{3.0, 1.0, 1.0, 2.0, 1.0, 2.5, 2.5, 1.0, 1.0, 1.0, 3.0},//uranus
			{3.0, 1.0, 1.0, 2.0, 1.0, 2.5, 2.5, 1.0, 1.0, 1.0, 3.0},//pluto
			{3.0, 1.0, 1.0, 2.0, 1.0, 2.5, 2.5, 1.0, 1.0, 1.0, 3.0},//nodes
			{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5} };//ASCMC
		////house

		memcpy(orbis, orbis1, sizeof(orbis1));


		double orbisplanetspar1[11][2] = { {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0} };
		memcpy(orbisplanetspar, orbisplanetspar1, sizeof(orbisplanetspar1));


		double orbisH1[11] = { 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25 };
		memcpy(orbisH, orbisH1, sizeof(orbisH1));

		orbisparH[0] = orbisparH[1] = 0.25;
		orbiscuspH = 3.0;

		double orbisAscMC1[11] = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
		memcpy(orbisAscMC, orbisAscMC1, sizeof(orbisAscMC1));


		orbisparAscMC[0] = orbisparAscMC[1] = 0.5;

		orbiscuspAscMC = 5.0;
		exact = 1.0;

		//#Primary Dirs
		primarydir = 0;
		subprimarydir = 2;
		subzodiacal = 0;
		bianchini = false;

		sigascmc[0] = sigascmc[1] = true;

		sighouses = false;

		bool sigplanets1[12] = { true, true, true, true, true, true, true, false, false, false, true, true };
		//bool sigplanets2[16]={false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true};
		bool sigplanets2[16] = { true, true, true, true, true, true, true, false, false, false, true, true, true, true, true, true };
		memcpy(sigplanets, sigplanets1, sizeof(sigplanets1));
		memcpy(promplanets, sigplanets2, sizeof(sigplanets2));

		bool pdaspects1[11] = { true, true, false, true, false, true, true, false, false, true, true };
		memcpy(pdaspects, pdaspects1, sizeof(pdaspects1));

		pdmidpoints = false;

		pdparallels[0] = pdparallels[1] = 1;

		pdsecmotion = true;
		pdsecmotioniter = 2;//#3rd iter is the default

		zodpromsigasps[0] = true, zodpromsigasps[1] = false;
		ascmchcsasproms = false;

		pdfixstars = false;

		bool pdfixstarssel1[30] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
		memcpy(pdfixstarssel, pdfixstarssel1, sizeof(pdfixstarssel1));

		pdlof[0] = pdlof[1] = false;

		pdsyzygy = false;

		pdterms = true;

		pdantiscia = false;

		pdcustomer = false;
		memset(pdcustomerlon, 0, 3);
		memset(pdcustomerlat, 0, 3);

		pdcustomersouthern = false;
		pdcustomer2 = false;

		memset(pdcustomer2lon, 0, 3);

		memset(pdcustomer2lat, 0, 3);

		pdcustomer2southern = false;
		pdkeydyn = false;
		pdkeyd = 0;
		pdkeys = 2;
		pdkeydeg = 0;
		pdkeymin = 0;
		pdkeysec = 0;

		useregressive = false;

		us.nArabicNight = 0;
		lotoffortune = 2;

		//#Syzygy
		syzmoon = MOON;
		const char* fixstars1[30] = {
			"etTau", "alTau", "bePer", "ga-1And", "alSco", "alBoo", "deCnc", "gaCnc",
				"etUMa", "alOri", "alCen", "alCar", "alGem", "beLeo", "alPsA", "alCrB",
				"alPeg", "beAnd", "alUMi", "beGem", "M44", "alCMi", "alLeo", "beOri",
				"alCMa", "alVir", "alSer", "alLyr", "al-2Lib", "beLib" };

		memcpy(fixstars, fixstars1, sizeof(fixstars1));

		//#Profections
		zodprof = true;
		usezodprojsprof = false;

		isfirbonatti = true;
		strcpy(deflocname, "");
		deflocplus = true;
		defloczhour = 0;
		defloczminute = 0;
		deflocdst = false;
		defloclondeg = 0;
		defloclonmin = 0;
		defloclatdeg = 0;
		defloclatmin = 0;
		defloceast = true;
		deflocnorth = true;
		deflocalt = 0;

		//##PDsInChart
		pdincharttyp = 2;
		pdinchartsecmotion = false;

		pdinchartterrsecmotion = true;

		//#Languages
		langid = 0;
		autosave = false;

		unsigned long  rgbbmp1[cColor] = {
			0x000000L,// kBlack           0
				0x00007FL,// kMaroon          1
				RGBVal(0,172,0),// kDkGreen      2
				RGBVal(206,134,0),// kOrange     3
				0x7F0000L,// kDkBlue          4
				RGBVal(255,30,255),// kPurple          5
				0x7F7F00L,// kDkCyan          6
				0xBFBFBFL,// kLtGray          7
				0x7F7F7FL,// kDkGray          8
				0x0000FFL,// kRed             9
				RGBVal(0,213,0),//0x00FF00L,// kGreen           10
				0x00FFFFL,// kYellow          11
				RGBVal(255,157,174)/*0xFF0000L*/,// kBlue            12
				RGBVal(51,133,255)/*0xFF00FFL*/,// kMagenta           13
				RGBVal(28,174,240),//0xFFFF00L,// kCyan                                  14
				RGBVal(255,255,255)//RGBVal(254,255,251)//RGBVal(250,250,250)/*0xFFFFFFL*/// kWhite             15    FFA500
		};

		int kElemA1[4] = { 9,3,2,13 };
		int kAspA1[18 + 1] = { 15, 4, 12, 9, 10, 14, 13, 13, 4, 4, 11, 6, 6, 1, 5, 5, 1, 1, 5 };

		memcpy(&this->rgbbmp, &rgbbmp1, 16 * sizeof(long));
		memcpy(&this->kAspA, &kAspA1, 19 * sizeof(int));
		memcpy(&this->kElemA, &kElemA1, 4 * sizeof(int));

		strcpy(myBuff, "MS Shell Dlg");

		if (wi.chs)
			size = 12;
		else
			size = 14;

		cWeight = 300;//FW_LIGHT FW_BLACK,             cWeight
		bItalic = 0;//   bItalic
		hasToolbar = false;
		bStatusBar = false;
		fInverse = true;
		UseAspectGrid = false;
		HarmonicChartsYearsMode = true;
		NoNutation = false;
		NoPrecession = false;
		AddNutation = false;
		IsChaldean = true;
		Dignities = 1;   // 1 for tradtion astrolog, 0 for modern astrolog
		IsPlanetModern = false;
		IsPlanetTraditional = false;
		IsPlanetLuminaries = false;
		orbs = -1;
		progdirrestric = 8;
		UseTheme = 1;
		zregyptian = true;
		zr27cap = true;
		ZodRelsign = 0;
		ZodRelperiod = 6;
		PlanetOrder = 0;  //  Chaldean
		StartWith = 0;    //  起运方法
		sectptolemy = false;
	}
};

#endif