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

# 包含目录
include_directories(include)
include_directories(include/core)
include_directories(include/models)
include_directories(include/data)
include_directories(include/utils)
include_directories(swe/inc)

# 查找所有源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 创建静态库
add_library(astrolog32 STATIC ${SOURCES})

# 设置包含目录
target_include_directories(astrolog32 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/swe/inc
)

# 添加预处理器定义，表示我们正在构建静态库
target_compile_definitions(astrolog32 PUBLIC ASTROLOG32_STATIC_LIB)

# 查找并链接SWE库
if(WIN32) # windows
    set(SWE_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/swe/lib/windows")
    # 强制使用x86版本的SWE库
    find_library(SWE_LIB NAMES swedll32 PATHS ${SWE_LIB_PATH} PATH_SUFFIXES x86 NO_DEFAULT_PATH)
    set(SWE_DLL_PATH "${SWE_LIB_PATH}/x86/swedll32.dll")
    
    # 在MSVC环境下，同时链接导入库和运行时DLL
    if(MSVC)
        # 确保DLL在运行时可以被找到
        target_compile_definitions(astrolog32 PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
    
    target_link_libraries(astrolog32 ${SWE_LIB})
elseif(APPLE) #macos
    set(SWE_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/swe/lib/macos")
    find_library(SWE_LIB NAMES swedll32 PATHS ${SWE_LIB_PATH})
    target_link_libraries(astrolog32 ${SWE_LIB})
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES "Android")
        set(SWE_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/swe/lib/android")
        find_library(SWE_LIB NAMES swe PATHS ${SWE_LIB_PATH})
        target_link_libraries(astrolog32 ${SWE_LIB})
    elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        set(SWE_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/swe/lib/FreeBSD")
        find_library(SWE_LIB NAMES swe PATHS ${SWE_LIB_PATH})
        target_link_libraries(astrolog32 ${SWE_LIB})
    else()    # linux
        set(SWE_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/swe/lib/linux")
        find_library(SWE_LIB NAMES swe PATHS ${SWE_LIB_PATH})
        target_link_libraries(astrolog32 ${SWE_LIB})
    endif()
endif()

# 创建可执行文件用于测试
add_executable(astrolog32_oldmain test/oldmain.cpp)
# 链接静态库和SWE库
target_link_libraries(astrolog32_oldmain astrolog32 ${SWE_LIB})

# 添加Windows平台特定的链接选项
if(WIN32 AND MSVC)
    # 确保测试可执行文件也能正确链接
    target_compile_definitions(astrolog32_oldmain PRIVATE _CRT_SECURE_NO_WARNINGS)
    
    # 为测试可执行文件显式链接SWE库
    target_link_libraries(astrolog32_oldmain ${SWE_LIB})
    
    # 将SWE DLL文件复制到可执行文件目录
    add_custom_command(TARGET astrolog32_oldmain POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SWE_DLL_PATH}"
        $<TARGET_FILE_DIR:astrolog32_oldmain>)
        
    # 将SWE DLL文件同时复制到可执行文件输出目录下对应的目录中
    add_custom_command(TARGET astrolog32_oldmain POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<IF:$<CONFIG:Debug>,${DLL_OUTPUT_DIR_DEBUG},${DLL_OUTPUT_DIR_RELEASE}>")
    add_custom_command(TARGET astrolog32_oldmain POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SWE_DLL_PATH}"
        "$<IF:$<CONFIG:Debug>,${DLL_OUTPUT_DIR_DEBUG},${DLL_OUTPUT_DIR_RELEASE}>")
endif()

add_executable(astrolog32_apimainchart test/apimainchart.cpp)
# 链接静态库和SWE库
target_link_libraries(astrolog32_apimainchart astrolog32 ${SWE_LIB})

# 添加Windows平台特定的链接选项
if(WIN32 AND MSVC)
    # 确保测试可执行文件也能正确链接
    target_compile_definitions(astrolog32_apimainchart PRIVATE _CRT_SECURE_NO_WARNINGS)
    
    # 为测试可执行文件显式链接SWE库
    target_link_libraries(astrolog32_apimainchart ${SWE_LIB})
    
    # 将SWE DLL文件复制到可执行文件目录
    add_custom_command(TARGET astrolog32_apimainchart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SWE_DLL_PATH}"
        $<TARGET_FILE_DIR:astrolog32_apimainchart>)
        
    # 将SWE DLL文件同时复制到可执行文件输出目录下对应的目录中
    add_custom_command(TARGET astrolog32_apimainchart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<IF:$<CONFIG:Debug>,${DLL_OUTPUT_DIR_DEBUG},${DLL_OUTPUT_DIR_RELEASE}>")
    add_custom_command(TARGET astrolog32_apimainchart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SWE_DLL_PATH}"
        "$<IF:$<CONFIG:Debug>,${DLL_OUTPUT_DIR_DEBUG},${DLL_OUTPUT_DIR_RELEASE}>")
endif()

add_executable(astrolog32_apichart2 test/apichart2.cpp)
# 链接静态库和SWE库
target_link_libraries(astrolog32_apichart2 astrolog32 ${SWE_LIB})

# 添加Windows平台特定的链接选项
if(WIN32 AND MSVC)
    # 确保测试可执行文件也能正确链接
    target_compile_definitions(astrolog32_apichart2 PRIVATE _CRT_SECURE_NO_WARNINGS)
    
    # 为测试可执行文件显式链接SWE库
    target_link_libraries(astrolog32_apichart2 ${SWE_LIB})
    
    # 将SWE DLL文件复制到可执行文件目录
    add_custom_command(TARGET astrolog32_apichart2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SWE_DLL_PATH}"
        $<TARGET_FILE_DIR:astrolog32_apichart2>)
        
    # 将SWE DLL文件同时复制到可执行文件输出目录下对应的目录中
    add_custom_command(TARGET astrolog32_apichart2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<IF:$<CONFIG:Debug>,${DLL_OUTPUT_DIR_DEBUG},${DLL_OUTPUT_DIR_RELEASE}>")
    add_custom_command(TARGET astrolog32_apichart2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SWE_DLL_PATH}"
        "$<IF:$<CONFIG:Debug>,${DLL_OUTPUT_DIR_DEBUG},${DLL_OUTPUT_DIR_RELEASE}>")
endif()

# 安装规则
install(TARGETS astrolog32
    ARCHIVE DESTINATION lib/${PLATFORM_DIR}
    LIBRARY DESTINATION lib/${PLATFORM_DIR}
)
install(DIRECTORY include/ DESTINATION include)