#pragma once

#include <iostream>
#include <vector>
#include <string>
using std::vector;

#include "../Helpers.h"
#include "../Assert.h"

namespace Vector {

	template<typename T>
	inline int count(const std::vector<T>& container, const T& element) {
		int c = 0;
		for (int i = 0; i < container.size(); i++) {
			if (container[i] == element) c++;
		}
		return c;
	}

	template<typename T>
	inline int indexOf(const std::vector<T>& vec, const T& elem) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] == elem) return i;
		}
		return -1;
	}

	template<typename T>
	inline std::vector<T> reverse(std::vector<T>&& a) {
		std::vector<T> result(a);
		std::reverse(result.begin(), result.end());
		return result;
	}

	template<typename T>
	inline std::vector<T>& reverse(std::vector<T>& a) {
		std::reverse(a.begin(), a.end());
		return a;
	}

	template<typename T>
	inline T& remove(T& array, int index) {
		Assert(index >= 0);
		Assert(index < array.size());
		array[index] = array.back();
		array.pop_back();
		return array;
	}

	template<typename T>
	inline bool contains(const std::vector<T>& container, const T& element) {
		for (int i = 0; i < container.size(); i++) {
			if (container[i] == element) {
				return true;
			}
		}
		return false;
	}

	template<typename T>
	inline bool equals(const std::vector<T>& a, const std::vector<T>& b) {
		if (a.size() != b.size()) return false;
		for (int i = 0; i < a.size(); i++) {
			if (!(a[i] == b[i])) return false;
		}
		return true;
	}

    template<typename T, typename LESS>
    inline bool isSorted(const std::vector<T>& vec, const LESS& less) {
        for (int i = 0; i + 1 < vec.size(); i++) {
            if (less(vec[i + 1], vec[i])) return false;
        }
        return true;
    }

    template<typename T>
    inline bool isSorted(const std::vector<T>& vec) {
        return isSorted(vec, [](const T& a, const T& b){return a < b;});
    }

	template<typename T>
	inline long long byteSize(const std::vector<T>& vec) {
		return sizeof(T) * vec.size();
	}

	template<>
	inline long long byteSize(const std::vector<bool>& vec) {
		return vec.size();
	}

	template<typename T>
	inline long long byteSize(const std::vector<std::vector<T>>& vec) {
		long long size = 0;
		for (const std::vector<T>& e : vec) size += sizeof(int) + byteSize(e);
		return size;
	}

    template<typename T, typename LESS>
    inline T max(const std::vector<T>& vec, const LESS& less) {
        Assert(!vec.empty());
        T result = vec.front();
        for (const T& element : vec) {
            if (less(result, element)) result = element;
        }
        return result;
    }

    template<typename T, typename LESS>
    inline T min(const std::vector<T>& vec, const LESS& less) {
        Assert(!vec.empty());
        T result = vec.front();
        for (const T& element : vec) {
            if (less(element, result)) result = element;
        }
        return result;
    }

	template<typename T>
	inline T max(const std::vector<T>& vec) {
		max(vec, [](const T& a, const T& b){return a < b;});
	}

	template<typename T>
	inline T min(const std::vector<T>& vec) {
        min(vec, [](const T& a, const T& b){return a < b;});
	}

}

template<typename T>
inline std::vector<T> operator+(std::vector<T>&& a, std::vector<T>&& b) {
    std::vector<T> result(a);
    result.reserve(result.size() + b.size());
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

template<typename T>
inline std::vector<T>& operator+=(std::vector<T>& a, const std::vector<T>& b) {
    a.reserve(a.size() + b.size());
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T>& a) {
	for (int i = 0; i < a.size(); i++) {
		out << a[i] << std::endl;
	}
	return out;
}
