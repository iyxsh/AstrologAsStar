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
#include "../../include/core/aspects.h"   /* cAspect/ignoreA (A6 -RA/-RE) */
#include "../../include/models/settings.h"

extern US us;
extern unsigned char oscLilith;   /* astrolog.cpp byte oscLilith */
extern byte ignoreA[];            /* aspects.cpp（aspects.h 已 extern） */
extern double rAspOrb[];          /* utils.cpp 相位容差表 */
extern double rObjOrb[];          /* utils.cpp 对象容差表 */

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
    int allStar, nStar;
    int nAsp;
    byte iA[cAspect + 1];
    double aspOrb[cAspect + 1];
    double objOrb[42 + 1];   /* cLastMoving=42（planet.h 枚举） */
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
    s.allStar = us.fAllStar;
    s.nStar = us.nStar;
    s.nAsp = us.nAsp;
    memcpy(s.iA, ignoreA, sizeof(s.iA));
    memcpy(s.aspOrb, rAspOrb, sizeof(s.aspOrb));
    memcpy(s.objOrb, rObjOrb, sizeof(s.objOrb));
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
    us.fAllStar = s.allStar;
    us.nStar = s.nStar;
    us.nAsp = s.nAsp;
    memcpy(ignoreA, s.iA, sizeof(s.iA));
    memcpy(rAspOrb, s.aspOrb, sizeof(s.aspOrb));
    memcpy(rObjOrb, s.objOrb, sizeof(s.objOrb));
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

    /* ---- 9. -U fixed-star enable (A4): bare -U enables, -U<sub> sorts ---- */
    int st0 = us.fAllStar, ns0 = us.nStar;
    CHECK(ConfigApply("-U", err, sizeof(err)) == 1);
    CHECK(us.fAllStar == 1);                    /* real-star computation on */
    restore(base);
    CHECK(ConfigApply("-Uz", err, sizeof(err)) == 1);
    CHECK(us.fAllStar == 1);
    CHECK(us.nStar == 'z');                     /* zodiac sort mode */
    restore(base);
    CHECK(ConfigApply("-Ub", err, sizeof(err)) == 1);
    CHECK(us.fAllStar == 1);
    CHECK(us.nStar == 'b');                     /* brightness sort mode */
    restore(base);
    CHECK(us.fAllStar == st0 && us.nStar == ns0); /* restore() reverts both */
    restore(base);

    /* ---- 10. -RE/-RA aspect enable/restrict (A6) ---- */
    {
        /* 默认 ignoreA 屏蔽 6..18（前序用例均 restore，此处为初始默认态）；
         * UpdateAspectCount 为绝对重算：nAsp = 18 − 被屏蔽数。 */
        const char* re6[] = { "-RE", "6" };          /* 启用第 6 相（150°）：屏蔽剩 12 → nAsp=6 */
        CHECK(ConfigProcessTokens(re6, 2, err, sizeof(err)) == 1);
        CHECK(ignoreA[6] == 0);
        CHECK(us.nAsp == 6);
        restore(base);
        const char* ra3[] = { "-RA", "3" };          /* 屏蔽第 3 相（90°）：屏蔽 14 → nAsp=4 */
        CHECK(ConfigProcessTokens(ra3, 2, err, sizeof(err)) == 1);
        CHECK(ignoreA[3] == 1);
        CHECK(us.nAsp == 4);
        restore(base);
        const char* raBad[] = { "-RA", "99" };       /* 越界 → 报错 */
        CHECK(ConfigProcessTokens(raBad, 2, err, sizeof(err)) == 0);
        CHECK(err[0] != '\0');
        restore(base);
        const char* reNoArg[] = { "-RE" };           /* 无数字 → 成功 no-op（原版语义） */
        CHECK(ConfigProcessTokens(reNoArg, 1, err, sizeof(err)) == 1);
        restore(base);
        /* 非 -RA/-RE 的 -R* 保持 no-op 不报错 */
        CHECK(ConfigApply("-Rq", err, sizeof(err)) == 1);
        restore(base);
    }

    /* ---- 11. -Ao/-Am custom orbs (A6 余项) ---- */
    {
        const char* ao6[] = { "-Ao", "6", "9.5" };        /* 第 6 相容差 9.5° */
        CHECK(ConfigProcessTokens(ao6, 3, err, sizeof(err)) == 1);
        CHECK(rAspOrb[6] == 9.5);
        restore(base);
        const char* am2[] = { "-Am", "2", "4.25" };       /* Moon 对象容差 4.25° */
        CHECK(ConfigProcessTokens(am2, 3, err, sizeof(err)) == 1);
        CHECK(rObjOrb[2] == 4.25);
        restore(base);
        const char* aoNeg[] = { "-Ao", "3", "-1" };       /* 负 orb → 旧版语义：忽略该相 */
        CHECK(ConfigProcessTokens(aoNeg, 3, err, sizeof(err)) == 1);
        CHECK(ignoreA[3] == 1 && rAspOrb[3] == 0.0);
        restore(base);
        const char* aoBad[] = { "-Ao", "99", "1" };       /* 越界 → 报错 */
        CHECK(ConfigProcessTokens(aoBad, 3, err, sizeof(err)) == 0);
        CHECK(err[0] != '\0');
        restore(base);
        const char* aoShort[] = { "-Ao", "6" };           /* 缺数值 → 报错 */
        CHECK(ConfigProcessTokens(aoShort, 2, err, sizeof(err)) == 0);
        restore(base);
        const char* anNoop[] = { "-An", "6", "Foo" };     /* 未实现子开关 no-op */
        CHECK(ConfigProcessTokens(anNoop, 3, err, sizeof(err)) == 1);
        restore(base);
    }

    printf("PASS unit_config: house/node/lilith/arabic/center/sidereal/allstar/aspect/orb switches\n");
    return 0;
}
