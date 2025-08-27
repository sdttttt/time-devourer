cmake_minimum_required(VERSION 3.10)
project(Timer VERSION 0.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES "*.cpp" "timer.rc")
add_executable(timer WIN32 ${SOURCES})
target_compile_definitions(timer PRIVATE _UNICODE UNICODE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)