cmake_minimum_required(VERSION 3.15)
project(opencvTest)
include(CTest)
enable_testing()
set(project_sources
   main.cpp
   GolomCode.cpp
   Bitstream.cpp
)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(opencvTest main.cpp GolombCode.cpp Bitstream.cpp)
# add_executable(golombTest main.cpp GolombCode.cpp Bitstream.cpp) 
target_link_libraries(opencvTest ${OpenCV_LIBS})