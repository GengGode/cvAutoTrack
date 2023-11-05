
set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-release-static/x64/vc17/staticlib")
#set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/lib")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(include)
include_directories(src)


file(GLOB_RECURSE CppFiles "src/*.cpp")
file(GLOB_RECURSE HppFiles "src/*.h")
file(GLOB_RECURSE RcFiles "resource/*.rc")

parse_version_file(src/version/version.ver)
set(PROJECT_VERSION_MAJOR ${VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${VERSION_PATCH})

# version.rc
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.rc.in"
    "version.rc"
)

add_compile_options(/utf-8)

if(BUILD_SUBMODULE)
    add_library(cvAutoTrack.resources SHARED cvAutoTrack.rc src/dllmain.cpp)
    unset(RcFile)
else()
    set(RcFile cvAutoTrack.rc)
endif()

# define CVAUTOTRACK_EXPORTS
add_definitions(-DCVAUTOTRACK_EXPORTS)

add_library(cvAutoTrack SHARED
    include/cvAutoTrack.h
    src/cvAutoTrack.cpp
    ${CppFiles}
    ${HppFiles}
    #${RcFiles}
    ${RcFile}
    version.rc
)

if(BUILD_SUBMODULE)
    add_compile_options(/MD)
    set_target_properties(cvAutoTrack  PROPERTIES COMPILE_FLAGS "/MD")
else()
    add_compile_options(/MT)
    set_target_properties(cvAutoTrack  PROPERTIES COMPILE_FLAGS "/MT")
endif()

include(FetchContent)
# 配置cereal
set(BUILD_DOC OFF)
set(BUILD_SANDBOX OFF)
set(SKIP_PERFORMANCE_COMPARISON ON)
set(BUILD_TESTS OFF)
FetchContent_Declare(cereal GIT_REPOSITORY https://github.com/USCiLab/cereal.git GIT_TAG v1.3.2)
FetchContent_MakeAvailable(cereal)

target_link_libraries(cvAutoTrack PRIVATE  ${OpenCV_LIBS} d3d11.lib d3dcompiler.lib dxgi.lib)