cmake_minimum_required(VERSION 3.15)

include(cmake/parse_version.cmake)
include(cmake/auto_subdirectory.cmake)

# 设置第三方目录
set(ThridPatryDir ${CMAKE_SOURCE_DIR}/third_party)
set(SourceDir ${CMAKE_SOURCE_DIR}/source/src)
# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 设置编译选项
set(CMAKE_CXX_STANDARD 20)
if(MSVC)
    add_compile_options(/std:c++latest)
endif()
add_compile_options(/utf-8)
# Unicode
add_definitions(-DUNICODE -D_UNICODE)

project(libcvAutoTrack)

# 包含子项目
add_subdirectory(source)
add_subdirectory(tests)