get_filename_component(dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(test_name test_${dir_name})
set(CMAKE_CXX_STANDARD 20)
add_compile_options(/std:c++20)
add_executable(${test_name} main.cpp  
${SourceDir}/resources/Resources.cpp
${SourceDir}/resources/resources.load.cpp
${SourceDir}/resources/binary/resources.binary.cpp
)
set_property(TARGET ${test_name} PROPERTY  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_options(${test_name} PRIVATE "/W4")
string(REPLACE "_" ";" test_name_splits ${test_name})
list(GET test_name_splits 1 test_group_name)
set_target_properties(${test_name} PROPERTIES FOLDER "tests/${test_group_name}")
target_link_libraries(${test_name} ${OpenCV_LIBS} cereal::cereal fmt::fmt-header-only)
add_test(NAME ${test_name} COMMAND ${test_name})