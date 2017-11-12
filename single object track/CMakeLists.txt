cmake_minimum_required(VERSION 3.7)
project(trackingpath_)

set(CMAKE_CXX_STANDARD 11)
find_package( OpenCV REQUIRED )
set(SOURCE_FILES main.cpp)
add_executable(trackingpath_ ${SOURCE_FILES})
target_link_libraries( trackingpath_ ${OpenCV_LIBS} )