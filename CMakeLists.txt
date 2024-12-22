cmake_minimum_required(VERSION 2.8)

project(test)

set(DEFAULT_CXX_STANDARD 11)

set(OpenCV_DIR  D:/openglEnv/Opencv/opencv/build/x64/mingw/lib/)#库路径你自己换成你电脑上的

aux_source_directory(UTIL ${PROJECT_SOURCE_DIR}/util)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} D:/openglEnv/Eigen/eigen/Eigen/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(main ${PROJECT_SOURCE_DIR}/main.cpp UTIL)

target_link_libraries(main  ${OpenCV_LIBS})
