
file(GLOB_RECURSE CppFiles 
"algorithms/*.cpp"
"genshin/*.cpp"
"global/*.cpp"
"match/*.cpp"
"resource/*.cpp"
"resources/*.cpp"
"utils/*.cpp"
"AutoTrack.cpp"
"cvAutoTrack.cpp"
"dllmain.cpp"
"ErrorCode.cpp"
"module.debug.cpp"
"module.error.cpp"
"module.frame.cpp"
"module.version.cpp"
"pch.cpp"
)
file(GLOB_RECURSE HppFiles 
"algorithms/*.h"
"algorithms/*.hpp"
"genshin/*.h"
"genshin/*.hpp"
"global/*.h"
"global/*.hpp"
"match/*.h"
"match/*.hpp"
"resource/*.h"
"resource/*.hpp"
"resources/*.h"
"resources/*.hpp"
"utils/*.h"
"utils/*.hpp"
)
set(VerRcFile "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.rc")
set(VerHppFile "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.h")
set(InfaceHppFile "${IncludeDir}/cvAutoTrack.h")


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
set_target_properties(cvAutoTrack PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

        
# release gen .pdb
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
if(MSVC)
    add_pch_define(cvAutoTrack ${SourceDir}/pch.h ${SourceDir}/pch.cpp)

    #/utf-8
    target_compile_options(cvAutoTrack PRIVATE "/utf-8")
    add_definitions(-DUNICODE -D_UNICODE)
    #/W4
    #target_compile_options(cvAutoTrack PRIVATE "/W4")

    add_compile_options(/MP)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set_target_properties(cvAutoTrack PROPERTIES COMPILE_FLAGS "/MP")
endif()


add_subdirectory(frame)

find_package(fmt REQUIRED)
find_package(imgui REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cereal REQUIRED)
target_include_directories(cvAutoTrack PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(cvAutoTrack
PRIVATE  
    tianli::frame
    ${OpenCV_LIBS}
    cereal::cereal 
    fmt::fmt-header-only 
    imgui::imgui
    opengl32
    )

add_custom_target(copy_version_file ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/resource/version.ver" "$<TARGET_FILE_DIR:cvAutoTrack>/version")
set_target_properties(copy_version_file PROPERTIES FOLDER "scripts")
