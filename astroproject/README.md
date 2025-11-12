# Astrolog32 重构项目

这是一个对原有 Astrolog32 程序的重构项目，目标是改善代码结构和可维护性，并将其构建成一个静态库供其他项目使用。

## 项目结构

```
astroproject/
├── CMakeLists.txt
├── README.md
├── bin/
│   └── windows/
│       ├── Debug/
│       │   ├── astrolog32_oldmain.exe
│       │   ├── astrolog32_apimain.exe
│       │   └── swedll32.dll
│       └── Release/
│           ├── astrolog32_oldmain.exe
│           ├── astrolog32_apimain.exe
│           └── swedll32.dll
├── include/
│   ├── astrolog.h
│   ├── astrolog_lib.h
│   ├── core/
│   │   ├── planet.h
│   │   ├── aspects.h
│   │   ├── houses.h
│   │   ├── fixed_stars.h
│   │   ├── lunar_nodes.h
│   │   ├── progressions.h
│   │   ├── transits.h
│   │   ├── synastry.h
│   │   ├── ephemeris.h
│   │   ├── chart.h
│   │   ├── options.h
│   │   └── transits.h
│   ├── data/
│   │   └── signs.h
│   ├── models/
│   │   ├── settings.h
│   │   ├── chart_data.h
│   │   └── version.h
│   └── utils/
│       ├── TransU.h
│       ├── parser.h
│       ├── formatter.h
│       └── utils.h
├── lib/
│   └── windows/
│       ├── Debug/
│       │   └── astrolog32.lib
│       └── Release/
│           └── astrolog32.lib
├── src/
│   ├── astrolog.cpp
│   ├── core/
│   │   ├── planet.cpp
│   │   ├── aspects.cpp
│   │   ├── houses.cpp
│   │   ├── fixed_stars.cpp
│   │   ├── lunar_nodes.cpp
│   │   ├── progressions.cpp
│   │   ├── transits.cpp
│   │   ├── synastry.cpp
│   │   ├── ephemeris.cpp
│   │   └── chart.cpp
│   ├── models/
│   └── utils/
│       ├── utils.cpp
│       ├── parser.cpp
│       ├── formatter.cpp
│       └── TransU.cpp
├── swe/
│   ├── inc/
│   └── lib/
│       └── windows/
└── test/
    ├── oldmain.cpp
    └── apimain.cpp
```

## 模块说明

### 核心模块 (core/)
- **planet.h/.cpp**: 行星计算相关功能
- **aspects.h/.cpp**: 相位计算相关功能
- **houses.h/.cpp**: 宫位系统相关功能
- **fixed_stars.h/.cpp**: 恒星计算相关功能
- **lunar_nodes.h/.cpp**: 月交点计算相关功能
- **progressions.h/.cpp**: 推运计算相关功能
- **transits.h/.cpp**: 行运计算相关功能
- **synastry.h/.cpp**: 合盘比较相关功能
- **ephemeris.h/.cpp**: 星历表计算相关功能
- **chart.h/.cpp**: 主要星盘计算功能
- **options.h**: 选项设置相关功能

### 数据模块 (data/)
- **signs.h**: 星座数据定义

### 模型 (models/)
- **settings.h**: 用户设置和内部设置结构体定义
- **chart_data.h**: 星盘数据结构体定义
- **version.h**: 版本信息定义

### 工具 (utils/)
- **TransU.h/.cpp**: Unicode转换相关工具函数
- **parser.h/.cpp**: 解析相关工具函数
- **formatter.h/.cpp**: 格式化相关工具函数
- **utils.h/.cpp**: 通用工具函数

### 库接口
- **astrolog_lib.h**: 库对外提供的API接口定义
- **astrolog.h**: 主程序入口相关定义

## 编译说明

项目使用 CMake 构建系统，支持跨平台编译。

### 基本编译步骤

```bash
mkdir build
cd build
cmake ..
make
```

### Windows平台编译

在Windows平台上，可以使用Visual Studio进行编译：
Debug模式：
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Debug
```
Release模式：
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release
```

注意：项目强制使用x86架构，即使在64位系统上也是如此。

### 调试和发布模式

可以通过设置CMAKE_BUILD_TYPE变量来选择构建类型：

```bash
# 调试模式（默认）
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 发布模式
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### 编译器特定选项

项目会根据使用的编译器自动设置适当的编译选项：

- **MSVC**（Microsoft Visual C++）:
  - 调试模式: `/Zi /RTC1`
  - 发布模式: `/O2`
  - 其他选项: `/W3`, `/EHsc`

- **GCC/Clang**:
  - 调试模式: `-g -ggdb`
  - 发布模式: `-O3`
  - Linux平台额外选项: `-finput-charset=UTF-8 -fexec-charset=UTF-8`

## 输出文件

项目构建后会生成以下主要文件，按构建配置分类：

### Debug模式
1. **静态库文件**: `lib/windows/Debug/astrolog32.lib` - 调试版本的静态库
2. **测试程序**: 
   - `bin/windows/Debug/astrolog32_oldmain.exe` - 使用旧版主函数的测试程序
   - `bin/windows/Debug/astrolog32_apimain.exe` - 使用新API接口的测试程序
3. **依赖库**: `bin/windows/Debug/swedll32.dll` - Debug版本的Swiss Ephemeris动态库

### Release模式
1. **静态库文件**: `lib/windows/Release/astrolog32.lib` - 发布版本的静态库
2. **测试程序**: 
   - `bin/windows/Release/astrolog32_oldmain.exe` - 使用旧版主函数的测试程序
   - `bin/windows/Release/astrolog32_apimain.exe` - 使用新API接口的测试程序
3. **依赖库**: `bin/windows/Release/swedll32.dll` - Release版本的Swiss Ephemeris动态库

## 库接口说明

项目现在作为静态库提供，主要接口包括：

- `initEnv()` - 初始化环境
- `RParseSz()` - 字符串解析函数
- `SetChartData()` - 设置星盘数据
- `GetChartAspect()` - 获取星盘相位信息
- `GetChartAspectRelation()` - 获取星盘相位关系信息
- `GetChartResult()` - 获取完整的星盘结果

## 重构进展

已完成：
1. 创建了模块化的目录结构
2. 将原有的庞大源文件拆分为多个模块
3. 定义了清晰的头文件和实现文件
4. 建立了基本的 CMakeLists.txt 构建配置
5. 开始迁移核心功能实现到模块中
6. 集成了 Swiss Ephemeris 库
7. 创建了基础的工具函数模块
8. 整理并修复了重复声明和定义问题
9. 统一了工具函数的实现和声明
10. 合并了分散的工具函数文件到统一的utils.cpp中
11. 将项目改造为静态库形式，提供API接口供外部调用
12. 创建了测试程序验证库的功能
13. 改进了构建系统，区分Debug和Release模式的输出目录

待完成：
1. 将原有代码完全迁移到对应的模块文件中
2. 完善各模块的实现
3. 测试重构后的功能
4. 优化代码结构和接口设计