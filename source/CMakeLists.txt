
set(BUILD_SUBMODULE ON)
set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-release-static/x64/vc17/staticlib")
if(BUILD_SUBMODULE)
    set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/lib")
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(include)
include_directories(src)


file(GLOB_RECURSE CppFiles "src/*.cpp")
file(GLOB_RECURSE HppFiles "src/*.h")
file(GLOB_RECURSE RcFiles "resource/*.rc")
file(GLOB_RECURSE VerRcFile "version.rc")

auto_increment_version_file(src/version/version_hash.hash src/version/version.ver)
parse_version_file(src/version/version.ver)
set(PROJECT_VERSION_MAJOR ${VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${VERSION_PATCH})

# version.rc
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.rc.in"
    "version.rc"
)


if(BUILD_SUBMODULE)
    add_library(cvAutoTrack.resources SHARED cvAutoTrack.rc src/dllmain.cpp)
    set_target_properties(cvAutoTrack.resources PROPERTIES FOLDER "cvAutoTrack")
    unset(RcFile)
    message(STATUS "build cvAutoTrack.resources")
else()
    set(RcFile cvAutoTrack.rc)
    message(STATUS "build cvAutoTrack")
endif()

# define CVAUTOTRACK_EXPORTS
add_definitions(-DCVAUTOTRACK_EXPORTS)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CppFiles} ${HppFiles} ${RcFiles} ${RcFile})
source_group("Resource" PREFIX "./" FILES ${VerRcFile})

add_library(cvAutoTrack SHARED
    include/cvAutoTrack.h
    src/cvAutoTrack.cpp
    ${CppFiles}
    ${HppFiles}
    #${RcFiles}
    ${RcFile}
    ${VerRcFile}
)

macro(add_pch_define target pch_header_file pch_source_file)
    if(MSVC)
        get_filename_component(pch_file_name ${pch_header_file} NAME_WE)

        if(CMAKE_CONFIGURATION_TYPES)
        #如果有配置选项（Debug/ReLease），路径添加以及配置选项
            set(pch_dir "${CMAKE_CURRENT_BINARY_DIR}/pch/${CMAKE_CFG_INTDIR}")
        else(CMAKE_CONFIGURATION_TYPES)
            SET(pch_dir "${CMAKE_CURRENT_BINARY_DIR}/pch")
        endif(CMAKE_CONFIGURATION_TYPES)

        file(MAKE_DIRECTORY ${pch_dir})

        set_target_properties(${target} PROPERTIES COMPILE_FLAGS "/Yu${pch_header_file}.h /FI${pch_header_file}.h /Fp${pch_dir}/${target}_${pch_file_name}.pch")

        set_source_files_properties(${pch_source_file} PROPERTIES COMPILE_FLAGS "/Yc${pch_file_name}.h")
        
        set_target_properties(${target} PROPERTIES PCH_OUTPUT_DIR ${pch_dir})
        set_target_properties(${target} PROPERTIES PCH_OUTPUT_NAME ${target}_${pch_file_name}.pch)
        set_source_files_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${pch_dir}/${target}_${pch_file_name}.pch")
    endif()

endmacro(add_pch_define)



set_target_properties(cvAutoTrack PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set_target_properties(cvAutoTrack PROPERTIES FOLDER "cvAutoTrack")

#set_target_properties(cvAutoTrack PROPERTIES COMPILE_FLAGS "/Yu${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
add_pch_define(cvAutoTrack ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp)
        
add_compile_options(/MP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
set_target_properties(cvAutoTrack PROPERTIES COMPILE_FLAGS "/MP")

if(BUILD_SUBMODULE)
    if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
        add_compile_options(/MD)
        set_target_properties(cvAutoTrack  PROPERTIES COMPILE_FLAGS "/MD")
        message(STATUS "使用Opencv动态库")
    else()
        add_compile_options(/MDd)
        set_target_properties(cvAutoTrack  PROPERTIES COMPILE_FLAGS "/MDd")
        message(STATUS "使用Opencv动态库调试版")
    endif()
else()
    add_compile_options(/MT)
    set_target_properties(cvAutoTrack  PROPERTIES COMPILE_FLAGS "/MT")
    message(STATUS "使用Opencv静态库")
endif()

include(FetchContent)
# 配置cereal
set(BUILD_DOC OFF)
set(BUILD_SANDBOX OFF)
set(SKIP_PERFORMANCE_COMPARISON ON)
set(BUILD_TESTS OFF)
FetchContent_Declare(cereal GIT_REPOSITORY https://github.com/USCiLab/cereal.git GIT_TAG v1.3.2)
FetchContent_MakeAvailable(cereal)

target_link_libraries(cvAutoTrack PRIVATE  ${OpenCV_LIBS} cereal::cereal d3d11.lib d3dcompiler.lib dxgi.lib)

if(BUILD_SUBMODULE)
    add_custom_command(TARGET cvAutoTrack POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/bin/opencv_world480.dll"
                "$<TARGET_FILE_DIR:cvAutoTrack>")                    
endif()
