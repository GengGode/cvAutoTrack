cmake_minimum_required(VERSION 3.15)

set(BUILD_CVAUTOTRACK_TESTS OFF CACHE BOOL "build cvAutoTrack tests")
set(BUILD_DOC OFF CACHE BOOL "build cvAutoTrack doc")

include(cmake/parse_version.cmake)
include(cmake/auto_subdirectory.cmake)
include(cmake/add_pch_configure.cmake)

# 设置第三方目录
set(DependentPatryDir ${CMAKE_SOURCE_DIR}/dependent_party)
set(ThridPatryDir ${CMAKE_SOURCE_DIR}/third_party)
set(IncludeDir ${CMAKE_SOURCE_DIR}/include)
set(SourceDir ${CMAKE_SOURCE_DIR}/source)

include_directories(${IncludeDir})
include_directories(${ThridPatryDir})
include_directories(${DependentPatryDir})


# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 设置编译选项
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    add_compile_options(/std:c++17)
    # Unicode
    add_definitions(-DUNICODE -D_UNICODE)
endif()
add_compile_options(/utf-8)

project(libcvAutoTrack)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
Set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake_targets")

# 包含子项目
add_subdirectory(source)
if(BUILD_CVAUTOTRACK_TESTS)
    message(STATUS  "编译内容：cvAutoTrack, tests")
    add_subdirectory(tests)
    else()
    message(STATUS  "编译内容：cvAutoTrack | 不编译tests")
    message(STATUS  "   (如果需要编译tests，请设置BUILD_CVAUTOTRACK_TESTS=ON)")
endif()
