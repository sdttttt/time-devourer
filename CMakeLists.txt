cmake_minimum_required(VERSION 3.10)
project("time-devourer" VERSION 0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # 使用标准 gnu++17

# 启用 UNICODE（推荐显式设置）
add_definitions(-DUNICODE -D_UNICODE)

file(GLOB SOURCES "*.cpp")
list(APPEND SOURCES timer.rc)

add_executable("time-devourer" WIN32 ${SOURCES})

target_precompile_headers("time-devourer" PRIVATE
        <windows.h>
        <vector>
        <string>
        <algorithm>
)

target_link_libraries("time-devourer"
        shlwapi
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)