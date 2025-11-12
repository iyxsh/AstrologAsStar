#ifndef EPHEMERIS_H
#define EPHEMERIS_H
#include "../utils/utils.h"

// 距离单位
#define rAUToKm    149597870.7
#define rLYToAU    63241.07708427
#define rPCToAU    206264.8062471

// 时间常量
#define rDayInYear 365.24219
#define rEpoch2000 (-24.736467)
#define rJD2000    2451545.0

// 只在未定义时定义这些宏
#ifndef rPiHalf
#define rPiHalf    (rPi/2.0)
#endif

#ifndef rPiQuarter
#define rPiQuarter (rPi/4.0)
#endif

// Windows accepts names up to 256 characters, which can be 512 bytes for 
// Unicode, however Astrolog does not do Unicode, so we use a size of 256. Note 
// that the Windows defile MAX_PATH actually has 260 bytes. For our purposes 
// using that would cause us some difficulties when comparing sizes, so we 
// create our own definition MAX_FILE_NAME. 
#define MAX_FILE_NAME	256
#define MAX_STRING_NAME_LEN 50
typedef char file_name_t[MAX_FILE_NAME + 1];	// File name or directory or a combination

typedef struct
{
	char		name[MAX_STRING_NAME_LEN];	// name used in user's interface
	file_name_t dir;	// actual directory
}
directory_t;

typedef struct
{
	// This is the drive plus directory where astrolog32.exe resides. This 
	// directory is one of the places Astrolog looks for files.
	directory_t executable;

	// we have several directories where we put different sorts of files.

	// Astrolog32.exe, astrolog32.dat, icons, URLs, documentation files, 
	// fixed stars catalogue.
	directory_t main;

	// This directory will store Swiss Ephemeris files. Note that while a list 
	// of search directories is passed by Astolog32 to Swiss Ephemeris, Swiss 
	// Ephemeris has its own file search routine. For example, one of the places 
	// it looks for is the directory specified by the environment variable 
	// SE_EPHE_PATH. This is the current behaviour and will not be changed, as 
	// Astolog (currently) uses Swiss Ephemeris in its unchanged form. 
	directory_t ephemeris;

	// Charts and cp0.longitude position files.
	directory_t charts;

	// Interpretation files (pl01.dat, house10.dat, etc).
	directory_t interpretation;

	// Miscellaneous files, such as chart saved as either text, bitmap, Windows 
	// Meta File, or Postscript.
	directory_t miscel;

	// Atlas files. 
	directory_t american_atlas;
	directory_t international_atlas;
}
global_directories_t;

extern global_directories_t dirs;

// 函数声明
void SetEphemerisPath(void);
void ComputeWithSwissEphemeris(double t);
#endif // EPHEMERIS_H