cmake_minimum_required (VERSION 2.6)
project (glor_ipc CXX)

include("../lib_options.cmake")

find_package(Boost 1.45.0 COMPONENTS REQUIRED system)
if(NOT Boost_FOUND)
  die("Could not find Boost libraries, please make sure you have installed Boost, libboost-all-dev or the equivalent")
endif()

file(GLOB glor_ipc_sources # All files in directory:
	"*.h"
	"*.hpp"
	"*.cpp"
)

add_library (glor_ipc STATIC ${glor_ipc_sources})
add_dependencies (glor_ipc ${Boost_SYSTEM_LIBRARY})
set_target_properties (glor_ipc PROPERTIES OUTPUT_NAME "glor_ipc")

# target_link_libraries (upnpc-static ${LDLIBS}) # to add used libs

