# 1. 指定 CMake 的最低版本
cmake_minimum_required(VERSION 3.15)

# 2. 定义项目名称和使用的语言
project(RimeDictHelper2 LANGUAGES CXX)

# 3. 启用对 Windows 应用程序的支持 (保证 WinMain 被正确链接)
set(CMAKE_WIN32_EXECUTABLE ON)

# 4. 添加可执行文件
add_executable(RimeDictHelper2 WIN32
        RimeDictHelper2.cpp          # 你的主源文件
        # 如果你还有其他 .cpp 或 .rc 文件，都列在这里
)