get_filename_component(dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_name test_${dir_name})
add_executable(${test_name} main.cpp
"${SourceDir}/genshin/genshin.handle.cpp"
"${SourceDir}/genshin/genshin.screen.cpp"
"${SourceDir}/genshin/check/paimon/genshin.check.paimon.cpp"
"${SourceDir}/genshin/cailb/minimap/genshin.cailb.minimap.cpp"
"${SourceDir}/match/surf/SurfMatch.cpp"
"${SourceDir}/utils/Utils.cpp"
"${SourceDir}/resources/Resources.cpp"
"${SourceDir}/resources/resources.load.cpp"
"${SourceDir}/resources/binary/resources.binary.cpp"
)
set_property(TARGET ${test_name} PROPERTY  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
target_compile_options(${test_name} PRIVATE "/W2")
string(REPLACE "_" ";" test_name_splits ${test_name})
list(GET test_name_splits 1 test_group_name)
set_target_properties(${test_name} PROPERTIES FOLDER "tests/${test_group_name}")
#set_target_properties(${test_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutputPath)")
target_link_libraries(${test_name} ${OpenCV_LIBS}
    cereal::cereal
    fmt::fmt-header-only )
add_test(NAME ${test_name} COMMAND ${test_name})
# copy opencv
add_custom_command(TARGET ${test_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OpenCV_CONFIG_PATH}/../bin/opencv_core480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_calib3d480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_features2d480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_flann480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_highgui480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_imgcodecs480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_imgproc480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_video480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_videoio480$<$<CONFIG:Debug>:d>.dll"
        "${OpenCV_CONFIG_PATH}/../bin/opencv_xfeatures2d480$<$<CONFIG:Debug>:d>.dll"
        $<TARGET_FILE_DIR:${test_name}>)