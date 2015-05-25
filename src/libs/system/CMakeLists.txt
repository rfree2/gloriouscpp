cmake_minimum_required (VERSION 2.6)
project (glor_system CXX)


file(GLOB glor_system_sources # All files in directory:
	"*.h"
	"*.hpp"
	"*.cpp"
)

add_library (glor_system STATIC ${glor_system_sources})
set_target_properties (glor_system PROPERTIES OUTPUT_NAME "glor_system")

# target_link_libraries (upnpc-static ${LDLIBS}) # to add used libs

