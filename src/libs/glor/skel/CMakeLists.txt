cmake_minimum_required (VERSION 2.6)
project (glor_skel CXX)

include("../lib_options.cmake")

file(GLOB glor_skel_sources # All files in directory:
	"*.h"
	"*.hpp"
	"*.cpp"
)

add_library (glor_skel STATIC ${glor_skel_sources})
set_target_properties (glor_skel PROPERTIES OUTPUT_NAME "glor_skel")

# target_link_libraries (upnpc-static ${LDLIBS}) # to add used libs

