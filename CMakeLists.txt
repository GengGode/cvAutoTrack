cmake_minimum_required(VERSION 3.15)

include(cmake/parse_version.cmake)

# 设置第三方目录
set(ThridPatryDir ${CMAKE_SOURCE_DIR}/third_party)
# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 设置编译选项
set(CMAKE_CXX_STANDARD 20)
if(MSVC)
    add_compile_options(/std:c++latest)
    add_compile_options(/utf-8)
endif()

project(libcvAutoTrack)

# 包含子项目
add_subdirectory(cvAutoTrack)
add_subdirectory(test)
add_subdirectory(impl)