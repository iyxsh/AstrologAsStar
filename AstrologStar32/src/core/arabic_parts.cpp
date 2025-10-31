#include "../../include/core/arabic_parts.h"
#include "../../include/data/signs.h"
#include <cmath>
#include <cwchar>

// 常用阿拉伯点名称
const wchar_t* arabicPartNames[] = {
    L"财运点", L"精神点", L"爱情点", L"胜利点", L"报应点",
    L"需要点", L"事业点", L"婚姻点", L"死亡点", L"尊贵点",
    L"敌人点", L"父亲点", L"母亲点", L"兄弟姐妹点", L"子女点",
    L"叔伯点", L"祖父母点", L"连接点", L"心智点", L"身体点",
    L"美德点", L"磨难点", L"权力点", L"时尚点", L"法律点",
    L"小偷点", L"欺诈点", L"携带点", L"宝库点", L"节日点",
    L"市场点", L"马匹点", L"水手点", L"士兵点", L"僧侣点",
    L"医生点", L"染工点", L"食物点", L"祭祀点", L"苦难点",
    L"课程点", L"弥赛亚点", L"抑郁点", L"伤害点", L"遗产点",
    L"生育点", L"奖学金点", L"旅程点", L"悲惨点", L"儿子点",
    L"女儿点", L"道路点", L"不幸点", L"乞讨点", L"月经点",
    L"疾病点", L"监禁点", L"奴隶点", L"束缚点", L"愚蠢点",
    L"疯癫点", L"孤儿点", L"自杀点", L"大屠杀点", L"小丑点",
    L"种族点", L"祭祀点2", L"证人点", L"法官点", L"法律点",
    L"监狱点", L"十字架点", L"太阳点", L"月亮点", L"水星点",
    L"金星点", L"火星点", L"木星点", L"土星点", L"天王星点",
    L"海王星点", L"冥王星点"
};

// 常用阿拉伯点简称
const wchar_t* arabicPartShortNames[] = {
    L"财", L"精", L"爱", L"胜", L"报",
    L"需", L"事", L"婚", L"死", L"尊",
    L"敌", L"父", L"母", L"兄", L"子",
    L"叔", L"祖", L"连", L"心", L"身",
    L"德", L"磨", L"权", L"时", L"法",
    L"偷", L"欺", L"携", L"宝", L"节",
    L"市", L"马", L"水", L"兵", L"僧",
    L"医", L"染", L"食", L"祭", L"苦",
    L"课", L"弥", L"抑", L"伤", L"遗",
    L"生", L"奖", L"旅", L"惨", L"儿",
    L"女", L"道", L"不", L"乞", L"月",
    L"病", L"监", L"奴", L"束", L"愚",
    L"疯", L"孤", L"自", L"屠", L"丑",
    L"种", L"祭2", L"证", L"判", L"律",
    L"狱", L"十", L"日", L"月", L"水",
    L"金", L"火", L"木", L"土", L"天",
    L"海", L"冥"
};

// 计算所有阿拉伯点
void ArabicPartCalculator::calculateAllArabicParts(const std::vector<PlanetData>& planets,
                                               const HouseData& houses,
                                               std::vector<ArabicPart>& arabicParts) {
    initializeArabicParts(arabicParts);
    
    if (planets.size() < 10) return;
    
    double asc = houses.ascendant;
    double sun = planets[0].longitude;
    double moon = planets[1].longitude;
    double mercury = planets[2].longitude;
    double venus = planets[3].longitude;
    double mars = planets[4].longitude;
    double jupiter = planets[5].longitude;
    double saturn = planets[6].longitude;
    
    // 判断是白天还是夜晚（太阳是否在地平线以上）
    // 修复：使用正确的HouseData成员，cusps[1]是上升点，cusps[10]是下降点，cusps[4]是IC，cusps[7]是MC
    bool isDaytime = (sun >= houses.cusps[1] && sun < houses.cusps[10]) || 
                    (sun >= houses.cusps[4] && sun < houses.cusps[7]);
    
    // 财运点 (Part of Fortune)
    double fortune = calculateArabicPart(asc, moon, sun, 
                                       isDaytime ? ARABIC_FORMULA_NORMAL : ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"财运点", L"财", fortune);
    
    // 精神点 (Part of Spirit)
    double spirit = calculateArabicPart(asc, sun, moon,
                                      isDaytime ? ARABIC_FORMULA_REVERSE : ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"精神点", L"精", spirit);
    
    // 爱情点 (Part of Eros)
    double eros = calculateArabicPart(asc, venus, sun, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"爱情点", L"爱", eros);
    
    // 胜利点 (Part of Victory)
    double victory = calculateArabicPart(asc, jupiter, mars, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"胜利点", L"胜", victory);
    
    // 报应点 (Part of Nemesis)
    double nemesis = calculateArabicPart(asc, fortune, sun, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"报应点", L"报", nemesis);
    
    // 需要点 (Part of Necessity/Needed)
    double needed = calculateArabicPart(asc, moon, mars, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"需要点", L"需", needed);
    
    // 事业点 (Part of Career)
    double career = calculateArabicPart(asc, sun, saturn, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"事业点", L"事", career);
    
    // 婚姻点 (Part of Marriage)
    double marriage = calculateArabicPart(asc, venus, jupiter, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"婚姻点", L"婚", marriage);
    
    // 死亡点 (Part of Death)
    double death = calculateArabicPart(asc, mars, jupiter, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"死亡点", L"死", death);
    
    // 尊贵点 (Part of Exaltation)
    double exaltation = calculateArabicPart(asc, sun, mars, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"尊贵点", L"尊", exaltation);
    
    // 敌人点 (Part of Enemy)
    double enemy = calculateArabicPart(asc, saturn, jupiter, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"敌人点", L"敌", enemy);
    
    // 父亲点 (Part of Father)
    double father = calculateArabicPart(asc, sun, jupiter, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"父亲点", L"父", father);
    
    // 母亲点 (Part of Mother)
    double mother = calculateArabicPart(asc, moon, venus, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"母亲点", L"母", mother);
    
    // 兄弟姐妹点 (Part of Siblings)
    double siblings = calculateArabicPart(asc, jupiter, mercury, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"兄弟姐妹点", L"兄", siblings);
    
    // 子女点 (Part of Children)
    double children = calculateArabicPart(asc, jupiter, mars, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"子女点", L"子", children);
    
    // 叔伯点 (Part of Uncle)
    double uncle = calculateArabicPart(asc, mars, venus, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"叔伯点", L"叔", uncle);
    
    // 祖父母点 (Part of Grandparents)
    double grandparents = calculateArabicPart(asc, saturn, mercury, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"祖父母点", L"祖", grandparents);
    
    // 连接点 (Part of Conjunctive)
    double conjunctive = calculateArabicPart(asc, mercury, jupiter, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"连接点", L"连", conjunctive);
    
    // 心智点 (Part of Mind)
    double mind = calculateArabicPart(asc, mercury, moon, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"心智点", L"心", mind);
    
    // 身体点 (Part of Bodies)
    double bodies = calculateArabicPart(asc, mars, venus, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"身体点", L"身", bodies);
    
    // 美德点 (Part of Virtue)
    double virtue = calculateArabicPart(asc, jupiter, venus, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"美德点", L"德", virtue);
    
    // 磨难点 (Part of Tribulation)
    double tribulation = calculateArabicPart(asc, saturn, mars, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"磨难点", L"磨", tribulation);
    
    // 权力点 (Part of Kings)
    double kings = calculateArabicPart(asc, sun, jupiter, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"权力点", L"权", kings);
    
    // 时尚点 (Part of Fashion)
    double fashion = calculateArabicPart(asc, venus, mercury, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"时尚点", L"时", fashion);
    
    // 法律点 (Part of Laws)
    double laws = calculateArabicPart(asc, saturn, mercury, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"法律点", L"法", laws);
    
    // 小偷点 (Part of Thief)
    double thief = calculateArabicPart(asc, mars, mercury, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"小偷点", L"偷", thief);
    
    // 欺诈点 (Part of Fraud)
    double fraud = calculateArabicPart(asc, saturn, mercury, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"欺诈点", L"欺", fraud);
    
    // 携带点 (Part of Bearer)
    double bearer = calculateArabicPart(asc, mercury, moon, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"携带点", L"携", bearer);
    
    // 宝库点 (Part of Treasury)
    double treasury = calculateArabicPart(asc, jupiter, mercury, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"宝库点", L"宝", treasury);
    
    // 节日点 (Part of Festival)
    double festival = calculateArabicPart(asc, sun, venus, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"节日点", L"节", festival);
    
    // 市场点 (Part of Market)
    double market = calculateArabicPart(asc, mercury, mars, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"市场点", L"市", market);
    
    // 马匹点 (Part of Horse)
    double horse = calculateArabicPart(asc, mars, venus, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"马匹点", L"马", horse);
    
    // 水手点 (Part of Sailor)
    double sailor = calculateArabicPart(asc, moon, mars, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"水手点", L"水", sailor);
    
    // 士兵点 (Part of Soldier)
    double soldier = calculateArabicPart(asc, mars, saturn, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"士兵点", L"兵", soldier);
    
    // 僧侣点 (Part of Monk)
    double monk = calculateArabicPart(asc, saturn, venus, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"僧侣点", L"僧", monk);
    
    // 医生点 (Part of Doctor)
    double doctor = calculateArabicPart(asc, mercury, venus, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"医生点", L"医", doctor);
    
    // 染工点 (Part of Dyer)
    double dyer = calculateArabicPart(asc, mars, mercury, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"染工点", L"染", dyer);
    
    // 食物点 (Part of Food)
    double food = calculateArabicPart(asc, moon, jupiter, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"食物点", L"食", food);
    
    // 祭祀点 (Part of Sacrifice)
    double sacrifice = calculateArabicPart(asc, mars, jupiter, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"祭祀点", L"祭", sacrifice);
    
    // 苦难点 (Part of Suffering)
    double suffering = calculateArabicPart(asc, saturn, moon, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"苦难点", L"苦", suffering);
    
    // 课程点 (Part of Courses)
    double courses = calculateArabicPart(asc, mercury, jupiter, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"课程点", L"课", courses);
    
    // 弥赛亚点 (Part of Messiah)
    double messiah = calculateArabicPart(asc, sun, venus, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"弥赛亚点", L"弥", messiah);
    
    // 抑郁点 (Part of Depression)
    double depression = calculateArabicPart(asc, saturn, moon, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"抑郁点", L"抑", depression);
    
    // 伤害点 (Part of Injury)
    double injury = calculateArabicPart(asc, mars, saturn, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"伤害点", L"伤", injury);
    
    // 遗产点 (Part of Inheritance)
    double inheritance = calculateArabicPart(asc, jupiter, saturn, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"遗产点", L"遗", inheritance);
    
    // 生育点 (Part of Begetting)
    double begetting = calculateArabicPart(asc, venus, moon, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"生育点", L"生", begetting);
    
    // 奖学金点 (Part of Scholarship)
    double scholarship = calculateArabicPart(asc, mercury, venus, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"奖学金点", L"奖", scholarship);
    
    // 旅程点 (Part of Journey)
    double journey = calculateArabicPart(asc, mercury, jupiter, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"旅程点", L"旅", journey);
    
    // 悲惨点 (Part of Misery)
    double misery = calculateArabicPart(asc, saturn, jupiter, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"悲惨点", L"惨", misery);
    
    // 儿子点 (Part of Sons)
    double sons = calculateArabicPart(asc, jupiter, sun, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"儿子点", L"儿", sons);
    
    // 女儿点 (Part of Daughters)
    double daughters = calculateArabicPart(asc, jupiter, moon, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"女儿点", L"女", daughters);
    
    // 道路点 (Part of Way)
    double way = calculateArabicPart(asc, mercury, mars, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"道路点", L"道", way);
    
    // 不幸点 (Part of Misfortune)
    double misfortune = calculateArabicPart(asc, saturn, jupiter, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"不幸点", L"不", misfortune);
    
    // 乞讨点 (Part of Begging)
    double begging = calculateArabicPart(asc, mercury, saturn, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"乞讨点", L"乞", begging);
    
    // 月经点 (Part of Catamenia)
    double catamenia = calculateArabicPart(asc, moon, venus, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"月经点", L"月", catamenia);
    
    // 疾病点 (Part of Sickness)
    double sickness = calculateArabicPart(asc, mars, saturn, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"疾病点", L"病", sickness);
    
    // 监禁点 (Part of Imprisonment)
    double imprisonment = calculateArabicPart(asc, saturn, mars, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"监禁点", L"监", imprisonment);
    
    // 奴隶点 (Part of Slave)
    double slave = calculateArabicPart(asc, saturn, moon, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"奴隶点", L"奴", slave);
    
    // 束缚点 (Part of Bondage)
    double bondage = calculateArabicPart(asc, saturn, mercury, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"束缚点", L"束", bondage);
    
    // 愚蠢点 (Part of Idiot)
    double idiot = calculateArabicPart(asc, mercury, mars, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"愚蠢点", L"愚", idiot);
    
    // 疯癫点 (Part of Madness)
    double madness = calculateArabicPart(asc, moon, mars, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"疯癫点", L"疯", madness);
    
    // 孤儿点 (Part of Orphan)
    double orphan = calculateArabicPart(asc, saturn, mercury, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"孤儿点", L"孤", orphan);
    
    // 自杀点 (Part of Suicide)
    double suicide = calculateArabicPart(asc, saturn, moon, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"自杀点", L"自", suicide);
    
    // 大屠杀点 (Part of Massacre)
    double massacre = calculateArabicPart(asc, mars, saturn, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"大屠杀点", L"屠", massacre);
    
    // 小丑点 (Part of Jester)
    double jester = calculateArabicPart(asc, mercury, venus, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"小丑点", L"丑", jester);
    
    // 种族点 (Part of Race)
    double race = calculateArabicPart(asc, sun, saturn, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"种族点", L"种", race);
    
    // 祭祀点2 (Part of Sacrifice2)
    double sacrifice2 = calculateArabicPart(asc, mars, venus, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"祭祀点2", L"祭2", sacrifice2);
    
    // 证人点 (Part of Witness)
    double witness = calculateArabicPart(asc, sun, mercury, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"证人点", L"证", witness);
    
    // 法官点 (Part of Judge)
    double judge = calculateArabicPart(asc, saturn, mercury, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"法官点", L"判", judge);
    
    // 法律点 (Part of Law)
    double law = calculateArabicPart(asc, saturn, sun, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"法律点", L"律", law);
    
    // 监狱点 (Part of Prison)
    double prison = calculateArabicPart(asc, saturn, moon, ARABIC_FORMULA_REVERSE);
    addArabicPart(arabicParts, L"监狱点", L"狱", prison);
    
    // 十字架点 (Part of Cross)
    double cross = calculateArabicPart(asc, mars, saturn, ARABIC_FORMULA_NORMAL);
    addArabicPart(arabicParts, L"十字架点", L"十", cross);
    
    // 太阳点 (Part of Sun)
    addArabicPart(arabicParts, L"太阳点", L"日", sun);
    
    // 月亮点 (Part of Moon)
    addArabicPart(arabicParts, L"月亮点", L"月", moon);
    
    // 水星点 (Part of Mercury)
    addArabicPart(arabicParts, L"水星点", L"水", mercury);
    
    // 金星点 (Part of Venus)
    addArabicPart(arabicParts, L"金星点", L"金", venus);
    
    // 火星点 (Part of Mars)
    addArabicPart(arabicParts, L"火星点", L"火", mars);
    
    // 木星点 (Part of Jupiter)
    addArabicPart(arabicParts, L"木星点", L"木", jupiter);
    
    // 土星点 (Part of Saturn)
    addArabicPart(arabicParts, L"土星点", L"土", saturn);
    
    // 计算每个阿拉伯点所在的宫位
    for (auto& part : arabicParts) {
        part.house = HouseCalculator::getPlanetHouse(part.longitude, houses);
    }
}

// 计算特定阿拉伯点
double ArabicPartCalculator::calculateArabicPart(double ascendant, 
                                             double point1, 
                                             double point2, 
                                             int formula) {
    double result;
    
    switch (formula) {
        case ARABIC_FORMULA_REVERSE:
            result = ascendant + point2 - point1;
            break;
        case ARABIC_FORMULA_NIGHT:
            // 夜间公式通常与白天公式相反
            result = ascendant + point2 - point1;
            break;
        case ARABIC_FORMULA_NORMAL:
        default:
            result = ascendant + point1 - point2;
            break;
    }
    
    // 确保结果在0-360度范围内
    result = fmod(result, 360.0);
    if (result < 0) result += 360.0;
    
    return result;
}

// 获取阿拉伯点列表
void ArabicPartCalculator::getArabicPartList(std::vector<std::wstring>& partNames) {
    partNames.clear();
    for (int i = 0; i < 82; i++) {
        partNames.push_back(std::wstring(arabicPartNames[i]));
    }
}

// 格式化阿拉伯点显示
std::wstring ArabicPartCalculator::formatArabicPart(const ArabicPart& part, int format) {
    static wchar_t szFormatted[64];
    
    switch (format) {
    case 0:
        // 常规格式化：度数、星座、分
        {
            double deg = fmod(part.longitude + 0.5 / 60.0, 360.0);
            if (deg < 0) deg += 360.0;
            int sign = (int)deg / 30;
            int d = (int)deg - sign * 30;
            int m = (int)(fmod(deg, 1.0) * 60.0);
            
            // 将char*转换为wchar_t*
            size_t len = strlen(signNamesEn[sign+1]) + 1;
            wchar_t* wideName = new wchar_t[len];
            mbstowcs(wideName, signNamesEn[sign+1], len);
            swprintf(szFormatted, 64, L"%2d°%ls%02d′", d, wideName, m);
            delete[] wideName;
        }
        break;

    case 1:
        // 以小时/分钟格式显示
        {
            double deg = fmod(part.longitude + 0.5 / 4.0, 360.0);
            if (deg < 0) deg += 360.0;
            int d = (int)deg / 15;
            int m = (int)((deg - (double)d * 15.0) * 4.0);
            swprintf(szFormatted, 64, L"%2dh%02dm", d, m);
        }
        break;

    default:
        // 以小数度数格式显示
        swprintf(szFormatted, 64, L"%.6f", part.longitude);
        break;
    }
    
    return std::wstring(szFormatted);
}

// 初始化阿拉伯点向量
void ArabicPartCalculator::initializeArabicParts(std::vector<ArabicPart>& arabicParts) {
    arabicParts.clear();
}

// 添加阿拉伯点
void ArabicPartCalculator::addArabicPart(std::vector<ArabicPart>& parts, 
                                     const wchar_t* name, 
                                     const wchar_t* shortName,
                                     double longitude) {
    ArabicPart part;
    part.name = std::wstring(name);
    part.shortName = std::wstring(shortName);
    part.longitude = longitude;
    part.house = 0; // 将在稍后计算
    parts.push_back(part);
}