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
