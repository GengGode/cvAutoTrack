
set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-release-static/x64/vc17/staticlib")
set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/lib")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(include)
include_directories(src)


file(GLOB_RECURSE CppFiles "src/*.cpp")
file(GLOB_RECURSE HppFiles "src/*.h")
file(GLOB_RECURSE RcFiles "resource/*.rc")

add_compile_options(/utf-8)
add_library(cvAutoTrack.resources SHARED cvAutoTrack.rc src/dllmain.cpp)
add_library(cvAutoTrack SHARED
    include/cvAutoTrack.h
    src/cvAutoTrack.cpp
    ${CppFiles}
    ${HppFiles}
    #${RcFiles}
    #cvAutoTrack.rc
)
#set_target_properties(cvAutoTrack  PROPERTIES COMPILE_FLAGS "/MT")

target_link_libraries(cvAutoTrack ${OpenCV_LIBS} d3d11.lib d3dcompiler.lib dxgi.lib)