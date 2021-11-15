#pragma once

#include <map>
#include <string>
#include <vector>

#include "../String/String.h"

class CommandLineParser {

public:
	CommandLineParser(int argc, char** argv) {
		for (int currentIndex = 1; currentIndex < argc; ++currentIndex) {
			if (argv[currentIndex][0] == '-') {
				std::string key(&argv[currentIndex][1]);
				std::string value("");
				if ((currentIndex + 1) < argc) value.assign(argv[currentIndex+1]);
				arguments[key] = value;
			}
		}
	}

	CommandLineParser(const std::string& input) {
		std::vector<std::string> argv = String::split(String::trim(input), ' ');
		for (int currentIndex = 1; currentIndex < argv.size(); ++currentIndex) {
			if (argv[currentIndex][0] == '-') {
				std::string key(&argv[currentIndex][1]);
				std::string value("");
				if ((currentIndex + 1) < argv.size()) value.assign(argv[currentIndex+1]);
				arguments[key] = value;
			}
		}
	}

	template<typename T>
	inline T value(const std::string key, const T defaultValue = T()) {
		if (arguments.find(key) != arguments.end()) {
			return String::lexicalCast<T>(arguments[key]);
		} else {
			return defaultValue;
		}
	}

	template<typename T>
	inline T get(const std::string key, const T defaultValue = T()) {
		return value<T>(key, defaultValue);
	}

	inline bool isSet(const std::string key) {
		return arguments.find(key) != arguments.end();
	}

private:
	std::map<std::string, std::string> arguments;

};
