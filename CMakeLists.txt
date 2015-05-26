#cmake -DCMAKE_BUILD_TYPE=Release .
#cmake -DCMAKE_BUILD_TYPE=Debug .
cmake_minimum_required(VERSION 2.6)
project (GLORIUS_CPP)

set(CMAKE_CXX_FLAGS  "-std=c++11 -W -Werror -wall -pedantic-errors")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -NDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(GLO_INCLUDE_DIR ${PROJECT_BINARY_DIR}/src/libs)
include_directories(${GLO_INCLUDE_DIR})

add_subdirectory(src/libs/glor)
add_subdirectory(src/examples)