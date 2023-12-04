
set(WITH_DYNAMIC_OPENCV ON CACHE BOOL "link static opencv")
set(BUILD_CAPTURE_DXGI ON CACHE BOOL "build capture dxgi")
set(BUILD_SUPPORT_WINRT ON CACHE BOOL "support winrt api")
set(BUILD_SUPPORT_WINDUMP ON CACHE BOOL "support window dump")

set(WITH_DYNAMIC_OPENCV ON)
set(BUILD_CAPTURE_DXGI ON)

# opencv
set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-release-static/x64/vc17/staticlib")
if(WITH_DYNAMIC_OPENCV)
    set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/lib")
endif()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})



file(GLOB_RECURSE CppFiles "*.cpp")
file(GLOB_RECURSE HppFiles "*.h")
set(VerRcFile "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.rc")
set(VerHppFile "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.h")
set(InfaceHppFile "${IncludeDir}/cvAutoTrack.h")

auto_increment_version_file("resource/version_hash.hash" "resource/version.ver")
parse_version_file("resource/version.ver")
get_commit_hash(PROJECT_VERSION_HASH)
get_git_branch(PROJECT_VERSION_BRANCH)
set(PROJECT_VERSION_MAJOR ${VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${VERSION_PATCH})
set(PROJECT_VERSION_BUILD ${VERSION_BUILD})


# version.rc
configure_file("${VerRcFile}.in" "${VerRcFile}")
configure_file("${VerHppFile}.in" "${VerHppFile}")

add_definitions(-DCVAUTOTRACK_EXPORTS)
add_definitions(-DUSED_BINARY_IMAGE)
if(BUILD_CAPTURE_DXGI)
    add_definitions(-DBUILD_CAPTURE_DXGI)
endif()
if (BUILD_SUPPORT_WINRT)
    add_definitions(-DSUPPORT_WINRT)
endif()
if (BUILD_SUPPORT_WINDUMP)
    add_definitions(-DSUPPORT_WINDUMP)
endif()

source_group("include" FILES ${InfaceHppFile})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CppFiles} ${HppFiles} ${RcFiles} ${RcFile} ${VerRcFile})
source_group("version" FILES ${VerRcFile})

include_directories(${IncludeDir})
include_directories(${SourceDir})
add_library(cvAutoTrack SHARED ${InfaceHppFile}
    ${HppFiles}
    ${CppFiles}
    cvAutoTrack.cpp
    ${RcFile}
    ${VerRcFile}
)

set_target_properties(cvAutoTrack PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set_target_properties(cvAutoTrack PROPERTIES FOLDER "cvAutoTrack")

        
if(MSVC)
    add_pch_define(cvAutoTrack ${SourceDir}/pch.h ${SourceDir}/pch.cpp)
    
    add_compile_options(/MP)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set_target_properties(cvAutoTrack PROPERTIES COMPILE_FLAGS "/MP")
endif()
if(NOT MSVC)
    message(STATUS "使用pthread")
    # pthread
    find_package(Threads REQUIRED)
    target_link_libraries(cvAutoTrack PRIVATE Threads::Threads)
endif()

set_property(TARGET cvAutoTrack PROPERTY  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${WITH_DYNAMIC_OPENCV}>:DLL>")
if(WITH_DYNAMIC_OPENCV)
    message(STATUS "使用Opencv动态库")
else()
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
set_target_properties(cereal PROPERTIES FOLDER "cvAutoTrack/dependencies")
# 配置fmt
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG 10.1.1)
FetchContent_MakeAvailable(fmt)
set_target_properties(fmt PROPERTIES FOLDER "cvAutoTrack/dependencies")

target_link_libraries(cvAutoTrack 
PRIVATE  
    ${OpenCV_LIBS} 
    cereal::cereal 
    fmt::fmt-header-only 
    d3d11.lib 
    d3dcompiler.lib 
    dxgi.lib)

# install to release
if(WITH_DYNAMIC_OPENCV)
    add_custom_command(TARGET cvAutoTrack POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OpenCV_CONFIG_PATH}/../bin/opencv_world480$<$<CONFIG:Debug>:d>.dll" "$<TARGET_FILE_DIR:cvAutoTrack>") 
endif()
add_custom_target(copy_version_file ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.ver" "$<TARGET_FILE_DIR:cvAutoTrack>/version")