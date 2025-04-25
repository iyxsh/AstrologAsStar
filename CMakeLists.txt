cmake_minimum_required(VERSION 3.12)
project(astrolog32 VERSION 0.1)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置默认构建类型（单配置生成器）
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Debug 模式配置
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -ggdb")

# Release 模式配置
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

# 平台相关定义
if(WIN32)
    add_definitions(-DWIN32 -D_WINDOWS)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
elseif(APPLE)
    add_definitions(-DMACOS)
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES "Android")
        add_definitions(-DANDROID)
    else()
        add_definitions(-DLINUX)
        # GCC/Clang 编译器：指定输入和执行编码
        add_compile_options(-finput-charset=UTF-8)
        add_compile_options(-fexec-charset=UTF-8)
    endif()
endif()

# 源文件
set(SOURCES
    ./astrolog32/astrolog.cpp
    ./astrolog32/TransU/TransU.cpp
    ./astrolog32/useswe/useswe.cpp
)

# 头文件
set(HEADERS
    ./astrolog32/astrolog.h
    ./astrolog32/version.h
    ./astrolog32/swe/inc/swedll.h
    ./astrolog32/TransU/TransU.h
    ./astrolog32/useswe/useswe.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SOURCES} ${HEADERS})

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} astrolog32/swe/inc astrolog32/Trans astrolog32/xml)
# 链接系统库
if(WIN32) # windows
    set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/astrolog32/swe/lib/windows")
    find_library(SWE_LIB NAMES swedll32 PATHS ${SWE_LIB_PATH})
    target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/swedll32.lib)
elseif(APPLE) #macos
    set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/astrolog32/swe/lib/macos")
    find_library(SWE_LIB NAMES swedll32 PATHS ${SWE_LIB_PATH})
    target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/libswe.a)
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES "Android")
        set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/astrolog32/swe/lib/android")
        find_library(SWE_LIB NAMES swedll32 PATHS ${SWE_LIB_PATH})
        target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/swedll32.a)
    else()    # linux
        set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/astrolog32/swe/lib/linux")
        find_library(SWE_LIB NAMES libswe PATHS ${SWE_LIB_PATH})
        target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/libswe.a)
    endif()
endif()
# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION lib
)