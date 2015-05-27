add_executable(code_style 
	style.cpp)

add_dependencies(code_style
	glor_system)

include_directories(${Boost_INCLUDE_DIRS})
message (STATUS "boost include  ${Boost_INCLUDE_DIRS}")
message (STATUS "boost root  ${BOOST_ROOT}")

target_link_libraries (code_style
	glor_system
	rt
	pthread
	${Boost_SYSTEM_LIBRARY})
