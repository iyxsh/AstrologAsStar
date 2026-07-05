# AstrologAsStar

Astrolog32 重构项目 - 基于 Swiss Ephemeris 的星盘计算引擎

## 项目简介

AstrologAsStar 是一个对经典 Astrolog32 进行重构的项目，保留了其核心功能并使用现代 C++ 进行重写，集成了 Swiss Ephemeris（瑞士星历表）库以提供高精度的天体位置计算。

## 项目结构

```
astroproject/
├── include/              # 头文件目录
│   ├── core/            # 核心计算模块
│   │   ├── aspects.h   # 相位计算
│   │   ├── chart.h     # 星盘计算
│   │   ├── ephemeris.h # 星历表管理
│   │   ├── fixed_stars.h # 恒星计算
│   │   ├── houses.h    # 宫位系统
│   │   ├── lunar_nodes.h # 月交点
│   │   ├── options.h   # 选项配置
│   │   ├── planet.h    # 行星计算
│   │   ├── progressions.h # 行进
│   │   ├── synastry.h  # 合盘分析
│   │   └── transits.h  # 推运计算
│   ├── data/           # 数据模块
│   │   └── signs.h     # 星座数据
│   ├── models/         # 数据模型
│   │   ├── chart_data.h # 星盘数据
│   │   ├── settings.h  # 设置管理
│   │   └── version.h   # 版本信息
│   ├── utils/          # 工具函数
│   │   ├── TransU.h    # 转换工具
│   │   ├── formatter.h # 格式化工具
│   │   ├── parser.h    # 解析器
│   │   └── utils.h     # 通用工具
│   ├── astrolog.h      # 主头文件
│   └── astrolog_lib.h  # 库接口头文件
├── src/                # 源代码目录
│   ├── core/           # 核心模块实现
│   │   ├── aspects.cpp
│   │   ├── chart.cpp
│   │   ├── ephemeris.cpp
│   │   ├── fixed_stars.cpp
│   │   ├── houses.cpp
│   │   ├── lunar_nodes.cpp
│   │   ├── planet.cpp
│   │   ├── progressions.cpp
│   │   ├── synastry.cpp
│   │   └── transits.cpp
│   ├── utils/          # 工具实现
│   │   ├── TransU.cpp
│   │   ├── formatter.cpp
│   │   ├── parser.cpp
│   │   └── utils.cpp
│   └── astrolog.cpp    # 主程序入口
├── swe/                # Swiss Ephemeris 库
│   ├── inc/           # 头文件
│   └── lib/           # 预编译库
├── test/              # 测试程序
│   ├── apichart2.cpp
│   ├── apimainchart.cpp
│   └── oldmain.cpp
├── bin/               # 编译输出目录
├── lib/               # 库文件输出目录
└── CMakeLists.txt     # CMake 构建配置
```

## 核心功能

### 星盘计算
- 支持多种行星计算（太阳、月亮、水星、金星、火星、木星、土星、天王星、海王星、冥王星等）
- 精确的恒星位置计算
- 多种宫位系统支持（Placidus, Koch, Regiomontanus, Campanus 等）

### 相位分析
- 自动计算所有行星相位
- 支持多种相位类型：合相、六分相、四分相、三分相、对分相
- 支持逆行计算

### 进阶功能
- **行进 (Progressions)**: 太阳弧 progressions 和次级行进
- **推运 (Transits)**: 推运星盘计算
- **合盘 (Synastry)**: 两人星盘比较
- **日/月返**: 太阳返回图和月亮返回图

### 数据输出
- 文本格式星盘输出
- 详细相位列表
- 行星影响力计算

## 编译说明

### 环境要求

- C++ 编译器 (MSVC, GCC, Clang)
- CMake 3.10+
- Windows/Linux 操作系统

### 基本编译步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
cmake --build .
```

### Windows 平台编译

项目已配置好 Visual Studio 解决方案，可以直接用 Visual Studio 打开编译，或使用 CMake 生成解决方案：

```bash
cmake -G "Visual Studio 17 2022" -A Win32 ..
```

### 调试和发布模式

#### 调试模式（默认）
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

#### 发布模式
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### 输出文件

编译完成后，可执行文件和库文件位于：

- **Debug 模式**: `astroproject/bin/windows/Debug/`
- **Release 模式**: `astroproject/bin/windows/Release/`
- **库文件**: `astroproject/lib/windows/Debug/` 或 `Release/`

## 库接口

项目提供库接口支持，可作为动态库或静态库使用：

### 静态库
- `astrolog32.lib` - 静态链接库

### 动态库
- `swedll32.dll` - Swiss Ephemeris 运行时 DLL

### API 示例

```cpp
#include "astrolog.h"

// 设置星盘数据
ChartInput chartInput;
chartInput.year = 2024;
chartInput.month = 1;
chartInput.day = 15;
chartInput.time = 12.0;  // 12:00
chartInput.dst = 0;
chartInput.zon = 8.0;    // 东八区
chartInput.lon = 116.4;  // 北京经度
chartInput.lat = 39.9;   // 北京纬度

// 设置星盘数据并计算
SetChartData(chart_Newton, chartInput, true);

// 获取相位结果
std::wstring aspectResult = GetMainChartAspect();
```

## 行星与天体

项目支持计算以下天体：

| 天体 | 说明 |
|------|------|
| 太阳 | Sun |
| 月亮 | Moon |
| 水星 | Mercury |
| 金星 | Venus |
| 火星 | Mars |
| 木星 | Jupiter |
| 土星 | Saturn |
| 天王星 | Uranus |
| 海王星 | Neptune |
| 冥王星 | Pluto |
| 凯龙星 | Chiron |
| 灶神星 | Vesta |
| 智神星 | Pallas |
| 婚神星 | Juno |
| 谷神星 | Ceres |
| 月交点 | North Node, South Node |

## 依赖

- **Swiss Ephemeris**: 提供高精度的星历计算
- **C++ 标准库**: 标准模板库和字符串处理

## 许可证

本项目基于原始 Astrolog32 的许可证，具体请查看 LICENSE 文件。

## 贡献

欢迎提交 Issue 和 Pull Request。

## 参考

- [Swiss Ephemeris](https://www.astro.com/swisseph/)
- [Astrolog](https://www.astrolog.org/)