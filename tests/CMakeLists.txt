
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(fmt REQUIRED)
find_package(cereal REQUIRED)

include_directories(${SourceDir})
enable_testing()
auto_subdirectory()
