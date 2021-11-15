#pragma once

#include <iostream>
#include <string>

#include "../Helpers/String/String.h"

struct Parameter {

    Parameter(string type = "", string name = "", string display = "", string defaultValue = "", bool recalculateOnChange = false, string min = "", string max = "", string step = "", int index = 0) :
        type(type),
        name(name),
        display(display),
        defaultValue(defaultValue),
        value(defaultValue),
        recalculateOnChange(recalculateOnChange),
        min(min),
        max(max),
        step(step),
        index(index) {
    }

    Parameter(string type, string name, string display, string defaultValue, bool recalculateOnChange, int index) :
        Parameter(type, name, display, defaultValue, recalculateOnChange, "", "", "", index) {
    }

	inline void writeParameter(std::ostream &stream) const {
		stream << "{";
		stream << "\"Type\":\"" << type << "\""
			   << ", \"Name\":\"" << name << "\""
			   << ", \"Display\":\"" << display << "\""
			   << ", \"DefaultValue\":\"" << defaultValue << "\""
               << ", \"Index\":\"" << index << "\"";
		if (!min.empty()) stream << ", \"MinValue\":\"" << min << "\"";
		if (!max.empty()) stream << ", \"MaxValue\":\"" << max << "\"";
		if (!step.empty()) stream  << ", \"Step\":\"" << step << "\"";
		if (recalculateOnChange) stream << ", \"RecalculateOnChange\":\"true\"";
	    stream << "}";
	}

	template <typename T>
	inline T getValue() const  {
		if (type == "bool") {
			return value == "true";
		} else {
			return String::lexicalCast<T>(value);
		}
	}

    template <typename T>
    inline T getMax() const {return String::lexicalCast<T>(max);}

    template <typename T>
    inline T getMin() const {return String::lexicalCast<T>(min);}

    template <typename T>
    inline T getDefault() const {return String::lexicalCast<T>(defaultValue);}

    template <typename T>
    inline void validate() {
        T v = getValue<T>();
        if (v < getMin<T>()) {
            value = min;
        } else if (v > getMax<T>()) {
            value = max;
        }
    }


	string type;
	string name;
	string display;
	string defaultValue;
	string value;
	bool recalculateOnChange;

    string min;
    string max;
    string step;

    int index;

};
