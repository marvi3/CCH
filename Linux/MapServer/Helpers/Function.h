#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "Timer.h"
#include "Assert.h"
#include "Debug.h"

#include "String/String.h"

inline pid_t forkFailure() {
    error("Failure during fork()!");
    exit(1);
}

template<typename FUNCTION>
inline double timedFunctionCall(const FUNCTION& function, int maxTimeInMilliSeconds = 60000) {
    Timer timer;
    function();
    return timer.elapsedMilliseconds();
}

template<typename FUNCTION>
inline double timedAsynchronousFunctionCall(const FUNCTION& function, int maxTimeInMilliSeconds = 60000) {
    Timer timer;
    double* time = static_cast<double*>(mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    pid_t work = fork();
    if (work == -1) forkFailure();
    if (work == 0) {
        Timer timer;
        function();
        *time = timer.elapsedMilliseconds();
        exit(0);
    }
    pid_t timeout = fork();
    if (timeout == -1) forkFailure();
    if (timeout == 0) {
        sleep(maxTimeInMilliSeconds);
        exit(0);
    }
    pid_t finished = wait(NULL);
    while ((finished != work) && (finished != timeout)) {
        finished = wait(NULL);
    }
    if (finished == work) {
        kill(timeout, SIGKILL);
        waitpid(timeout, 0, 0);
        return *time;
    } else {
        kill(work, SIGKILL);
        waitpid(work, 0, 0);
        return timer.elapsedMilliseconds();
    }
}

template<typename FUNCTION>
inline double timedAsynchronousFunctionCall(const FUNCTION& function, int maxTimeInMilliSeconds, int intervallInMilliSeconds) {
    Timer timer;
    double* time = static_cast<double*>(mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    *time = -1;
    bool* done = static_cast<bool*>(mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    *done = false;
    pid_t child = fork();
    if (child == -1) forkFailure();
    if (child == 0) {
        Timer timer;
        function();
        *time = timer.elapsedMilliseconds();
        *done = true;
        exit(0);
    }
    while ((!(*done)) && (timer.elapsedMilliseconds() < maxTimeInMilliSeconds)) {
        sleep(intervallInMilliSeconds);
    }
    if (!(*done)) kill(child, SIGKILL);
    waitpid(child, 0, 0);
    if (*done) {
        return *time;
    } else {
        return timer.elapsedMilliseconds();
    }
}

template<typename FUNCTION>
inline double timedInterruptableFunctionCall(const FUNCTION& function, int maxTimeInMilliSeconds = 60000) {
    Timer timer;
    double* time = static_cast<double*>(mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    pid_t work = fork();
    if (work == -1) forkFailure();
    if (work == 0) {
        Timer timer;
        function();
        *time = timer.elapsedMilliseconds();
        exit(0);
    }
    pid_t timeout = fork();
    if (timeout == -1) forkFailure();
    if (timeout == 0) {
        getchar();
        exit(0);
    }
    pid_t finished = wait(NULL);
    while ((finished != work) && (finished != timeout)) {
        finished = wait(NULL);
    }
    if (finished == work) {
        kill(timeout, SIGKILL);
        waitpid(timeout, 0, 0);
        return *time;
    } else {
        kill(work, SIGKILL);
        waitpid(work, 0, 0);
        return timer.elapsedMilliseconds();
    }
}
