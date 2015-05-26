cmake_minimum_required(VERSION 2.6)
project (GLORIUS_CPP)

set(CMAKE_CXX_FLAGS  "-std=c++11 -W -Werror --pedantic -g3 -O0")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(src/libs)

add_subdirectory(src/libs/glor)
add_subdirectory(src/examples)