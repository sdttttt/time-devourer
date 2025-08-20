cmake_minimum_required(VERSION 3.10)
project("time-devourer" VERSION 0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB SOURCES "*.cpp" "timer.rc")
add_executable("time-devourer" WIN32 ${SOURCES})

target_precompile_headers("time-devourer" PRIVATE
        <windows.h>
        <vector>
        <string>
        <algorithm>
)

target_compile_definitions("time-devourer" PRIVATE _UNICODE UNICODE)
target_compile_features("time-devourer" PRIVATE cxx_std_17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)