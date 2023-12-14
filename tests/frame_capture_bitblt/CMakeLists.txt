get_filename_component(dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_name test_${dir_name})
add_executable(${test_name} main.cpp)
set_property(TARGET ${test_name} PROPERTY  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
string(REPLACE "_" ";" test_name_splits ${test_name})
list(GET test_name_splits 1 test_group_name)
set_target_properties(${test_name} PROPERTIES FOLDER "tests/${test_group_name}")
target_link_libraries(${test_name} ${OpenCV_LIBS})
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