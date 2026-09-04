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
//
// The --text (@0203) output reproduces the original-engine machine format
// (9-decimal positions) and is intended to be diffed against the golden
// samples in test/golden/.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "../include/astrolog_lib.h"
#include "../include/utils/utils.h"
#include "../include/utils/TransU.h"

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
		} else if (a == "-h" || a == "--help") {
			printf("astrolog32-cli - Astrolog32 refactor CLI\n");
			printf("Usage:\n");
			printf("  astrolog32-cli -qb M D Y T dst zon lon lat [--json|--text] [-n name] [-l location] [-o file]\n");
			printf("  lon: west positive / east negative ; zon: east positive / west negative\n");
			return 0;
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
