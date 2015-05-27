add_executable(code_style 
	style.cpp)

add_dependencies(code_style
	glor_system)

target_link_libraries (code_style
	glor_system
	${Boost_SYSTEM_LIBRARY}
	pthread)
