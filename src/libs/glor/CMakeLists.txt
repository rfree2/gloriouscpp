message (STATUS "Build libs")
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS  "-std=c++11 -W -Werror --pedantic -g3 -O0")

add_subdirectory(system)
add_subdirectory(ipc)
add_subdirectory(skel)

