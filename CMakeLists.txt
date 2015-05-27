#cmake -DCMAKE_BUILD_TYPE=Release .
#cmake -DCMAKE_BUILD_TYPE=Debug .
#windows
#cmake -G "MSYS Makefiles" .
cmake_minimum_required(VERSION 2.6)
project (GLORIUS_CPP)

if (WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost 1.45.0 COMPONENTS REQUIRED system)
if (NOT Boost_FOUND)
  message (WARNING "Could not find Boost libraries, please make sure you have installed Boost, libboost-all-dev or the equivalent")
endif()

set(CMAKE_CXX_FLAGS  "-std=c++11 -W -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -NDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(GLO_INCLUDE_DIR ${PROJECT_BINARY_DIR}/src/libs)
include_directories(${GLO_INCLUDE_DIR})

add_subdirectory(src/libs/glor)
add_subdirectory(src/examples)
