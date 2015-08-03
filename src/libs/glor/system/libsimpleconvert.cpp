
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

#include "libsimpleconvert.hpp"


std::string vec_to_str(const std::vector<char> &vec) {
	return std::string(vec.cbegin(), vec.cend());
}

std::vector<char> str_to_vec(const std::string &str) {
	return std::vector<char>(str.cbegin(), str.cend());
}

std::vector<char> str_to_vec(const char * const cstr) {
	std::string str(cstr); // TODO optimize?
	return std::vector<char>(str.cbegin(), str.cend());
}


