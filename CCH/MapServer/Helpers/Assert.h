// Framework Version 2

#pragma once

#include <iostream>
#include <string>
#include <cassert>

#define Assert(assumption) assert(assumption)
#define AssertMsg(assumption, msg) assert(assumption || (std::cout << "\n\033[31mASSERTION FAILED: " << msg << "\033[0m\nFile: " << __FILE__ << "\nLine: " << __LINE__ << "\n" << std::flush && false))

template<typename T>
std::ostream& warning(const T& c) {return std::cout << "\n\033[33mWARNING: " << c << "\033[0m\n" << std::flush;}

template<typename T>
std::ostream& error(const T& c) {return std::cout << "\n\033[31mERROR: " << c << "\033[0m\n" << std::flush;}

void checkAsserts() {
#ifdef NDEBUG
	std::cout << "Running in release-mode!" << std::endl;
	assert(false);
#else
	std::cout << "Running with active assertions!" << std::endl;
#endif
}
