cmake_minimum_required(VERSION 3.9)
project(ImageSegmentation)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ImageSegmentation main.cpp)
target_link_libraries(ImageSegmentation ${OpenCV_LIBS})
