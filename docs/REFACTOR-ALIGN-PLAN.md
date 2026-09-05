# AstrologAsStar 完全重构对齐方案（基线：Astrolog32 v3.51）

> 状态：**待确认草案**（2026-09-04）。本文档为重构的设计依据与实施路线图；
> 经确认后按 Git 技能规范分阶段落地，每阶段提交 + 双端/三端同步。
> 对齐目标 = **原版引擎计算能力**；GUI / 图形 / 档案库 / 解释文案等原版
> Windows 专属层**不在**对齐范围（新项目定位即"无 GUI 跨平台引擎"）。

---

## 0. 结论摘要

- 原版 `A32_V3_51_Proj`（E:\IT\astrolog\A32_V3_51_Proj，★正确基线）是 **~11.7 万行**的
  纯 Win32 SDK 单文件巨型应用（非 MFC）
  （astrolog.cpp 84,291 行 + newChart.h 22,986 行 + dbmanage.h 7,943 行 + 资源/地名库/星历文件），
  **纯计算能力远超**现重构版。
- 现重构版 `AstrologAsStar` 仅 ~1.07 万行，是"沿主链精简"的产物：
  **本命盘主链可用，其余能力要么是桩、要么是死代码、要么无入口**，且
  缺配置解析、无数值测试、编码与许可均有隐患。
- 最佳方案 = **以原版为"行为金标准"，在新架构内分阶段补全 + 金样对拍验收**，
  不搬运 Windows/GUI/DB 代码。保留 8 平台 CI 流水线与跨平台静态库 API 形态。
- 关键使能点：原版代码保留了完整**文本输出链**（ChartListing/PrintChart/FOutputData，
  `is.S=stdout` 输出、`-o` 写 UTF-8 文件），但 GUI exe 命令行模式不触发该链 →
  金样生成器 = **给原版源码加 console 入口，用宿主 VS2026 编译一次**（详见 §5.1 修正）。

---

## 1. 背景、目标与硬约束

### 1.1 背景
AstrologAsStar = 从 Astrolog32（5.40G 血统 + 6.00+ 片段 + 中文定制，内部版号
3.51，见 `version.h` `szVersionCore="3.51"`）重构出的**无 GUI 跨平台 C++11 引擎**，
当前形态：静态库 `astrolog32` + 4 个公开 API + 3 个演示 exe；SWE 以 git submodule
源码构建；CI 在单一 docker runner 上做 8 平台 × 3 档（dev/test/prod）矩阵构建。

### 1.2 目标
1. 与原版**引擎计算能力对齐**（不是文件对齐，是能力/数值对齐）。
2. 修复现存缺陷（宫位映射、节点/莉莉丝占位、桩函数、编码、许可等）。
3. **保留**：8 平台 CI 流水线、跨平台静态库/API、SWE 源码构建铁律、产物命名规范。
4. 建立**可长期演进**的测试网（数值金样 + 单测），杜绝"无测试重构"再发生。

### 1.3 硬约束（不可变）
- 无 GUI、无 Windows-only 依赖；保持 C++11；Linux/Windows/Android 交叉编译不破坏；
- CI 仍跑在 n8n VM 单一 docker runner 上（内存受限），单测必须轻量；
- SWE 一律源码构建（预编译二进制铁律，E30/E31）；
- `.gitlab-ci.yml` 由 `scripts/gen-gitlab-ci.py` 生成，平台矩阵改动走生成器。

---

## 2. 两侧工程解剖（事实基线）

### 2.1 原版 A32_V3_51_Proj（E:\IT\astrolog\A32_V3_51_Proj）★正确基线（2026-09-04 纠正）

> **位置纠错**：此前分析误用 `A32_V3_51_Proj_2022`（同族旧副本，含调试弹窗/多语言
> 发布资产）。用户指正**权威原工程 = `A32_V3_51_Proj`**（无后缀）。
> 经函数面比对（下详），**两版核心代码 100% 一致**（574 函数全同，_2022 仅多
> InitConsole/MyTextOutputA/MyTextOutputW 3 个调试函数 + 全局变量 `ignore1` 改名 +
> 注释差异），本方案差距矩阵/功能结论不受影响；仅路径、行号、exe 版本需按本表更新。
> **另修正**：原版为**纯 Win32 SDK 程序（UseOfMfc=false，零 MFC 头引用）**，
> 非"Windows MFC 应用"——GUI 层是手写 WNDCLASS/WndProc 经典 Win32。

| 资产 | 规模/说明 |
|---|---|
| astrolog.cpp | **84,291 行**（UTF-16）：引擎+GUI 交织单文件（WinMain 9780 / WndProc 11807；纯计算 CastChart 20629、DoReturn 17828、Action 17948、CastRelation 42411、DisplayGrands 36883、NCheckEclipse 38267、ChartListing 36076、ChartFirdaria 40357、ChartAstroGraph 40837、DisplayPrimDirs 33219、PrintChart 41477、FOutputData 49223） |
| newChart.h | 22,986 行（gb18030）：**面向对象进阶计算层**（内联类：Houses/Planet/Planets/两 Speculum/Fortune/MidPoints/ZodPars/Firdaria/Syzygy/Essentials/Accidentals/SecMotion/Transit/Profections/Decennial/ZodRelL1/**PrimDirs ~1.3 万行**/PDsInChart…），非绘图模块 |
| new.h | 784 行：印度盘/中文拼音等 |
| dbmanage.h | 7,943 行：SQLite 人物档案库（PERSON 表 + ListView + 增删改查 + .dat/.json 导入导出） |
| resources_en.h | 2,468 行：839 个 sXXXX 字符串 + 526 个 cmd 菜单命令 + 75 菜单/对话框资源 |
| atlas.h | 2.18MB：Walter Pullen 国际地名内嵌表（26,733 城）+ int/atlas/american、int/atlas/international 外置 |
| 编译 | astrolog32_en.vcxproj（v143/VS2022，Unicode，/SUBSYSTEM:Windows）：astrolog.cpp + cJSON.c + sqlite3\sqlite3.c + xmlParser.cpp + General\CharStrings.cpp + **内嵌 SWE 全套 swe*.c（2.10.03）** + resources_en.rc；Release_en/ 留有 2025-03-12 完整 .obj（可增量链接） |
| 数据资产 | int/ephemeris/（sepl_18.se1 484KB、semo_18.se1 1.30MB、seas_18.se1 223KB，DE431 系）；int/main/sefstars.txt（恒星表）；int/main/FIXSTARS.CAT；int/charts/（John_Lennon/Yoko_Ono.dat 样例盘） |
| 运行版 | int/main/astrolog32.exe（9.2MB，2025-03-12，**干净版**，无调试弹窗；Astrolog32.db 档案库、Help.chm 等随行） |
| 命令行 | 经典 astrolog 字母开关体系（FProcessSwitchesMain 21846；-M 宏、-Y 图类、#扩展开关）；**但为 GUI 程序：WinMain 解析开关后进消息循环，从不触发 Action() 文本输出** → 金样通道不能直接跑 exe -o（见 §5.1 修正） |

### 2.2 原版能力总览（对齐目标面）

- **天体**：118 对象槽 = 日/月/五星 + 天海冥 + Chiron + 谷神/智神/婚神/灶神 +
  南北交 + 莉莉丝 + 福点/Vertex/EP + 12 宫头 + **9 颗天王星族(Cupido…Proserpina)** +
  **75 颗恒星**（可选）；阿拉伯点**按需计算 177 个**（`cPart=177`）。
- **宫位**：16 系（Placidus/Koch/Equal(Asc)/Campanus/Meridian/Regiomontanus/
  Porphyry/Morinus/Topocentric/Alcabitius/Equal(MC)/Neo-Porphyry/Whole/Vedic/
  Null(整星座制)/Shripati）+ 3D 宫（RHousePlaceIn3D）。
- **相位**：18 型主次族 + 平行/反平行（declination）；主/次容许度表可自定义；
  格局识别码 acS..acK（stellium/大三角/T三角/Yod/大十字…）。
- **盘型/功能**：本命、行运、次限、太阳弧/月亮弧、PD（主限方向，黄道/宫位/入世
  多轴全矩阵）、法达 Firdaria、Profection、Decennial、黄道释放 ZodRelL1、
  日月返、合盘/组合盘/中点关系盘、泛音、移置（改地重算）、映点/反映点、
  十二分盘、九分盘(Navamsa)、行星时段、日/月食、生物节律、AstroGraph、
  恒星列表、日历/月历/星历表(ephemeris listing)。
- **输出**：文本长表 ChartListing、文本轮 ChartWheel、盘格 ChartGrid、
  相位表 ChartAspect(+Summary)、speculum 宫位表、格局文本；PS/WMF 图形。
- **应用层**：解释引擎（外库 .DAT 文本）、atlases 地名/时区、配置文本
  astrolog32.dat、语言 DLL。

### 2.3 重构版 AstrologAsStar 现状（实现度矩阵）

| 模块 | 判定 | 说明 |
|---|---|---|
| core/chart.cpp（397） | ✅ 真实现 | CastChart/ProcessInput/computeRiseSet；福点/南交补算；Dignify 真数据 |
| core/ephemeris.cpp（158） | ✅ 真实现 | SWE 循环计算；遗留 `..\main` Windows 反斜杠路径残留 |
| core/planet.cpp（1007） | ⚠️ 双引擎，Kepler 死 | SE 映射全（含 9 天王星族）；但 Kepler 引擎仅被桩函数调用 |
| core/houses.cpp（210） | ⚠️ 接线缺陷 | **hsEqualMC/hsWhole/hsNull 三系落到整宫 'A'**（Null 应为空宫） |
| core/aspects.cpp（605） | ⚠️ 有实现 | 18 型齐但默认只放行 5 主相位；关系盘 `\|\|` 判断语义可疑 |
| core/fixed_stars.cpp（374） | 🪦 死代码 | ComputeStars 完整但**零调用**，恒星全被 initEnv 屏蔽 |
| core/lunar_nodes.cpp（52） | 🪦 半桩 | 南交=北交+180 近似；真节点/真莉莉丝为占位；**无调用方** |
| core/progressions/synastry/transits.cpp（28~39） | 🪦 **假桩** | 三者同体，= 普通本命重算（不调 ProcessInput、不算宫头、无 JD 推进）；**无调用方** |
| utils/formatter.cpp / parser.cpp（5+5） | 🪦 纯桩 | 空文件；头文件也仅注释 |
| utils/TransU.cpp（132） | ⚠️ 真但脆 | setlocale 每调用一次（非线程安全）；注释乱码 U+FFFD |
| src/astrolog.cpp（5137） | ✅/🪦 混合 | ChartListing/ChartAspect/ChartAspectRelation/CastRelation/DisplayGrands/食/映点/十二分/九分 多数组件"移植但无开关/无入口"；PD/Firdaria 仅剩标题与注释；GetMainChartAspect 有 stdout 副作用 |
| include/core/options.h（613） | 🪦 **从未实例化** | Options 类无任何引用（仅被 .bak 引用） |
| config | ❌ 无 | 无配置解析器；`astrolog32.dat` 从未打开 → 宫位/黄道/天体集/容许度只能改常量 |
| test/（260） | ❌ 演示壳 | 3 exe 无断言、无数值校验、永远 exit 0 |
| CI verify | ❌ 存在性门禁 | 只查 .a 非空 + 头文件存在；**从不解包运行任何 exe**；无任何数值金样 |
| 许可 | ❌ 矛盾 | 文件头保留 Walter Pullen 5.40G/6.00 GPL 声明，根 LICENSE 却标 2025 iyxsh MIT |
| 中文/编码 | ⚠️ 双端风险 | Win 端 CP_ACP 转宽、Linux 端 C locale mbstowcs 失效；CI 不运行 exe 故未暴露 |

### 2.4 API 现状（astrolog_lib.h）

`initEnv / RParseSz / SetChartData(mode, ChartInput) / GetMainChartAspect /
GetChartAspectRelation / GetChartResult`。
- `GetMainChartAspect` = 本命位置 + 相位文本（含向 stdout 打印横幅副作用）。
- `GetChartAspectRelation` = CastRelation 但 `us.nRel` 默认 rcNone → 仅"盘2星体落
  盘1宫 + 相位表"，composite/midpoint 分支无入口。
- **没有任何盘型/配置参数**：无法切换行运/次限/返照/合盘、宫位系、黄道、天体集、
  容许度、日心/恒星黄道等 —— 这是与"引擎库"定位最不相称的架构缺口。

---

## 3. 差距矩阵（对齐动作总表）

| # | 能力域 | 原版 v3.51 | AstrologAsStar 现状 | 缺口 | 对齐动作 |
|---|---|---|---|---|---|
| A1 | 本命盘主链 | ✅ | ✅（基本一致） | 低 | 金样校准微差 |
| A2 | 宫位系统接线 | 16 系全 | 枚举 16，**实际 3 系错映射**（EqualMC/Whole/Null→整宫） | 高 | 修正 SwissHouse 映射 + 3D 宫复测 |
| A3 | 南北交点/莉莉丝 | 真交点 + 真莉莉丝远地点 | 已与原版对齐（2026-09-05 实证） | 完成 | SE 路由 + `-Yn`/`-YL` 已接，实证见 P1 清单 |
| A4 | 小行星/天王星族/恒星 | 全量可开关 | 恒星 `-U` 已对齐（2026-09-05 实证 128/128 行 9 位零差）；小行星/天王星族开关仍缺 | 恒星子项完成；余天体集开关 | 恒星接入 ComputeStars + `-U`；小行星/天王星族对象开关 = 天体集配置化（A4 子项 2） |
| A5 | 阿拉伯点 | 177 点按需 | 177 点表+引擎已落地（2026-09-05 实证 354/354 oracle 零差）；仅剩输出/展示路径 | 低（余展示接线） | 移植 tArabicPart 177 点表 + 引擎已毕；`-P` listing 展示路径待接 |
| A6 | 相位集/容许度 | 18 型+平行/反平行+自定义表 | 18 型枚举在、默认 5 主；`-RA/-RE` 屏蔽/启用已接（2026-09-05）；自定义相位名/角度/容差表待接 | 中 | 相位屏蔽配置化已毕；格局识别 API 入口 + 自定义表后续 |
| A7 | 格局识别 | ac 码 + 搜索 | DisplayGrands 在、无入口/无测 | 中 | 接 API + 金样 |
| A8 | 行运/次限/太阳弧 | Action/CastChart 标志真算法 | **假桩**（=本命重算） | 极高 | P2 重写（ProcessInput 已有 fProgressUS 骨架） |
| A9 | 日月返 | DoReturn | ❌ | 极高 | P2 移植 |
| A10 | 合盘/组合盘/中点 | 多关系盘 | 仅"落宫+相位表"；composite/midpoint 分支无入口 | 高 | P2 接 CastRelation 全分支 + API 参数 |
| A11 | 映点/反映点/十二分/九分 | 有 | 真算法残存但无开关 | 中 | P2/P3 接线 |
| A12 | 法达/Profection/Decennial/ZodRel/行星时段/日历月历星历表/生物节律 | 有 | ❌（Firdaria 仅标题） | 高 | P3 移植 |
| A13 | PD 主限方向（黄道/宫位/入世全矩阵） | ~1.3 万行 | ❌ | 极高 | **P4 可选大项**（成本最高，见决策点） |
| A14 | AstroGraph 世界地图 | 有 | ❌ | 高（量小但依赖地图坐标） | P4 可选 |
| A15 | 日食/月食表 | 有（NCheckEclipse…） | 几何函数在、无入口无测 | 中 | P2/P3 接线 |
| A16 | 配置/选项 | astrolog32.dat + 开关体系 | **无解析器、Options 类死** | 极高 | **P1 建立 Config**（先于一切功能补齐） |
| A17 | 星历数据策略 | .se1 + Moshier 回退 | 仅 SWIEPH 无文件（远日期/星历缺失区不稳） | 中 | 复用宿主 .se1 或 Moshier 回退 + 环境变量 |
| A18 | 输出形态 | 文本长表/轮/格/相位/speculum/格局 | ChartListing/ChartAspect/OutStr* 在但契约未稳定 | 中 | 稳定输出层 + CLI（见 5） |
| A19 | 数值测试 | 无（GUI 时代） | 无 | 极高 | **P0 金样对拍 + 单测网** |
| A20 | CLI | 经典开关 | 3 个固定参数演示壳 | 高 | P0 落地真 CLI（对拍/交付兼用） |
| A21 | 编码/中文 | 语言 DLL + 宽字符 | 双端 locale 隐患、乱码注释 | 中 | P0 统一 UTF-8/UTF-16 边界 |
| A22 | 许可 | GPL/5.40G 条款 | MIT 冲突 | 高 | P0 定 License+NOTICE（见决策点） |
| A23 | API 副作用 | — | 打印横幅等 stdout 副作用 | 中 | P0 净化（回调/去打印） |
| A24 | 死代码清理 | — | .bak 22,993 行头、假桩、Kepler 死引擎 | 低 | P0 清理/标注 |

---

## 4. 根因结论

为什么重构版"缺这么多"：
1. **精简式重构只保主链**：把 GUI 单文件里"本命盘文本输出"这条路抽出来跨平台化，
   其余功能（即便已在同文件里）多数以"注释掉 / 删掉 / 留桩"处理，且**没有保留
   行为基线**——无从知道砍掉后算得对不对。
2. **Options/配置层没接线**：原版的自由度都靠 `us`/配置文件/开关，重构时开关体系
   没迁 → 大量真实现（CastRelation 各分支、映点、格局、食）变成"无入口死代码"。
3. **测试与 CI 只做存在性门禁**：3 个演示 exe 永远 exit 0，CI 从不执行 → 桩函数、
   错映射、编码问题全部隐身。
4. **模块化只拆了壳**：目录结构现代化了，但全局单例（us/is/ciCore/cp0..cp4）
   与函数间隐式耦合照搬，模块间仍是"原文件分段"，PD 等复杂状态机难以直接嵌入。

---

## 5. 目标架构与设计原则

```
astroproject/
├─ include/astrolog.h / astrolog_lib.h   # 公共 API（新增盘型/配置入口，旧 API 保留兼容）
├─ include/core/…
│   options.h        # Options 类复活：唯一配置对象（取代散落全局 us 的读入口）
│   config.h         # 解析默认值/JSON/env（P1）
├─ src/
│   core/
│     chart.cpp      # 本命核心（保留）
│     relation.cpp   # P2: CastRelation 全分支正式化（行运/次限/太阳弧/返照/合盘/组合/中点/映点…）
│     returns.cpp    # P2: 日月返 DoReturn
│     events.cpp     # P3: 法达/Profection/Decennial/ZodRel/行星时段/日历/星历表/食
│     arabic_parts.cpp # P1: 177 点表
│     stars.cpp      # P1: 恒星启用（数据文件可选加载）
│     aspects.cpp    # 增强：相位集/容许度配置化 + 格局（P1-P2）
│     primary_directions.cpp  # P4（可选）: PD 全矩阵
│   util/…           # parser/formatter 真实现；TransU 去 setlocale
│   cli/main.cpp     # P0: astrolog32-cli（类原版开关 + JSON/text 输出）
├─ test/
│   golden/          # P0: 原版 -o 生成的金样 .txt + 对拍驱动
│   unit/            # P0: 轻量断言式单测（CTest 接入）
├─ swe/              # submodule 保持源码构建
├─ data/             # P1: 可选运行时数据（.se1 复用策略、恒星表、阿拉伯点表内嵌）
└─ CMakeLists.txt    # 新增 target：astrolog32-cli + 单测；install 规则扩展
```

设计原则：
1. **原版即规格**：任何"该算什么"以原版行为为准；任何"怎么算"以可测试的数值一致为准。
2. **先配置后功能**：P1 先打通 Options/Config 再到全功能（否则每加一个功能都要开洞）。
3. **先金样后实现**：每个能力先固化金样（红），再实现到绿。
4. **不搬运**：MFC/GDI/资源 DLL/SQLite 档案/解释文案库/语言 DLL 一律不迁；
   解释文案若未来需要 → 独立数据层 + 许可单列（决策点）。
5. **兼容旧 API**：`SetChartData/Get*` 签名不动，新增
   `SetChartOptions(const ChartOptions&)` / `GetChartText(ChartType,…)` 等扩展入口；
   `CastChart` 等内部全局单例保留但收敛读写点，文档明示非线程安全
   （并发抽取为可选项，见决策点）。

### 5.1 金样对拍机制（验收的锚）★2026-09-04 修正

**重要实证修正**：原版 `astrolog32.exe` 是 GUI 程序——命令行传 `-o`/日期开关后，
WinMain（L9780）调 FProcessCommandLine（L18191）→ FProcessSwitchesMain（L21846）解析
开关即进入**消息循环**，从不调用 Action()（L17948）→ 实测 `-o out.txt` 运行 8s 后
进程仍存活、文件不生成（int/main 与 ch/main 两个 exe 均如此，_2022 调试版还弹窗）。
因此"直接跑原版 exe -o 产金样"**不可行**。

**可行方案（金样生成器 = 原版 console 变体）**：
- 原版文本输出链完整且**不依赖 GUI**：ChartListing（L36076）/PrintChart（L41477）以
  `is.S=stdout` 输出文本；`-o` 经 FOutputData（L49223）用 CreateFileW 写 UTF-8（BOM+@0102
  头）；`-os` 将文本重定向文件（is.szFileScreen）。GUI 只是把 stdout 接到窗口缓冲。
- 做法：以原版 astrolog.cpp 为底，**新增一个 console 入口**（仿经典 Astrolog 文本模式
  main：解析开关 → 若非图形模式调 Action()/PrintChart → `-o` 落盘 → exit），
  用宿主 **VS2026 Community**（v143 工具链，Release_en 已有全部 .obj 佐证可编译）编译
  出 `astrolog32-golden.exe`。产物不入 AstrologAsStar 仓库，仅宿主生成金样用。
- 命令形如：`astrolog32-golden -qy 1958 -qm 7 -qd 4 -qt 12:01 -z0 -Y0 -o <out>`；
  若 console 改造有 GUI 初始化阻碍（LoadLanguagePack/字体等），退路 = 用
  `SendMessage` 驱动 GUI 的 cmdSaveText（40200）菜单命令（GUI 自动化，不推荐首选）。
- 生成：按"能力×代表盘"矩阵批量产出（本命/宫位系/推进/合盘等，覆盖中文名/东西经/
  夏令时/负时区/近两极/±100 年）；归一化（去 @0102 头/版本行/日期行/空白）后提交
  `test/golden/*.golden.txt`。
- 校验：CI verify 阶段（Linux 容器）跑 `astrolog32-cli` 同输入 → 归一化 → diff 金样。
  跨平台（mingw/android）保持编译 + 冒烟，数值对拍以原生 6 平台为准。
- 局限与对策：金样只取数值行（位置/宫头/相位/容许度等），排版不比对；历元选 .se1
  覆盖范围（1900–2100 稳妥，DE431 更宽）；sweph 同源（原版内嵌 2.10.03，AstrologAsStar
  submodule 版本需对齐或金样用 Moshier 区段保证可比）。

**✅ 2026-09-04 实施状态（console 变体已跑通）**：
- 工具链改为 **mingw32（32-bit x86，与原版 exe 对齐）**——VS cl 缺 SDK/ucrt 不可用；
  源码工作副本在 `E:\data\astrolog_golden\src`（UTF-16/GB18030→UTF-8 转码 + 最小 mingw
  适配：`-fno-operator-names`/`-D_stdcall`/`-fpermissive`/`-std=c++14`/`-fexec-charset=GBK`
  `-include mingw_patch.h`；`x_memset` __asm→memset；dbmanage.h throw 改
  std::runtime_error；about_dialog_text 改 extern；min/max 混型补转型；htmlhelp stub）。
- 产物 `run\astrolog32-golden.exe`（console 全静态）+ `run\ephemeris\*.se1`。
- **命令行语法（实证）**：`-qb M D Y T dst zon lon lat` 全参（lon 西正东负、zon 东正
  西负）；`-os <file>` 落文本（UTF-16LE）；**原版 `case 'o'` 实际消费文件名**
  （`is.szFileOut=A2U(SzPersist(argv[1]))`），且原生支持 `-o0` 开关
  （`SwitchF(us.fWritePos)` + `SwitchF(us.fWriteFile)`）。但原版 GUI 的 `Action()` 只在
  `FRedraw()`（窗口重绘）中被调用，headless 下进消息循环挂起、不自动落盘 → 故用
  console 变体 `golden_main` 直接驱动 `Action()`；它强制 `fGraphics=FALSE`（原版静态
  初始化=TRUE→走 FActionX 而非文本 PrintChart）+ `ciNatal=ciCore`（缺则 PrintHeader
  哨兵 -1 崩溃），并用自有 `--o0 <path>` preswitch 预设 `fWriteFile/fWritePos/is.szFileOut`
  直驱 `FOutputData` 输出 @0203。
- **✅ 首批 8 份 @0203 数值金样已归一化入库 `AstrologAsStar/test/golden/`**（北京/纽约
  夏令时/格林尼治 1900&2100/悉尼夏令时/北极圈 78N/上海/洛杉矶负时区），UTF-8 LF、去 BOM/
  CRLF/行尾空白/空行，`# golden:`+`# cmd:` 头 + @0203 主体；同输入双跑字节一致。
- **✅ 金样正确性三层验证已通过**：① printf 语义 artifact 修复（`_stprintf`→`_swprintf`
  1690 处，还原 MSVC 宽串语义，应用名 `Astrolog32`/星座 `Can` 与原版一致）；② 修复前后
  160 浮点 token 逐位一致；③ 独立 oracle（pymeeus 纯 Python 第二实现）对 8 场景×10 主
  行星"星座归属一致性"检查 8/8 sign_mismatch=0，无错配/乱码。详见 `test/golden/README.md`
  与 `test/oracle_check.py`。

---

## 6. 分阶段实施计划

### P0 · 地基与测试网（先决，不可跳）
- [x] 决策点确认（§8 第 1-4 项已拍板：P0-P3 核心对齐不含 PD/AstroGraph、正式 astrolog32-cli、金样入库 CI 对拍、衍生双段许可；5/6 线程安全/应用层内容留待 P1 前）。
- [x] 许可处理：LICENSE 双段（上游 GPL-2.0-or-later 聚合声明 + 新代码 MIT）+ NOTICE 溯源（71a329e）。
- [x] 金样生成器：给原版 astrolog.cpp 加 console 入口（`golden_main.cpp` 单 TU include + 复刻非 GUI 初始化），**mingw32 x86 编译**（VS2026 cl 缺 SDK/ucrt 实测不可用，改 mingw32 与原版 exe PE 对齐）产出 `astrolog32-golden.exe`；跑通 8 个代表输入（中文名/东西经/夏令时/负时区/近两极 78N/±100 年），首批 8 份 @0203 数值金样已入库 `test/golden/`。
- [x] CLI `astrolog32-cli` 落地（类原版开关子集 `-qb M D Y T dst zon lon lat` + `--json`/`--text`），复用公开 API `initEnv/RParseSz/GetChartMachineText/GetChartJSON`（`astroproject/cli/main.cpp` + `include/astrolog_lib.h` 扩展 + CMake target）。
  - `--text` 复刻原版 @0203 writer：金样精确对象集（1-10,16,19,22-33 + 16 占位星行常量）、Fortune/宫位 speed 强制 360、`/YF` 字段布局逐字节对齐；`initEnv()` 加 `g_fSilent/SetSilent()` 抑制版本横幅污染 stdout。
  - **引擎修复**：Fortune 昼夜反演重新启用（`chart.cpp` CastChart，原注释掉的 `nArabicNight` 逻辑恢复，`neg`→`negV`），`isDayBirth` 由已有 `computeRiseSet()`（swe_rise_trans）在 CastChart 顶部计算 —— 修后 8 份金样 Fortune 全部对齐（此前恒为 Asc+Moon−Sun，金样 6/8 为夜生式 Asc+Sun−Moon）。
  - 验证：`test/verify_cli.py` 逐字段容差对拍（经度/纬度严格 ≤1e-7°，speed ≤1e-3°，行星/NoN/占位星 distance ≤1e-6）→ **8/8 PASS**，实测 max_lon_err ≤5.2e-10°（9 位小数对齐）。
  - 已知引擎 GAP（P1 引擎对齐追踪，验证脚本显式跳过）：Fortune/宫位 cusp 的 spacex/y/z 未填充 → distance 恒 0（金样为随盘变化的 ~1.0 AU 地日距）。
- [x] CTest 接入 + `test/unit`（API 冒烟、输入校验、编码往返）+ CI verify 升级为"运行 CLI → diff 金样"。
  - CMakeLists：`enable_testing()` + `astrolog32_unit`（`test/unit/unit_smoke.cpp`，无 gtest 依赖纯断言）
    + `golden_diff` CTest（`Python3 + test/verify_cli.py + $<TARGET_FILE:astrolog32-cli>`）。
  - `unit_smoke.cpp` 五组：RParseSz 解析、@0203 结构（40 行/占位星/For+cusp speed=360/999）、
    JSON 形态（40 objects+houses）、非法输入不崩溃、中文名 UTF-8 往返。本地 `ctest` **2/2 PASS**。
  - CI：`gen-gitlab-ci.py` 新增 `golden_diff_{dev,test,prod}` job（stage verify，needs:[] 与制品解耦，
    builder-ubuntu-22.04 镜像内 cmake 构建 CLI → `python3 test/verify_cli.py`；CLI 自带 Swiss Moshier
    回退无需 .se1，已实测 8/8 一致）。重生成 `.gitlab-ci.yml`（+75 行，仅新增 3 job）。
- [x] P0.5 清理（#134）：`git rm` 孤儿 22993 行 MFC 头 `include/core/newChart.h.bak`（重构后无真身/无引用）；progressions/synastry/transits/lunar_nodes 四模块零调用（实测全仓无引用方）→ 顶部加 **STUB 横幅**（保留作 P2 重写骨架）；`planet.cpp ComputePlanets()` 旧 Kepler 引擎仅被上述 STUB 引用 → 加 **LEGACY 死代码横幅**（删除条件 = P2 完成 STUB 重写）；`SetChartData/GetChartResult/GetMainChartAspect/GetChartAspectRelation` 的调试 `wprintf`/`ChartListing` stdout 副作用全部纳入 `g_fSilent` 门控（静音默认 false，旧 demo 行为不变）；`src/utils/TransU.cpp` 头部 7 处 GBK 误读损坏注释重写为规范 UTF-8；`ephemeris.cpp` Windows 反斜杠目录表清理为跨平台正斜杠 + 惰性语义注释。回归：全量构建零 error、ctest 2/2、金样 8/8（max_lon_err ≤3.5e-10°）。
- 验收：CI 绿 + 首批金样 diff 全过 + 旧 API 行为不变。

### P1 · 配置层 + 本命盘修正
- [x] **P1.1 Config 层（A16 闸门，batch-1）**：新建 `include/core/config.h` +
  `src/core/config.cpp` —— 原版 astrolog32.dat 格式开关解释器（`ConfigProcessTokens`/
  `ConfigLoadFile`，默认值=原版即 us 静态聚合，未覆盖不变），词汇 = 原版
  FProcessSwitchesMain 子集（用户拍板：dat 原版格式 + P1 特性域 ~25 开关，非 JSON）：
  `-c <n>` 宫位系 / `-s` 恒星黄道族（offset、-sr/-sh/-sd/-sz/-sm）/ `-h <n>` 中心天体 /
  `-P` 阿拉伯点族（<n>/z/n/f/0）/ `-Y` 族（n 真节点、L 真莉莉丝 oscLilith、c/d/t/C/H）；
  未知/未实现开关安全 no-op（与原版容错分层一致）。CLI 接线：`--cfg <file>`（dat 先加载、
  命令行开关后覆盖）+ 引擎开关透传（`-c 6 -Yn -P 20 …`）。单测 `astrolog32_unit_config`
  （CTest unit_config）8 组断言。**交叉对拍实证**：`-c 6`(Porphyry) 宫头与原版 golden runner
  `astrolog32-golden.exe --o0` **9 位小数逐位一致**（Asc..12th），行星/福点仅末位噪声；
  dat 文件路径与直传 `-c 6` 输出全等；非法参数报 `(file … line N)`。回归：ctest 3/3、
  金样 8/8 不破。注：Options 类（MFC 默认表/显示参数）保持休眠 —— 引擎配置由 `us` 单例 +
  本解释器承担，P1.5 阿拉伯点表头化时再行归并。
- [x] A2 宫位三系修正（hsNull 空宫语义）—— **2026-09-04 修复**：根因 = houses.cpp SwissHouse 把
  Swiss 无对应的 hsEqualMC(10)/hsWhole(12)/hsNull(14) 落 `default:'A'` 后，原版的
  `if (ch=='A') ComputeHouses(housesystem)` 后处理在重构中被**注释删除** → 三系退化为
  swe 'A' 等宫。修复 = 恢复该分发并移植原版 `HouseEqualMidheaven()`（cusp=MC−270+30(i−1)）/
  `HouseWhole()`（cusp=整宫 0°，宫 1=Asc 所在星座）/`HouseNull()`（cusp=Sign2Z(i)）三算法。
  实证：**16/16 宫位系 × 3 盘（bj/bombay/syd）与原版 golden runner 逐位一致**；ctest 3/3 +
  金样 8/8 回归不破。3D 宫复测归 P1.7 矩阵。
- [x] A3 真节点/真莉莉丝（#136）—— **2026-09-05 实证收尾**：① 引擎核查——planet.cpp
  `astrolog_object_2_SE_object()` 的 NoNode→`us.fTrueNode?SE_TRUE_NODE:SE_MEAN_NODE`、
  Lil→`oscLilith?SE_OSCU_APOG:SE_MEAN_APOG` 与 golden 原版 astrolog.cpp:76381/76385 **逐行一致**；
  SoNode 派生（chart.cpp：NoNode+180 / vel 复制 / lat 不处理）与 golden:20740 逐行一致；金样 8/8 中
  的 NoN(mean node) 9 位零差已覆盖默认链。② 开关——`-Yn`(fTrueNode)/`-YL`(oscLilith) 已于 P1.1 接入
  config，全局默认 `oscLilith=0`（mean，同 golden）；unit_config 2 组 toggle 断言在册。③ **交叉对拍**：
  golden runner 开启对象 17/18（`-YR 17 17 0 -YR 18 18 0`）得 4 变体锚点：mean NoN=27Lib35.817169449、
  `-Yn`=28Lib5.270379889（mean↔true 差 ~29.5′）、`-YL` Lil 15Ari0.524→23Ari21.985（差 ~8.6°，速度变负），
  SoN=NoN+180。本地 CLI 四变体同命令对拍：**mean 9 位零差**；`-Yn` 真节点 bj-1958 差 1.5e-4°(0.55")
  但 syd/ny 仅 ~3e-6°(0.01")——归因**两侧 swe 2.10.03 源码文件漂移**（sweph.c 34 行 / swemmoon.c 12 行 /
  swephlib.c 100 行 diff，mean/解析路径不受影响）；**同表环境实证**（.se1 可达）golden 与本地 CLI 逐位
  一致（5.485724545）→ 路由与 swe 版本等价，无逻辑错。④ 顺带还原 `initEnv()` 曾强制 `eepp=1`(SWIEPH)
  的硬编码为原版 `eepp=-1`（golden 从不强制；实证本地缺表 fallback==显式 MOSEPH，无数值影响，
  .se1 存在时自动用表）。回归：ctest 3/3、金样 8/8 不破。遗留：`-YR` 对象开关启用属 A4 天体集范围。
- [x] A4 恒星启用 `-U`（天体集子项 1）—— **2026-09-05 实证收尾**：① 语义定位——金样 40 行对象集中
  16 个恒星占位槽（45/46/47/52/57/67/74/79/81/82/91/93/99/104/114/117）原版默认输出
  `0 Ari 0, 0, 57.295779513 999`，仅 `-U` 填真实恒星；A4 = 把原「零调用」的 `ComputeStars`
  （fixed_stars.cpp，读内嵌 75 星 J2000 赤经赤纬表，sefstars.txt 存在则 swe_fixstar 覆盖）接入主链。
  ② 接线——`settings.h`：**US 结构体末尾**新增 `bool fAllStar`（★勿插中部：US 静态聚合初始化
  `US us={…}` 按成员序对位，插入早段会使后续 `1.0`(double nHarmonic) 错位落到 `int objOnAsc`
  触发 `-Wnarrowing`，重构教训已录）；`config.cpp`：`case 'U'` 置 `us.fAllStar=1`，`-Uz/n/b/l/p`
  同时记 `us.nStar`（排序模式）；`chart.cpp`：`CastChart` 在 `ComputeWithSwissEphemeris(is.T)` 后调
  `ComputeStars(us.fSidereal ? 0.0 : -Off)` —— **SD 必须传 `-Off` 而非 0**（初版传 0 致 16 星黄经
  整体偏移 -24.16°：`#define rEpoch2000 (-24.736467)` 常量外还需 tropical 岁差项 SD=+ayanamsa
  抵消常量并施加 J2000→盘面剩余岁差，与原版 golden astrolog.cpp:20783 调用**逐字一致**）；
  `astrolog.cpp`：`GetChartMachineText` 占位槽按 `us.fAllStar` 分流——置位输出
  `cp0.longitude/latitude/vel_longitude` 真实值（速度 0.000037909=岁差速率，距离仍 999），默认
  保持占位（金样不破）。③ **交叉对拍**：golden runner `--o0 -U` × 8 盘（1900/1958/1969/1988/
  1999/2000/2020/2100，含南半球/夏令时/负时区/78N 高纬）= **16×8=128 恒星行逐字节 9 位一致**
  （黄经/黄纬/速度/距离全同）；行星仅既有末位噪声，福点/宫头距离跳过（engine gap，verify_cli
  容差内）。④ unit_config 新增 `-U`/`-Uz`/`-Ub` 断言（fAllStar 置位 + nStar 排序 + restore
  回滚）；ctest 3/3 + 金样 8/8 不破。遗留：小行星/天王星族(11-15/34-42)对象级开关 = A4 子项 2
  「天体集开关配置化」，与 A5 同批或后置。
- [x] A5 阿拉伯点 177 表 + 引擎（P1.5 首步）—— **2026-09-05 落地**：① 数据——
  `src/core/arabic_parts_data.inc` 177 行 {form,name} 逐字转录自原版 `ai[cPart]`（原版
  `#ifdef ARABIC` 恒开；form=11 字符公式 DSL = 3 组 [mod][obj10][obj1]，name 即原版
  tArabicPart 语言表内容），表头化入 `include/models/arabic_parts.h`（`cPart=177` 归口，
  astrolog.cpp/config.cpp 本地宏留 `#ifndef` 兼容）。② 引擎——`src/core/arabic_parts.cpp`
  `ComputeArabicParts(rPart[177])` 数值核心逐行对齐原版 `DisplayArabic()` 取位段（golden
  A32_V3_51 astrolog.cpp:33295）：修饰符 h 宫头 / r 宫头星座守护星 / j 宫头+10° / H 落宫宫头 /
  R 落宫星座守护星 / D 所落星座守护星 / '0'-'3' 常量度数 / ' ' 普通对象或 F(福点)/S(精神点)
  前序递归；`rCur = rBit1−rBit2`；`form[9]=='F'` 福点族日夜翻转（条件同 CastChart 内联）；
  `rPart[i]=Mod(rCur+rBit0)`；屏蔽对象/引用未算前序点 → 槽保持 -360 哨兵。依赖
  cp0.cusp_pos/house_no、rules、ignore1、isDayBirth（CastChart 后就绪）。
  ③ **验证**——`test/unit/unit_arabic.cpp`：bj/la 两盘 apFor=0 == CastChart 内联
  cp0.longitude[oFor]（bj golden 锚点 226.637503635117° 零差）+ apSpi==Asc±(Sun−Moon) +
  全量 ∈[0,360)；`test/verify_arabic.py` python 独立重解公式 DSL 与引擎逐点对拍
  **354/354 ≤1.5e-11°**（含 H/R 落宫、F/S 递归、常量度数与日夜翻转；python 端曾因
  0°Ari 环绕宫未线性化误判落宫，已修）。ctest 5/5（新增 unit_arabic + arabic_oracle）。
  遗留：阿拉伯点**输出/展示路径**（原版为文本 listing，由 `-P` 起 us.nArabic 触发，
  非 @0203 机器行）+ `-Pz/n/f` 排序接线 → 与 A6 相位集或 P1.7 矩阵同批。
- [x] A6 相位屏蔽配置化（A7 格局 API 前置）—— **2026-09-05 落地**：`ignoreA[]` 由 const
  改可写（aspects.cpp/ aspects.h），config 层新增 `-RA <asp>…`（屏蔽 ignoreA=1）/
  `-RE <asp>…`（启用 ignoreA=0），镜像原版 astrolog.cpp:22779/22799 语义（pmAspect 数值、
  越界报错、非数字停消费、无数字 no-op）；`UpdateAspectCount()` 按启动公式重算
  `nAsp = cAspect − 屏蔽数`。默认 ignoreA 仍屏蔽 6..18 → nAsp 默认 5 不变（金样 16/16、
  ctest 5/5 回归通过）。unit_config 新增 -RE 6（nAsp=6）/ -RA 3（nAsp=4）/ 越界报错 /
  无参 no-op / -Rq no-op 断言。遗留：自定义相位名/角度/容差表（-Aa/-Ao/-Am）与 A7
  格局识别 API 入口（DisplayGrands 已存在、无入口/无测）。
- [ ] A7 格局识别 API 入口（DisplayGrands 接线/无测）+ A6 余项（自定义相位名/角度/容差表）。
- [ ] 金样：本命盘 × 宫位系(16) × 天体集 × 容许度矩阵。
- 验收：本命盘数值与金样逐项一致。

### P2 · 动态盘第一梯队（核心业务盘型）
- [ ] relation.cpp：CastRelation 全分支正式化；`ChartType` API 参数（行运/次限/
  太阳弧/月亮弧/合盘/组合盘/中点/映点/移置）。
- [ ] returns.cpp：日月返（含双月返 IsDoubleReturn 逻辑）。
- [ ] A15 日月食表入口。
- 验收：与金样一致（覆盖代表日期 ±100 年）。

### P3 · 第二梯队（时间主星族）
- [ ] 法达 Firdaria、Profection、Decennial、黄道释放 ZodRel、行星时段、
  日历/月历/星历表 listing、生物节律（纯计算段）。
- 验收：金样一致。

### P4 · 高精/可选大项
- [ ] PD 主限方向全矩阵（若决策要做；移植量 ~1.3 万行内联逻辑，需状态机重构）。
- [ ] AstroGraph（若决策要做）。
- 验收：金样一致（PD 以原版对拍为准）。

> 每个 P 阶段结束：更新 README/能力矩阵 + 提交（Git 技能规范）+ 双端推送 +
> 记忆归档。P1~P4 均**不得**破坏旧 API 与平台矩阵。

---

## 7. 风险与对策

| 风险 | 对策 |
|---|---|
| 原版 GUI exe 无法命令行批处理输出（WinMain 进消息循环不触发 Action） | 金样生成器 = 原版源码加 console 入口 + 宿主 VS2026 编译；文本输出链本就不依赖 GUI（is.S=stdout） |
| PD/法达等依赖原版全局单例状态机，移植易引入漂移 | 以原版 console 变体对拍为准 + 单元化（输入状态显式化）；PD 放 P4 |
| 远日期（<1800/>2400）sweph 文件缺失 | Moshier 回退 + 金样限 .se1 可靠区间 |
| 中文/编码在 Linux CI 从未被跑 | P0 加"中文输入→CLI→UTF-8 输出"CI 用例（容器内 zh_CN.UTF-8 或显式 UTF-8 管道） |
| 许可风险（再分发含上游代码/文案） | P0 处理；解释文案/恒星释义若入包需逐项核上游许可 |
| runner 内存受限、8 平台×3 档矩阵时间 | 单测/对拍只进原生平台 verify 的轻量 job；不做全矩阵测试 |

---

## 8. 决策点（需用户拍板）

1. **对齐范围与深度**：
   A) 分阶段核心对齐（P0–P3：本命/行运/次限/太阳弧/返照/合盘/组合/中点/法达等 + 修缺陷）——**推荐**；
   B) 全量含 P4（PD 主限全矩阵 + AstroGraph，成本与周期显著更高）；
   C) 仅修缺陷 + 配置化（P0+P1，最小改动）。
2. **正式 CLI**：是否新增 `astrolog32-cli`（对拍/交付/调试三用，含 JSON 输出）？
   要（推荐）／ 不要（仅 API + 单测内部驱动）。
3. **金样对拍**：是否接受"金样 = 原版源码 console 变体（宿主 VS2026 编译 astrolog32-golden.exe）生成、提交仓库、CI 对拍"作为验收机制？（已确认替代"跑 GUI exe -o"）
   接受（推荐）／ 只要手工对照／ 不做数值对拍（仅单测自洽）。
4. **许可**：LICENSE 改为衍生双段（保留上游声明 + NOTICE + 明确 MIT 仅覆盖新代码）？
   改为衍生合规（推荐）／ 维持现状 MIT（接受再分发风险）。
5. **线程安全**：本阶段 API 维持"单线程全局单例"（文档明示）还是投入抽取 ChartContext 支持并发？
   维持单线程（推荐）／ 并发化。
6. **解释文案/档案库等原版应用层内容**：一律不迁移（推荐）／ 需要解释文案（另行评估许可与数据层）。
