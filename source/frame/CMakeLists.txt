add_library(tianli_frame INTERFACE)
add_library(tianli::frame ALIAS tianli_frame)

target_sources(tianli_frame
    PRIVATE
        frame.include.h)