// astrolog32-cli — P0.3 CLI for the refactored Astrolog32 library.
//
// Mirrors a subset of the original Astrolog command-line switches:
//   -qb M D Y T dst zon lon lat   natal chart
//                                  (lon: west positive / east negative;
//                                   zon: east positive / west negative)
//   --json | --text               output format (default: --text = @0203)
//   -n <name> -l <location>       chart name / location
//   -o <file> | --o0 <file>       write output to file instead of stdout
//   -h | --help
//   --cfg <file>                  load an astrolog32.dat-format config file
//   ...engine switches...         P1.1 config passthrough, e.g. -c 6 -Yn -P 20
//                                 (-s -sr -sm <n> -h <n> -Pz -YL ...)
//
// The --text (@0203) output reproduces the original-engine machine format
// (9-decimal positions) and is intended to be diffed against the golden
// samples in test/golden/.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "../include/astrolog_lib.h"
#include "../include/utils/utils.h"
#include "../include/utils/TransU.h"
#include "../include/core/config.h"

static std::string w2u(const std::wstring& w)
{
	std::string out;
	for (wchar_t wc : w) {
		if (wc < 0x80) out.push_back((char)wc);
		else if (wc < 0x800) {
			out.push_back((char)(0xC0 | (wc >> 6)));
			out.push_back((char)(0x80 | (wc & 0x3F)));
		}
		else {
			out.push_back((char)(0xE0 | (wc >> 12)));
			out.push_back((char)(0x80 | ((wc >> 6) & 0x3F)));
			out.push_back((char)(0x80 | (wc & 0x3F)));
		}
	}
	return out;
}

int main(int argc, char* argv[])
{
	bool json = false;
	std::string outFile;
	std::string cfgFile;
	std::vector<std::string> cfgToks;   /* engine-switch passthrough tokens */
	std::wstring name = L"";
	std::wstring loc = L"";
	bool haveQb = false;
	int qbMon = 0, qbDay = 0, qbYea = 0;
	const char* qbTim = nullptr;
	const char* qbDst = nullptr;
	const char* qbZon = nullptr;
	const char* qbLon = nullptr;
	const char* qbLat = nullptr;

	for (int k = 1; k < argc; k++) {
		std::string a = argv[k];
		if (a == "-qb") {
			if (k + 8 >= argc) {
				fprintf(stderr, "error: -qb requires 8 args: M D Y T dst zon lon lat\n");
				return 2;
			}
			qbMon = atoi(argv[k + 1]);
			qbDay = atoi(argv[k + 2]);
			qbYea = atoi(argv[k + 3]);
			qbTim = argv[k + 4];
			qbDst = argv[k + 5];
			qbZon = argv[k + 6];
			qbLon = argv[k + 7];
			qbLat = argv[k + 8];
			k += 8;
			haveQb = true;
		} else if (a == "--json") {
			json = true;
		} else if (a == "--text") {
			json = false;
		} else if (a == "-n") {
			if (k + 1 < argc) name = char_to_wchar(argv[++k]);
		} else if (a == "-l") {
			if (k + 1 < argc) loc = char_to_wchar(argv[++k]);
		} else if (a == "-o" || a == "--o0" || a == "--output") {
			if (k + 1 < argc) outFile = argv[++k];
		} else if (a == "--cfg" || a == "-cfg") {
			if (k + 1 < argc) cfgFile = argv[++k];
			else { fprintf(stderr, "error: --cfg requires a file path\n"); return 2; }
		} else if (a == "-h" || a == "--help") {
			/* -h 后跟数字 = 原版中心天体开关 -h <n>；否则为帮助 */
			bool numericNext = (k + 1 < argc) && argv[k + 1][0] != '\0' &&
				(strchr("-0123456789", argv[k + 1][0]) != NULL);
			if (numericNext) {
				cfgToks.push_back("-h");
				cfgToks.push_back(argv[++k]);
			} else {
				printf("astrolog32-cli - Astrolog32 refactor CLI\n");
				printf("Usage:\n");
				printf("  astrolog32-cli -qb M D Y T dst zon lon lat [--json|--text]\n");
				printf("                 [-n name] [-l location] [-o file] [--cfg file]\n");
				printf("  engine switches (P1.1): -c <house> -s [-sr|-sh|-sd|-sz] -sm <mode>\n");
				printf("    -h <center> -P <n>|-Pz|-Pn|-Pf|-P0 -Yn -YL -Yc -Yd -Yt -YC -YH\n");
				printf("  lon: west positive / east negative ; zon: east positive / west negative\n");
				return 0;
			}
		} else if ((a[0] == '-' || a[0] == '=' || a[0] == '_' || a[0] == ':') && a.size() >= 2) {
			/* 未保留的开关 → 引擎配置透传（原版开关子集，--cfg 之后应用）。
			 * 引擎开关的数字参数是独立 token（如 "-c 6"）：紧随其后的数字形
			 * token 一并收集（-s/-P 的可选数字同理；解释器会忽略多余数字）。 */
			cfgToks.push_back(a);
			if (k + 1 < argc) {
				const char* nx = argv[k + 1];
				char c0 = nx[0];
				bool numericNext = (c0 >= '0' && c0 <= '9') ||
					((c0 == '-' || c0 == '.') && nx[1] >= '0' && nx[1] <= '9');
				if (numericNext) { cfgToks.push_back(argv[++k]); }
			}
		} else {
			fprintf(stderr, "warning: ignoring unknown argument '%s'\n", a.c_str());
		}
	}

	if (!haveQb) {
		fprintf(stderr, "error: missing -qb M D Y T dst zon lat\n");
		return 2;
	}

	SetSilent(true);   // suppress the initEnv version banner on stdout
	initEnv();

	/* P1.1 config 层：--cfg 文件先加载，CLI 引擎开关后覆盖（原版分层语义） */
	if (!cfgFile.empty()) {
		char err[256] = "";
		if (!ConfigLoadFile(cfgFile.c_str(), err, sizeof(err))) {
			fprintf(stderr, "error: --cfg: %s\n", err);
			return 2;
		}
	}
	if (!cfgToks.empty()) {
		std::vector<const char*> cv;
		cv.reserve(cfgToks.size());
		for (size_t i = 0; i < cfgToks.size(); i++) cv.push_back(cfgToks[i].c_str());
		char err[256] = "";
		if (!ConfigProcessTokens(&cv[0], (int)cv.size(), err, sizeof(err))) {
			fprintf(stderr, "error: engine switch: %s\n", err);
			return 2;
		}
	}

	ChartInput ci = {0};
	ci.mon = qbMon; ci.day = qbDay; ci.yea = qbYea;
	ci.tim = RParseSz(qbTim, pmTim);
	ci.dst = RParseSz(qbDst, pmDst);
	ci.zon = RParseSz(qbZon, pmZon);
	ci.lon = RParseSz(qbLon, pmLon);
	ci.lat = RParseSz(qbLat, pmLat);
	ci.alt = 0.0;
	wcsncpy(ci.nam, name.c_str(), 255);
	wcsncpy(ci.loc, loc.c_str(), 255);

	if (json) {
		std::string j = GetChartJSON(ci);
		if (!outFile.empty()) {
			FILE* f = fopen(outFile.c_str(), "wb");
			if (f) { fputs(j.c_str(), f); fputc('\n', f); fclose(f); }
		} else {
			fputs(j.c_str(), stdout); fputc('\n', stdout);
		}
	} else {
		std::wstring w = GetChartMachineText(ci);
		std::string u = w2u(w);
		if (!outFile.empty()) {
			FILE* f = fopen(outFile.c_str(), "wb");
			if (f) { fputs(u.c_str(), f); fclose(f); }
		} else {
			fputs(u.c_str(), stdout);
		}
	}
	return 0;
}
