cmake_minimum_required(VERSION 3.12)
project(AstrologStar32 VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置默认构建类型（单配置生成器）
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Debug 模式配置
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -ggdb")

# Release 模式配置
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -g")

# 平台相关定义
if(WIN32)
    add_definitions(-DWIN32 -D_WINDOWS)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    # 为 GCC/Clang 添加对 __declspec 的支持
    add_compile_options("$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fdeclspec>")
    add_compile_options("$<$<NOT:$<C_COMPILER_ID:MSVC>>:-fdeclspec>")
    set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/swe/lib/windows")
elseif(APPLE)
    add_definitions(-DMACOS)
    set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/swe/lib/macos")
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES "Android")
        add_definitions(-DANDROID)
        set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/swe/lib/android")
    elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        add_definitions(-DFREEBSD)
        # GCC/Clang 编译器：指定输入和执行编码
        add_compile_options(-finput-charset=UTF-8)
        add_compile_options(-fexec-charset=UTF-8)
        set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/swe/lib/FreeBSD")
    else()
        add_definitions(-DLINUX)
        # GCC/Clang 编译器：指定输入和执行编码
        add_compile_options(-finput-charset=UTF-8)
        add_compile_options(-fexec-charset=UTF-8)
        set(SWE_LIB_PATH "${PROJECT_SOURCE_DIR}/swe/lib/linux")
    endif()
endif()

# 源文件
set(SOURCES
    src/main.cpp
    src/utils.cpp
    src/transcoder.cpp
    src/useswe.cpp
    src/core/planet.cpp
    src/core/houses.cpp
    src/core/aspects.cpp
    src/core/ephemeris.cpp
    src/core/progressions.cpp
    src/core/transits.cpp
    src/core/arabic_parts.cpp
    src/core/fixed_stars.cpp
    src/core/synastry.cpp
    src/core/lunar_nodes.cpp
    src/utils/parser.cpp
    src/utils/formatter.cpp
    src/data/signs.cpp
)

# 头文件
set(HEADERS
    include/utils.h
    include/transcoder.h
    include/version.h
    include/useswe.h
    include/astrolog.h
    include/core/planet.h
    include/core/houses.h
    include/core/aspects.h
    include/core/ephemeris.h
    include/core/progressions.h
    include/core/transits.h
    include/core/arabic_parts.h
    include/core/fixed_stars.h
    include/core/synastry.h
    include/core/lunar_nodes.h
    include/utils/parser.h
    include/utils/formatter.h
    include/data/signs.h
    include/models/chart_data.h
    include/models/celestial_body.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/swe/inc
)

# 链接系统库
if(WIN32) # windows
    target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/swedll32.lib)
elseif(APPLE) #macos
    target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/libswe.a)
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES "Android")
        target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/swedll32.a)
    elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/libswe.a)
    else()    # linux
        target_link_libraries(${PROJECT_NAME} ${SWE_LIB_PATH}/libswe.a)
    endif()
endif()

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION lib
)