#CMakeLists.txt to build project using cmake
#run $cmake CMakeLists.txt

cmake_minimum_required(VERSION 2.6)

project(inpainting)

FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(
GLOB_RECURSE
source_files
src/*.[cht]pp
)

SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_COMPILER "g++")
ADD_DEFINITIONS( -Wall -Wextra -std=c++11 -Wno-comment -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -fopenmp -DCORRECTION)

add_executable(
  inpainting
  ${source_files}
)

TARGET_LINK_LIBRARIES(inpainting -lm -ldl -lGLEW ${OpenCV_LIBRARIES} -fopenmp)
