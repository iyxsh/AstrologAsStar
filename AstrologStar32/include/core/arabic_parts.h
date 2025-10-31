#ifndef ARABIC_PARTS_H
#define ARABIC_PARTS_H

#include "planet.h"
#include "houses.h"
#include <vector>
#include <string>

// 阿拉伯点数据结构
struct ArabicPart {
    std::wstring name;           // 名称
    std::wstring shortName;      // 简称
    double longitude;            // 经度
    int house;                   // 所在宫位
};

// 阿拉伯点计算公式类型
#define ARABIC_FORMULA_NORMAL    0  // 标准公式: ASC + B - C
#define ARABIC_FORMULA_REVERSE   1  // 反向公式: ASC + C - B
#define ARABIC_FORMULA_NIGHT     2  // 夜间公式: 使用不同的计算方式

class ArabicPartCalculator {
public:
    // 计算所有阿拉伯点
    static void calculateAllArabicParts(const std::vector<PlanetData>& planets,
                                     const HouseData& houses,
                                     std::vector<ArabicPart>& arabicParts);
    
    // 计算特定阿拉伯点
    static double calculateArabicPart(double ascendant, 
                                    double point1, 
                                    double point2, 
                                    int formula = ARABIC_FORMULA_NORMAL);
    
    // 获取阿拉伯点列表
    static void getArabicPartList(std::vector<std::wstring>& partNames);
    
    // 格式化阿拉伯点显示
    static std::wstring formatArabicPart(const ArabicPart& part, int format = 0);

private:
    // 内部辅助函数
    static void initializeArabicParts(std::vector<ArabicPart>& arabicParts);
    static void addArabicPart(std::vector<ArabicPart>& parts, 
                            const wchar_t* name, 
                            const wchar_t* shortName,
                            double longitude);
};

// 常用阿拉伯点索引
#define ARABIC_PART_FORTUNE         0   // 财运点
#define ARABIC_PART_SPIRIT          1   // 精神点
#define ARABIC_PART_EROS            2   // 爱情点
#define ARABIC_PART_VICTORY         3   // 胜利点
#define ARABIC_PART_NEMESIS         4   // 报应点
#define ARABIC_PART_NEEDED          5   // 需要点
#define ARABIC_PART_CAREER          6   // 事业点
#define ARABIC_PART_MARRIAGE        7   // 婚姻点
#define ARABIC_PART_DEATH           8   // 死亡点
#define ARABIC_PART_EXALTATION      9   // 尊贵点
#define ARABIC_PART_ENEMY          10   // 敌人点
#define ARABIC_PART_FATHER         11   // 父亲点
#define ARABIC_PART_MOTHER         12   // 母亲点
#define ARABIC_PART_SIBLINGS       13   // 兄弟姐妹点
#define ARABIC_PART_CHILDREN       14   // 子女点
#define ARABIC_PART_UNCLE          15   // 叔伯点
#define ARABIC_PART_GRANDPARENTS   16   // 祖父母点
#define ARABIC_PART_CONJUNCTIVE    17   // 连接点
#define ARABIC_PART_MIND           18   // 心智点
#define ARABIC_PART_BODIES         19   // 身体点
#define ARABIC_PART_VIRTUE         20   // 美德点
#define ARABIC_PART_TRIBULATION    21   // 磨难点
#define ARABIC_PART_KINGS          22   // 权力点
#define ARABIC_PART_FASHION        23   // 时尚点
#define ARABIC_PART_LAWS           24   // 法律点
#define ARABIC_PART_THIEF          25   // 小偷点
#define ARABIC_PART_FRAUD          26   // 欺诈点
#define ARABIC_PART_BEARER         27   // 携带点
#define ARABIC_PART_TREASURY       28   // 宝库点
#define ARABIC_PART_FESTIVAL       29   // 节日点
#define ARABIC_PART_MARKET         30   // 市场点
#define ARABIC_PART_HORSE          31   // 马匹点
#define ARABIC_PART_SAILOR         32   // 水手点
#define ARABIC_PART_SOLDIER        33   // 士兵点
#define ARABIC_PART_MONK           34   // 僧侣点
#define ARABIC_PART_DOCTOR         35   // 医生点
#define ARABIC_PART_DYER           36   // 染工点
#define ARABIC_PART_FOOD           37   // 食物点
#define ARABIC_PART_SACRIFICE      38   // 祭祀点
#define ARABIC_PART_SUFFERING      39   // 苦难点
#define ARABIC_PART_COURSES        40   // 课程点
#define ARABIC_PART_MESSIAH        41   // 弥赛亚点
#define ARABIC_PART_DEPRESSION     42   // 抑郁点
#define ARABIC_PART_INJURY         43   // 伤害点
#define ARABIC_PART_INHERITANCE    44   // 遗产点
#define ARABIC_PART_BEGETTING      45   // 生育点
#define ARABIC_PART_SCHOLARSHIP    46   // 奖学金点
#define ARABIC_PART_JOURNEY        47   // 旅程点
#define ARABIC_PART_MISERY         48   // 悲惨点
#define ARABIC_PART_SONS           49   // 儿子点
#define ARABIC_PART_DAUGHTERS      50   // 女儿点
#define ARABIC_PART_WAY            51   // 道路点
#define ARABIC_PART_MISFORTUNE     52   // 不幸点
#define ARABIC_PART_BEGGING        53   // 乞讨点
#define ARABIC_PART_CATAMENIA      54   // 月经点
#define ARABIC_PART_SICKNESS       55   // 疾病点
#define ARABIC_PART_IMPRISONMENT   56   // 监禁点
#define ARABIC_PART_SLAVE          57   // 奴隶点
#define ARABIC_PART_BONDAGE        58   // 束缚点
#define ARABIC_PART_IDIOT          59   // 愚蠢点
#define ARABIC_PART_MADNESS        60   // 疯癫点
#define ARABIC_PART_ORPHAN         61   // 孤儿点
#define ARABIC_PART_SUICIDE        62   // 自杀点
#define ARABIC_PART_MASSACRE       63   // 大屠杀点
#define ARABIC_PART_JESTER         64   // 小丑点
#define ARABIC_PART_RACE           65   // 种族点
#define ARABIC_PART_SACRIFICE2     66   // 祭祀点2
#define ARABIC_PART_WITNESS        67   // 证人点
#define ARABIC_PART_JUDGE          68   // 法官点
#define ARABIC_PART_LAW            69   // 法律点
#define ARABIC_PART_PRISON         70   // 监狱点
#define ARABIC_PART_CROSS          71   // 十字架点
#define ARABIC_PART_SUN            72   // 太阳点
#define ARABIC_PART_MOON           73   // 月亮点
#define ARABIC_PART_MERCURY        74   // 水星点
#define ARABIC_PART_VENUS          75   // 金星点
#define ARABIC_PART_MARS           76   // 火星点
#define ARABIC_PART_JUPITER        77   // 木星点
#define ARABIC_PART_SATURN         78   // 土星点
#define ARABIC_PART_URANUS         79   // 天王星点
#define ARABIC_PART_NEPTUNE        80   // 海王星点
#define ARABIC_PART_PLUTO          81   // 冥王星点

#endif // ARABIC_PARTS_H