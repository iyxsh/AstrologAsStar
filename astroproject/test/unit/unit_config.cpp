// astrolog32_unit_config — P1.1 config-layer unit tests (no gtest).
//
// Verifies the original-format switch interpreter (core/config.cpp):
//   - -c <n> house system (+ invalid / missing arg rejected)
//   - -Yn / -YL true-node & true-lilith toggles
//   - -P <n> Arabic parts count (+ range check)
//   - -h <n> center object
//   - -s <offset> sidereal correction + bare -s toggle
//   - unknown / unimplemented switches are safe no-ops (returns success)
//   - ConfigLoadFile() reads an astrolog32.dat-style text file (comments,
//     blanks, quoted tokens) and stops on the first invalid line
//
// Numeric output alignment stays covered by test/verify_cli.py; this file
// only guards the configuration plumbing (A16 gate).
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../../include/core/config.h"
#include "../../include/models/settings.h"

extern US us;
extern unsigned char oscLilith;   /* astrolog.cpp byte oscLilith */

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        return 1; \
    } \
} while (0)

/* Snapshot the fields this suite mutates, so cases are independent. */
struct Snap {
    int house, arabicParts, center;
    double siderealCorrection;
    int trueNode, lilith, sidereal;
};
static Snap snap(void)
{
    Snap s;
    s.house = us.nHouseSystem;
    s.arabicParts = us.nArabicParts;
    s.center = us.objCenter;
    s.siderealCorrection = us.rSiderealCorrection;
    s.trueNode = us.fTrueNode;
    s.lilith = oscLilith;
    s.sidereal = us.fSidereal;
    return s;
}
static void restore(const Snap& s)
{
    us.nHouseSystem = s.house;
    us.nArabicParts = s.arabicParts;
    us.objCenter = s.center;
    us.rSiderealCorrection = s.siderealCorrection;
    us.fTrueNode = s.trueNode;
    oscLilith = s.lilith;
    us.fSidereal = s.sidereal;
}

int main(void)
{
    Snap base = snap();
    char err[256];

    /* ---- 1. -c house system ---- */
    const char* c6[] = { "-c", "6" };
    err[0] = 0;
    CHECK(ConfigProcessTokens(c6, 2, err, sizeof(err)) == 1);
    CHECK(us.nHouseSystem == 6);
    restore(base);

    const char* cbad[] = { "-c", "99" };      /* out of 16-system range */
    CHECK(ConfigProcessTokens(cbad, 2, err, sizeof(err)) == 0);
    CHECK(err[0] != '\0');
    const char* cnone[] = { "-c" };           /* missing argument */
    CHECK(ConfigProcessTokens(cnone, 1, err, sizeof(err)) == 0);
    restore(base);

    /* ---- 2. -Yn true node toggle (inverts from base) ---- */
    int tn0 = us.fTrueNode;
    CHECK(ConfigApply("-Yn", err, sizeof(err)) == 1);
    CHECK(us.fTrueNode == (tn0 ? 0 : 1));
    CHECK(ConfigApply("-Yn", err, sizeof(err)) == 1);
    CHECK(us.fTrueNode == tn0);
    restore(base);

    /* ---- 3. -YL true (osculating) Lilith toggle ---- */
    int lil0 = oscLilith;
    CHECK(ConfigApply("-YL", err, sizeof(err)) == 1);
    CHECK(oscLilith == (lil0 ? 0 : 1));
    CHECK(ConfigApply("-YL", err, sizeof(err)) == 1);
    CHECK(oscLilith == lil0);
    restore(base);

    /* ---- 4. -P Arabic parts count + range check ---- */
    const char* p20[] = { "-P", "20" };
    CHECK(ConfigProcessTokens(p20, 2, err, sizeof(err)) == 1);
    CHECK(us.nArabicParts == 20);
    restore(base);
    const char* pbad[] = { "-P", "999" };     /* > cPart(177) */
    CHECK(ConfigProcessTokens(pbad, 2, err, sizeof(err)) == 0);
    CHECK(err[0] != '\0');
    restore(base);

    /* ---- 5. -h center object ---- */
    const char* h0[] = { "-h", "0" };
    CHECK(ConfigProcessTokens(h0, 2, err, sizeof(err)) == 1);
    CHECK(us.objCenter == 0);
    restore(base);

    /* ---- 6. -s <offset> and bare -s toggle ---- */
    const char* s1[] = { "-s", "1" };
    CHECK(ConfigProcessTokens(s1, 2, err, sizeof(err)) == 1);
    CHECK(us.rSiderealCorrection == 1.0);     /* offset applied */
    CHECK(us.fSidereal == (base.sidereal ? 0 : 1));  /* bare -s also toggles */
    restore(base);
    CHECK(ConfigApply("-s", err, sizeof(err)) == 1);
    CHECK(us.fSidereal == (base.sidereal ? 0 : 1));
    restore(base);

    /* ---- 7. unknown switches are safe no-ops ---- */
    CHECK(ConfigApply("-Qx", err, sizeof(err)) == 1);   /* unknown letter */
    CHECK(ConfigApply("-Yq", err, sizeof(err)) == 1);   /* unimplemented -Y sub */
    CHECK(us.nHouseSystem == base.house);
    restore(base);

    /* ---- 8. ConfigLoadFile: astrolog32.dat-style text ---- */
    const char* datPath = "unit_config_test.dat";
    FILE* f = fopen(datPath, "wb");
    CHECK(f != NULL);
    fprintf(f, "# P1.1 test config\r\n"          /* comment + CRLF */
               "\r\n"                            /* blank line */
               "; house to Porphyry (6)\r\n"
               "-c 6\r\n"
               "-Yn\r\n"
               "-P 12\r\n");
    fclose(f);
    err[0] = 0;
    CHECK(ConfigLoadFile(datPath, err, sizeof(err)) == 1);
    CHECK(us.nHouseSystem == 6);
    CHECK(us.fTrueNode == (base.trueNode ? 0 : 1));
    CHECK(us.nArabicParts == 12);
    restore(base);

    /* invalid line stops the load and reports file/line */
    f = fopen(datPath, "wb");
    fprintf(f, "-c 6\n-c 99\n");                /* 99 out of range on line 2 */
    fclose(f);
    err[0] = 0;
    CHECK(ConfigLoadFile(datPath, err, sizeof(err)) == 0);
    CHECK(strstr(err, "line 2") != NULL);
    remove(datPath);
    restore(base);

    /* missing file -> error, not crash */
    CHECK(ConfigLoadFile("unit_config_NO_SUCH.dat", err, sizeof(err)) == 0);
    CHECK(err[0] != '\0');

    printf("PASS unit_config: house/node/lilith/arabic/center/sidereal switches\n");
    return 0;
}
