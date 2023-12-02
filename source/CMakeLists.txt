
set(BUILD_SUBMODULE ON)
set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-release-static/x64/vc17/staticlib")
if(${BUILD_SUBMODULE})
    set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/lib")
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(include)
include_directories(src)


file(GLOB_RECURSE CppFiles "src/*.cpp")
file(GLOB_RECURSE HppFiles "src/*.h")
file(GLOB_RECURSE RcFiles "resource/*.rc")
set(VerRcFile "${CMAKE_BINARY_DIR}/resource/version.rc")

auto_increment_version_file(src/version/version_hash.hash src/version/version.ver)
parse_version_file(src/version/version.ver)
set(PROJECT_VERSION_MAJOR ${VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${VERSION_PATCH})

# version.rc
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.rc.in"
    "${VerRcFile}"
)


if(BUILD_SUBMODULE)
    # add_library(cvAutoTrack.resources SHARED cvAutoTrack.rc src/dllmain.cpp)
    # set_target_properties(cvAutoTrack.resources PROPERTIES FOLDER "cvAutoTrack")
    # unset(RcFile)
    # message(STATUS "build cvAutoTrack.resources")
else()
    set(RcFile cvAutoTrack.rc)
    message(STATUS "build cvAutoTrack")
endif()

# define CVAUTOTRACK_EXPORTS
add_definitions(-DCVAUTOTRACK_EXPORTS)
add_definitions(-DUSED_BINARY_IMAGE)

source_group("include" FILES include/cvAutoTrack.h)
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CppFiles} ${HppFiles} ${RcFiles} ${RcFile} ${VerRcFile})
source_group("version" FILES ${VerRcFile})

add_library(cvAutoTrack SHARED
    include/cvAutoTrack.h
    src/cvAutoTrack.cpp
    ${CppFiles}
    ${HppFiles}
    #${RcFiles}
    ${RcFile}
    ${VerRcFile}
)



set_target_properties(cvAutoTrack PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set_target_properties(cvAutoTrack PROPERTIES FOLDER "cvAutoTrack")

add_pch_define(cvAutoTrack ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp)
        
add_compile_options(/MP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
set_target_properties(cvAutoTrack PROPERTIES COMPILE_FLAGS "/MP")

include(FetchContent)
# 配置cereal
set(BUILD_DOC OFF)
set(BUILD_SANDBOX OFF)
set(SKIP_PERFORMANCE_COMPARISON ON)
set(BUILD_TESTS OFF)
FetchContent_Declare(cereal GIT_REPOSITORY https://github.com/USCiLab/cereal.git GIT_TAG v1.3.2)
FetchContent_MakeAvailable(cereal)
# 配置fmt
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG 10.1.1)
FetchContent_MakeAvailable(fmt)

target_link_libraries(cvAutoTrack PRIVATE  ${OpenCV_LIBS} cereal::cereal fmt::fmt-header-only d3d11.lib d3dcompiler.lib dxgi.lib)

if(BUILD_SUBMODULE)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        add_custom_command(TARGET cvAutoTrack POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/bin/opencv_world480d.dll"
                    "$<TARGET_FILE_DIR:cvAutoTrack>") 
    else()
        add_custom_command(TARGET cvAutoTrack POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/bin/opencv_world480.dll"
                    "$<TARGET_FILE_DIR:cvAutoTrack>") 
    endif()         
endif()
