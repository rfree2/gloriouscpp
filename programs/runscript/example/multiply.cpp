#!/usr/bin/env runscriptcpp11
#include <iostream>
#include <vector>

int main(int argc, const char **argv) {
	std::vector<std::string> args_make;
	for (int i=1; i<argc; ++i) args_make.push_back(argv[i]);
	const std::vector<std::string> args(args_make);
	const std::string prog_name(argv[0]);

	std::cout << "Hi! This is script running in program " << prog_name << ". ";

	auto x = std::stold(args.at(0));
	auto y = std::stold(args.at(1));
	std::cout << x << "*" << y << " = " << (x*y) << std::endl;

	std::cout << "TEXT to COUT" << std::endl;
	std::cerr << "TEXT to CERR" << std::endl;

	if ((x==0) && (y==0)) {
		int a=1,b=1;
		std::cout << "A test: enter two integers please separated by space or enter: ";
		std::cin >> a >> b;
		std::cout << a << " * " << b << " = " << (a*b) << std::endl;
	}
}

