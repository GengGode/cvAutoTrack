
set(OpenCV_DIR "${ThridPatryDir}/opencv-xfeatures2d-shared/x64/vc17/lib")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
#gtest
include(FetchContent)
FetchContent_Declare(gtest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG v1.14.0)
FetchContent_MakeAvailable(gtest)
include_directories(${SourceDir})
enable_testing()
auto_subdirectory()