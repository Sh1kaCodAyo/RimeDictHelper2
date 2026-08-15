cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
project(RimeDictHelper2 LANGUAGES CXX)

if (MSVC)
    add_compile_options(/utf-8)
endif ()

set(CMAKE_WIN32_EXECUTABLE ON)

# 添加资源文件
set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -I${CMAKE_SOURCE_DIR}")

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # 确保 Debug 模式生成完整的调试信息
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL")
endif ()

add_executable(RimeDictHelper2 WIN32
        src/RimeDictHelper2.cpp
        resource/RimeDictHelper2.rc
)

# 复制依赖文件到构建目录
file(COPY ${CMAKE_SOURCE_DIR}/resource/config.ini DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resource/after.bat DESTINATION ${CMAKE_BINARY_DIR})
