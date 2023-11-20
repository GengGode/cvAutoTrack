get_filename_component(dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_name test_${dir_name})
add_executable(${test_name} main.cpp ${SourceDir}/resources/binary/resources.binary.cpp)

add_definitions(-DUSED_BINARY_IMAGE)
set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "/MD")

target_link_libraries(${test_name} ${OpenCV_LIBS} gtest_main)
add_test(NAME ${test_name} COMMAND ${test_name})