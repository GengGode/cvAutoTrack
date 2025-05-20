add_library(tianli_frame STATIC)
add_library(tianli::frame ALIAS tianli_frame)

target_sources(tianli_frame
    PUBLIC
        frame.include.h
    PRIVATE
        frame.include.cpp
        capture/capture.include.h
        capture/capture.bitblt.h
        capture/capture.window_graphics.h
        capture/capture.window_graphics.cpp

        local/local.include.h
        local/local.picture.h
        local/local.video.h
        
        )

set_target_properties(tianli_frame PROPERTIES FOLDER "cvAutoTrack/modules")

find_package(OpenCV REQUIRED)
target_link_libraries(tianli_frame
    PUBLIC
        ${OpenCV_LIBS}
        d3d11.lib 
        d3dcompiler.lib 
        dxgi.lib)
target_include_directories(tianli_frame
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        ${OpenCV_INCLUDE_DIRS}
)