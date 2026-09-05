cmake_minimum_required(VERSION 3.12)
project(Astrolog32 VERSION 0.1)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置默认构建类型（单配置生成器）
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Debug 模式配置 - 分别针对不同编译器设置调试信息选项
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /RTC1")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi /RTC1")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -ggdb")
endif()

# Release 模式配置
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
endif()

# 平台相关定义
if(WIN32)
    add_definitions(-DWIN32 -D_WINDOWS)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    
    # 添加MSVC特定的编译选项
    if(MSVC)
        # 启用运行时库检查和安全增强
        add_compile_options(/W3)           # 警告级别3
        add_compile_options(/EHsc)         # 启用C++异常处理
        
        # 链接器选项
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
    endif()
    
    set(PLATFORM_DIR "windows")
    # 强制使用x86架构
    set(SWE_ARCH_DIR "x86")
elseif(APPLE)
    add_definitions(-DMACOS)
    set(PLATFORM_DIR "macos")
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES "Android")
        add_definitions(-DANDROID)
        set(PLATFORM_DIR "android")
    elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        add_definitions(-DFREEBSD)
        set(PLATFORM_DIR "FreeBSD")
    else()
        add_definitions(-DLINUX)
        # GCC/Clang 编译器：指定输入和执行编码
        add_compile_options(-finput-charset=UTF-8)
        add_compile_options(-fexec-charset=UTF-8)
        set(PLATFORM_DIR "linux")
    endif()
endif()

# 设置库文件输出目录
set(LIB_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIB_OUTPUT_DIR}/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIB_OUTPUT_DIR}/Release)

# 设置可执行文件输出目录
set(EXEC_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/${PLATFORM_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXEC_OUTPUT_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXEC_OUTPUT_DIR}/Release)

# 设置DLL文件输出目录
set(DLL_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/${PLATFORM_DIR})
set(DLL_OUTPUT_DIR_DEBUG ${DLL_OUTPUT_DIR}/Debug)
set(DLL_OUTPUT_DIR_RELEASE ${DLL_OUTPUT_DIR}/Release)
# ============================================================
# SWE (Swiss Ephemeris) —— 源码构建（替代预编译 libswe.a）
# ------------------------------------------------------------
# SWE 以 git submodule 引入（astroproject/swe/swisseph，GitHub 官方原版）。
# 不再依赖预编译库：源码随仓库构建，避免平台/ABI 不匹配
# （此前预编译库在 musl 下缺 fseeko64 符号即为此类问题）。
# 官方 Makefile 的 SWEOBJ 即为下列 9 个源文件。
# ============================================================
set(SWE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/swe/swisseph)

set(SWE_LIB_SRCS
    ${SWE_SOURCE_DIR}/swedate.c
    ${SWE_SOURCE_DIR}/swehouse.c
    ${SWE_SOURCE_DIR}/swejpl.c
    ${SWE_SOURCE_DIR}/swemmoon.c
    ${SWE_SOURCE_DIR}/swemplan.c
    ${SWE_SOURCE_DIR}/sweph.c
    ${SWE_SOURCE_DIR}/swephlib.c
    ${SWE_SOURCE_DIR}/swecl.c
    ${SWE_SOURCE_DIR}/swehel.c
)

# SWE 静态库（源码构建）
add_library(swe STATIC ${SWE_LIB_SRCS})
target_include_directories(swe PUBLIC ${SWE_SOURCE_DIR})

if(WIN32)
    # MSVC 下 SWE 需 MSDOS 宏以启用 _fseeki64（见 swejpl.c）
    target_compile_definitions(swe PRIVATE MSDOS)
else()
    # Unix 系：64 位文件偏移（swejpl.c 亦自带 _FILE_OFFSET_BITS 64）
    target_compile_definitions(swe PRIVATE _FILE_OFFSET_BITS=64)
endif()

# 官方 Makefile: LIBS = -lm -ldl (Linux)
if(UNIX AND NOT APPLE)
    target_link_libraries(swe PUBLIC m)
    find_library(SWE_DL_LIB dl)
    if(SWE_DL_LIB)
        target_link_libraries(swe PUBLIC ${SWE_DL_LIB})
    endif()
endif()

# ============================================================
# 项目静态库 astrolog32
# ============================================================
include_directories(include)
include_directories(include/core)
include_directories(include/models)
include_directories(include/data)
include_directories(include/utils)
include_directories(swe)                   # 自定义补丁头（swe_sidtime_args.h）
include_directories(${SWE_SOURCE_DIR})      # SWE 官方源码头文件

# 查找所有源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 创建静态库（含 SWE 自定义补丁 swe_sidtime_args.c）
add_library(astrolog32 STATIC ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/swe/swe_sidtime_args.c)

# 设置包含目录
target_include_directories(astrolog32 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/swe
    ${SWE_SOURCE_DIR}
)

# 添加预处理器定义，表示我们正在构建静态库
target_compile_definitions(astrolog32 PUBLIC ASTROLOG32_STATIC_LIB)

# 链接 SWE（PUBLIC 传递到可执行文件）
target_link_libraries(astrolog32 PUBLIC swe)

# ============================================================
# 测试可执行文件
# ============================================================
add_executable(astrolog32_oldmain test/oldmain.cpp)
target_link_libraries(astrolog32_oldmain astrolog32)

if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_oldmain PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_executable(astrolog32_apimainchart test/apimainchart.cpp)
target_link_libraries(astrolog32_apimainchart astrolog32)

if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_apimainchart PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_executable(astrolog32_apichart2 test/apichart2.cpp)
target_link_libraries(astrolog32_apichart2 astrolog32)

if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_apichart2 PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# ============================================================
# P0.3 — astrolog32-cli (real command-line interface)
# ============================================================
add_executable(astrolog32-cli cli/main.cpp)
target_link_libraries(astrolog32-cli astrolog32)

if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32-cli PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# ============================================================
# P0.4 — CTest：API 冒烟单测 + 金样 diff（run CLI → diff golden）
# ============================================================
enable_testing()

# 单元冒烟测试（无 gtest 依赖，断言失败返回非零）
add_executable(astrolog32_unit test/unit/unit_smoke.cpp)
target_link_libraries(astrolog32_unit astrolog32)
if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_unit PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(NAME unit_smoke COMMAND astrolog32_unit)

# P1.1 — 配置层（原版 dat 格式开关解释器）单测
add_executable(astrolog32_unit_config test/unit/unit_config.cpp)
target_link_libraries(astrolog32_unit_config astrolog32)
if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_unit_config PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(NAME unit_config COMMAND astrolog32_unit_config)

# P1.5/A5 — 阿拉伯点 177 引擎单测（福点/精神点 == CastChart 内联 + golden 锚点）
add_executable(astrolog32_unit_arabic test/unit/unit_arabic.cpp)
target_link_libraries(astrolog32_unit_arabic astrolog32)
if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_unit_arabic PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(NAME unit_arabic COMMAND astrolog32_unit_arabic)

# A7 — 格局识别 API（DetectGrands）合成网格单测
add_executable(astrolog32_unit_grands test/unit/unit_grands.cpp)
target_link_libraries(astrolog32_unit_grands astrolog32)
if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_unit_grands PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(NAME unit_grands COMMAND astrolog32_unit_grands)

# P2/A8 — 次限推进链单测（delta=0 golden 恒等 + 前向 sanity）
add_executable(astrolog32_unit_progress test/unit/unit_progress.cpp)
target_link_libraries(astrolog32_unit_progress astrolog32)
if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_unit_progress PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(NAME unit_progress COMMAND astrolog32_unit_progress)

# P2/A8-1 — 行运盘单测（rcTransit == 目标盘 → 金样继承）
add_executable(astrolog32_unit_transit test/unit/unit_transit.cpp)
target_link_libraries(astrolog32_unit_transit astrolog32)
if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_unit_transit PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(NAME unit_transit COMMAND astrolog32_unit_transit)

# P2/A8-2 — 太阳/月亮弧方向单测（构造恒等 + delta0）
add_executable(astrolog32_unit_solararc test/unit/unit_solararc.cpp)
target_link_libraries(astrolog32_unit_solararc astrolog32)
if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_unit_solararc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(NAME unit_solararc COMMAND astrolog32_unit_solararc)

# P2/A9-0 — 太阳返照解算单测（天文定义：moment Sun == natal Sun）
add_executable(astrolog32_unit_returns test/unit/unit_returns.cpp)
target_link_libraries(astrolog32_unit_returns astrolog32)
if(WIN32 AND MSVC)
    target_compile_definitions(astrolog32_unit_returns PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(NAME unit_returns COMMAND astrolog32_unit_returns)

# 金样对拍：运行 astrolog32-cli → 与 test/golden/*.golden.txt 逐字段 diff
# （需要 Python3 解释器；未找到时跳过注册，可手动跑 test/verify_cli.py）
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(NAME golden_diff
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../test/verify_cli.py
            $<TARGET_FILE:astrolog32-cli>)
    # P1.5/A5 — 阿拉伯点二次 oracle：python 独立解码公式 DSL → 与引擎逐点对拍
    add_test(NAME arabic_oracle
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../test/verify_arabic.py
            $<TARGET_FILE:astrolog32-cli>
            $<TARGET_FILE:astrolog32_unit_arabic>)
    # A7① — 真实盘格局计数：本地 --grands 与 golden -g0 计数 fixture 对拍
    add_test(NAME grands_oracle
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../test/verify_grands.py
            $<TARGET_FILE:astrolog32-cli>)
endif()

# 安装规则
install(TARGETS astrolog32
    ARCHIVE DESTINATION lib/${PLATFORM_DIR}
    LIBRARY DESTINATION lib/${PLATFORM_DIR}
)
install(DIRECTORY include/ DESTINATION include)
