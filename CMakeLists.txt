cmake_minimum_required(VERSION 4.0)

project(test)

set(DEFAULT_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

find_package(Threads REQUIRED)

add_executable(
    main ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/util/parallel.cpp
    ${PROJECT_SOURCE_DIR}/util/color.cpp
    ${PROJECT_SOURCE_DIR}/util/colorspace.cpp
    ${PROJECT_SOURCE_DIR}/util/spectrum.cpp
    ${PROJECT_SOURCE_DIR}/util/vecmath.cpp
    )

target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/util)
target_compile_features(main PRIVATE cxx_std_20)

target_link_libraries(main PRIVATE Threads::Threads)
