
#pragma once
#ifndef __INCLUDED_H_libsimpleconvert_hpp 
#define __INCLUDED_H_libsimpleconvert_hpp 1

#include <vector>
#include <string>

std::string vec_to_str(const std::vector<char> &vec);

std::vector<char> str_to_vec(const std::string &str);

std::vector<char> str_to_vec(const char * const cstr);


#endif


