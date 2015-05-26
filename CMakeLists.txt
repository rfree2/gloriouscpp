cmake_minimum_required(VERSION 2.6)
project (GLORIUS_CPP)

find_package(Boost 1.45.0 COMPONENTS interprocess)
set(CMAKE_CXX_FLAGS  "-std=c++11 -W -Werror --pedantic -g3 -O0")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

add_subdirectory(src/libs)
add_subdirectory(src/examples)