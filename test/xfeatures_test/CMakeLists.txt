add_executable(xfeatures_test main.cpp)
target_link_libraries(xfeatures_test ${OpenCV_LIBS})

add_test(NAME xfeatures_test COMMAND xfeatures_test)