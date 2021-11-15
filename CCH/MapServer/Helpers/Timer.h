// Framework Version 2

#pragma once

#include <sys/time.h>

class Timer {

public:
	Timer() : start(timestamp()) {}

	void restart() {
	    start = timestamp();
	}

	double elapsedMilliseconds() {
		double cur = timestamp();
		return cur - start;
	}

private:
	inline double timestamp() {
		timeval tp;
		gettimeofday(&tp, nullptr);
		double ms = static_cast<double>(tp.tv_usec) / 1000.0;
		return (tp.tv_sec * 1000) + ms;
	}

private:
    double start;

};

