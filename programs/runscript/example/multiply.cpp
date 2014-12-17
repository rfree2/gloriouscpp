#!/usr/bin/env runscriptcpp11
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
using std::cout; using std::cin; using std::cerr; using std::endl; using std::vector; using std::string;

void Usage() {
	cout << "Call this program/script with two numbers." << endl;
}

int main(int argc, const char **argv) {
	if (!(argc>=1)) throw std::out_of_range("Program called without name?!");
	vector<string> args_make(argv+1, argv+argc);
	const vector<string> args(args_make);
	const string prog_name(argv[0]);
	cout << "Hi! This is script running in program " << prog_name << endl;

	try {
		auto x = std::stold(args.at(0)), y = std::stold(args.at(1));
		cout << x << "*" << y << " = " << (x*y) << endl;
	} catch(...) { Usage(); }
	cerr << "TEXT to CERR" << endl;
}

