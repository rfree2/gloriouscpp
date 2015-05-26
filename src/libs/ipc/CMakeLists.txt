cmake_minimum_required (VERSION 2.6)
project (glor_ipc CXX)

include("../lib_options.cmake")

file(GLOB glor_ipc_sources # All files in directory:
	"*.h"
	"*.hpp"
	"*.cpp"
)

add_library (glor_ipc STATIC ${glor_ipc_sources})
set_target_properties (glor_ipc PROPERTIES OUTPUT_NAME "glor_ipc")

# target_link_libraries (upnpc-static ${LDLIBS}) # to add used libs

