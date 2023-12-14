
set(OpenCV_DIR "${ThridPatryDir}/opencv-highgui-shared/x64/vc17/lib")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


include_directories(${SourceDir})
enable_testing()
auto_subdirectory()
