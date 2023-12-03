get_filename_component(dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_name test_${dir_name})
add_executable(${test_name} main.cpp ${SourceDir}/resources/binary/resources.binary.cpp)

add_definitions(-DUSED_BINARY_IMAGE)
set_property(TARGET ${test_name} PROPERTY  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_target_properties(${test_name} PROPERTIES FOLDER "tests")
target_link_libraries(${test_name} ${OpenCV_LIBS} fmt::fmt-header-only)
add_test(NAME ${test_name} COMMAND ${test_name})