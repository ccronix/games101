cmake_minimum_required(VERSION 3.10)
project(Rasterizer)

set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)

find_package(Eigen3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Rasterizer main.cpp rasterizer.hpp rasterizer.cpp global.hpp Triangle.hpp Triangle.cpp)
target_link_libraries(Rasterizer ${OpenCV_LIBS})
