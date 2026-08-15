cmake_minimum_required(VERSION 3.15)
project(RimeDictHelper2 LANGUAGES CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_WIN32_EXECUTABLE ON)

# 添加资源文件
set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -I${CMAKE_SOURCE_DIR}")

add_executable(RimeDictHelper2 WIN32
        RimeDictHelper2.cpp
        RimeDictHelper2.rc          # 明确添加 .rc 文件
)

# 可选：自动复制 config.ini
file(COPY ${CMAKE_SOURCE_DIR}/config.ini DESTINATION ${CMAKE_BINARY_DIR})