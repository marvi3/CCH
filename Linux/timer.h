#ifndef TIMER_H
#define TIMER_H

#include <time.h>

inline
long long get_micro_time(){
	clock_t t;
	t = clock();
	return t;
}

#endif
