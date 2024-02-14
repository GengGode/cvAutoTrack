cmake_minimum_required(VERSION 3.15)
project(cvAutoTrack)

#=======================================================================================================================
# 设置dll输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 设置安装目录
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
#=======================================================================================================================
# 设置第三方目录
set(DependentPatryDir ${CMAKE_SOURCE_DIR}/dependent_party)
set(ThridPatryDir ${CMAKE_SOURCE_DIR}/third_party)
set(IncludeDir ${CMAKE_SOURCE_DIR}/include)
set(SourceDir ${CMAKE_SOURCE_DIR}/source)
#=======================================================================================================================
include(cmake/auto_subdirectory.cmake)
include(cmake/add_pch_configure.cmake)
#=======================================================================================================================
include(cmake/parse_version.cmake)
auto_increment_version_file("source/resource/version_hash.hash" "source/resource/version.ver")
parse_version_file("source/resource/version.ver")
get_commit_hash(PROJECT_VERSION_HASH)
get_git_branch(PROJECT_VERSION_BRANCH)
#=======================================================================================================================
set(BUILD_DYANMIC_LINK_DEPS ON CACHE BOOL "动态链接依赖库")
set(BUILD_TESTS OFF CACHE BOOL "编译测试")
set(BUILD_DOC OFF CACHE BOOL "编译文档")



include_directories(${IncludeDir})
include_directories(${ThridPatryDir})
include_directories(${DependentPatryDir})


#=======================================================================================================================
# 设置编译选项
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    add_compile_options(/std:c++17)
    # Unicode
    add_definitions(-DUNICODE -D_UNICODE)
endif()
add_compile_options(/utf-8)
#=======================================================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
Set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake_targets")
#=======================================================================================================================
# 包含子项目
add_subdirectory(source)
if(BUILD_CVAUTOTRACK_TESTS)
    message(STATUS  "编译内容：cvAutoTrack, tests")
    add_subdirectory(tests)
    else()
    message(STATUS  "编译内容：cvAutoTrack | 不编译tests")
    message(STATUS  "   (如果需要编译tests，请设置BUILD_CVAUTOTRACK_TESTS=ON)")
endif()
#=======================================================================================================================
message(STATUS "==============================================================")
message(STATUS "Config : ")
message(STATUS "    当前版本                     : ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
message(STATUS "    产物名称                     : ${PROJECT_NAME}")
message(STATUS "    动态链接                     : ${BUILD_DYANMIC_LINK_DEPS}")
message(STATUS "    编译测试                     : ${BUILD_TESTS}")
message(STATUS "Resource : ")
message(STATUS "    地图数量                     : ${model_rec_file}")
message(STATUS "Library : ")
message(STATUS "    opencv_include_dir          : ${opencv_include_dir}")
message(STATUS "    opencv_version              : ${opencv_version}")
message(STATUS "Include : ")   
message(STATUS "    include_dir                 : ${include_dir}")
message(STATUS "Install : ")   
message(STATUS "    CMAKE_INSTALL_PREFIX        : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==============================================================\n")
