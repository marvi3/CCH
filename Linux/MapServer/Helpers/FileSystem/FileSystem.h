#pragma once

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include <dirent.h>

#include "../String/String.h"

namespace FileSystem {

	inline std::string getWorkingDirectory() {
		char buff[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
		if (len != -1) {
		    buff[len] = '\0';
		    std::string path(buff);
		    return path.substr(0, path.rfind('/'));
		}
		return "/home";
	}

	inline std::vector<std::string> getFiles(const std::string& path) {
		DIR* dir;
		struct dirent* ent;
		if ((dir = opendir(path.c_str())) != NULL) {
			std::vector<std::string> dirs;
			while ((ent = readdir(dir)) != NULL) {
				dirs.push_back(ent->d_name);
			}
			closedir(dir);
			std::sort(dirs.begin(), dirs.end());
			return dirs;
		}
		return std::vector<std::string>();
	}

	inline bool isDirectory(const std::string& path) {
		DIR* dir;
		if ((dir = opendir(path.c_str())) != NULL) {
			closedir(dir);
			return true;
		} else {
			return false;
		}
	}

	inline bool isFile(const std::string& path) {
		std::ifstream f(path.c_str());
		if (f.good()) {
			f.close();
			return true;
		} else {
			return false;
		}
	}

	inline bool isFileOrDirectory(const std::string& path) {
		return isFile(path) || isDirectory(path);
	}

	inline std::string extendPath(const std::string& base, const std::string& file) {
		if (file.size() < 1) {
			return base;
		} else if (file[0] == '/' || base.empty()) {
			return file;
		} else if (file[0] == '.') {
			if (file.size() < 2) {
				return base;
			} else if (file[1] != '.') {
				return "/FileNotFound";
			}
			int splitPos = base.rfind('/');
			if (splitPos < 0) {
				return "/FileNotFound";
			} else if (file.size() <= 3) {
				return base.substr(0, splitPos);
			} else if (file[2] != '/') {
				return "/FileNotFound";
			} else {
				return extendPath(base.substr(0, splitPos), file.substr(3));
			}
		}
		if (base[base.length() - 1] == '/') {
	        return base + file;
		} else {
	        return base + "/" + file;
		}
	}

	inline std::string ensureExtension(const std::string& file, const std::string& extension) {
	    if (String::endsWith(file, extension)) {
	        return file;
	    } else {
	        return file + extension;
	    }
	}

}
