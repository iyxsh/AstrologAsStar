# test/golden — Astrolog32 v3.51 数值金样（P0.2）

本目录存放对齐验收用的**数值金样**：由原版 Astrolog32 v3.51（CANON 工程
`A32_V3_51_Proj`）源码的 console 变体产出，作为 `astrolog32-cli` 逐项 diff 的行为锚点。

## 文件约定

- 每份金样 = `# golden:`（中文场景说明）+ `# cmd:`（复现命令）+ `@0203` 位置表主体。
- 格式：UTF-8、LF、无 BOM、无行尾空白、无空行（`gen_goldens.py` 归一化产物）。
- 主体是机器可读 **@0203 position table（9 位小数）**，优于文本 listing 的角分精度，
  且不携带文本排版头（规避 PrintHeader 宽字符 cosmetic 截断）。

## 来源与复现（宿主，不入仓库）

生成器在宿主 `E:\data\astrolog_golden\`（mingw32 console 变体 `astrolog32-golden.exe`
+ `gen_goldens.py`），详见其 README。重建 8 份金样：

```bash
cd E:\data\astrolog_golden
python gen_goldens.py E:/data/gitCode/AstrologAsStar/test/golden o0
```

命令行语法实证（供 CLI 对拍复用）：`-qb M D Y T dst zon lon lat` 全参（lon 西正东负、
zon 东正西负）；`-os <file>` 落文本（UTF-16LE）；数值走 golden_main 自有 `--o0 <path>`。

## 场景覆盖（8 份）

| 文件 | 场景 | 覆盖点 |
|---|---|---|
| bj-1958-1204 | 北京 1958-07-04 12:01 CST(+8) | 东经/北半球/正时区 |
| ny-2000-dst | 纽约 2000-06-15 EDT(-4) | 夏令时 dst=1 |
| lon-1900 / lon-2100 | 格林尼治 1900 / 2100 | ±100 年历元边界 |
| syd-1999-dst | 悉尼 1999 夏令时 | 南半球 + 夏令时 |
| arctic-2020 | 北极圈 78N | 近两极（宫位语义需 CLI 复核） |
| shanghai-1988 | 上海 1988 | 东经代表性 |
| la-1969-negzone | 洛杉矶 1969 负时区 | zon<0（西八区 PST） |

## 对拍用法（P0.4 CI 接线）

`astrolog32-cli`（或原生 CLI 变体）以 `# cmd` 行同输入运行 → 归一化到同一约定 →
`diff` 金样。注意：sweph 历元须对齐（金样用原版内嵌 2.10.03 的 .se1 区段 1900–2100）。

## 正确性验证（金样可信度）

金样本质是**原版 `astrolog.cpp` 直接 `#include` 编译**的 console 变体产物——计算代码
（星历 / 宫位 / 坐标）与原版 100% 同源，仅 I/O 入口壳不同。因此"金样 = 原版行为"在源码
层面成立。围绕它做了三层验证：

1. **printf 语义 artifact 修复（关键）**：初版 mingw 构建因 `swprintf` 窄串语义，把
   `@0203  ; A chart positions.`、星座 `C` 等宽串截成首字符。已通过源码级
   `_stprintf`→`_swprintf`（1690 处）修正为 MSVC 宽串语义，应用名 `Astrolog32` / 星座
   `Can` 已与原版一致。
2. **数值稳定性**：修复前后 160 个浮点 token 逐位一致；同输入双跑字节一致。
3. **独立 oracle（pymeeus，纯 Python 第二实现）交叉验证**：对 8 个场景各 10 主行星做
   "星座归属一致性" sanity check，**8/8 场景 sign_mismatch=0**，确认无错配 / 乱码 /
   数量级错误。（pymeeus 返回 J2000/mean 黄经，与金样 of-date tropical/true 存在岁差
   ~0.6° + Moon/行星 mean-vs-true 差异，度数不逐位相等属预期，非金样错误。）

**精确 oracle = 原版 exe**：原版 GUI 二进制 `A32_V3_51_Proj/int/main/astrolog32.exe`
本身支持 `-o0 <file>` 开关（写 @0203 位置表），在**有显示会话**中运行即可逐位比对：

```bash
astrolog32.exe -qb 7 4 1958 12:01 0 8 -116:23 39:54 -o0 out.txt
# 对比 out.txt 与 bj-1958-1204.golden.txt 的 @0203 主体
```

（headless 下原版 GUI 进消息循环挂起，无法自动化；本目录 `../oracle_check.py` 为
 pymeeus 独立 sanity 验证脚本，依赖 `pip install pymeeus`。）
