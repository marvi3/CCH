#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <unistd.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

#include "Assert.h"
#include "String/String.h"

inline bool printStackTrace(std::ostream& out = std::cout, std::size_t maxFrames = 63) {
    out << "Stack trace at " << String::timeString() << ": " << std::endl;
    void* addressList[maxFrames + 1];

    std::size_t numFrames = backtrace(addressList, maxFrames);
    if (numFrames <= 0) {
        out << "    ERROR! No stack trace available!" << std::endl;
        return false;
    }

    char** symbolList = backtrace_symbols(addressList, numFrames);

    std::size_t functionNameSize = 256;
    char* functionName = (char*)malloc(functionNameSize);

    for (int i = 0; i < numFrames; i++) {
        char* name = 0;
        char* caller = 0;
        for (char* j = symbolList[i]; *j; j++) {
            if (*j == '(') {
                *j = '\0';
                name = j + 1;
            } else if (*j == '+') {
                *j = '\0';
                caller = j + 1;
            } else if (*j == ')' && caller) {
                *j = '\0';
                break;
            }
        }
        if (name && caller && name < caller) {
            int status;
            char* demangledFunctionName = abi::__cxa_demangle(name, functionName, &functionNameSize, &status);
            if (status == 0) {
                functionName = demangledFunctionName;
                out << "   " << symbolList[i] << ": " << functionName << "  [" << caller << "]" << std::endl;
            }
            else {
                out << "   " << symbolList[i] << ": " << name << "  [" << caller << "]" << std::endl;
            }
        } else {
            out << "    ERROR! Could not parse: " << symbolList[i] << std::endl;
        }

    }

    free(functionName);
    free(symbolList);
    return false;
}
