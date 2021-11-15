#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <random>

#include "Vector.h"

#include "../Assert.h"

/*
 * perm = permutation = oldToNew
 * ord  = order       = newToOld
 */

typedef std::vector<int> Order;
typedef std::vector<int> Permutation;

namespace Vector {

	inline Permutation id(int n) {
	    Permutation result(n);
		for (int i = 0; i < n; i++) result[i] = i;
		return result;
	}

	inline Permutation randomPermutation(int n, int seed = 42) {
		std::srand(seed);
		Permutation result(id(n));
		for (int i = 0; i < n; i++) std::swap(result[i], result[std::rand() % n]);
		return result;
	}

	template<typename T>
	inline Order getSortOrder(const std::vector<T>& vec) {
	    Order result(id(vec.size()));
		std::sort(result.begin(), result.end(), [&](const int& a, const int& b){return vec[a] < vec[b];});
		return result;
	}

	template<typename T, typename Compare>
	inline Order getSortOrder(const std::vector<T>& vec, Compare comp) {
	    Order result(id(vec.size()));
		std::sort(result.begin(), result.end(), [&](const int& a, const int& b){return comp(vec[a], vec[b]);});
		return result;
	}

	inline bool isPermutation(const Permutation& perm) {
		std::vector<bool> seen(perm.size(), false);
		for (int i : perm) {
			if (i < 0) return false;
			if (i >= perm.size()) return false;
			if (seen[i]) return false;
			seen[i] = true;
		}
		return true;
	}
	inline bool isOrder(const Order& order) {
		return isPermutation(order);
	}

	template<typename T>
	inline std::vector<T> getOrdered(const std::vector<T>& vec, const Order& order) {
		Assert(vec.size() == order.size());
		Assert(isOrder(order));
		std::vector<T> result(vec.size());
		for (int i = 0; i < vec.size(); i++) result[i] = vec[order[i]];
		return result;
	}
	template<typename T>
	inline std::vector<T>& applyOrder(std::vector<T>& vec, const Order& order) {
		Assert(vec.size() == order.size());
		Assert(isOrder(order));
		std::vector<bool> seen(vec.size(), false);
		for (int i = 0; i < vec.size(); i++) {
			if (seen[i]) continue;
			int index = i;
			T value = vec[index];
			while (!seen[order[index]]) {
				seen[index] = true;
				vec[index] = vec[order[index]];
				index = order[index];
			}
			seen[index] = true;
			vec[index] = value;
		}
	}
	template<typename T>
	inline std::vector<std::vector<T>>& applyOrder(std::vector<std::vector<T>>& vec, const Order& order) {
		Assert(vec.size() == order.size());
		Assert(isOrder(order));
		std::vector<bool> seen(vec.size(), false);
		for (int i = 0; i < vec.size(); i++) {
			if (seen[i]) continue;
			int index = i;
			while (!seen[order[index]]) {
				seen[index] = true;
				vec[index].swap(vec[order[index]]);
				index = order[index];
			}
			seen[index] = true;
		}
	}

	template<typename T>
	inline std::vector<T> getPermuted(const std::vector<T>& vec, const Permutation& perm) {
		Assert(vec.size() == perm.size());
		Assert(isPermutation(perm));
		std::vector<T> result(vec.size());
		for (int i = 0; i < vec.size(); i++) result[perm[i]] = vec[i];
		return result;
	}
	template<typename T>
	inline std::vector<T>& applyPermutation(std::vector<T>& vec, const Permutation& perm) {
		Assert(vec.size() == perm.size());
		Assert(isPermutation(perm));
		std::vector<bool> seen(vec.size(), false);
		for (int i = 0; i < vec.size(); i++) {
			if (seen[i]) continue;
			seen[i] = true;
			int index = perm[i];
			T value = vec[i];
			while (!seen[index]) {
				seen[index] = true;
				vec[i] = vec[index];
				vec[index] = value;
				value = vec[i];
				index = perm[index];
			}
		}
		return vec;
	}
	template<typename T>
	inline std::vector<std::vector<T>>& applyPermutation(std::vector<std::vector<T>>& vec, const Permutation& perm) {
		Assert(vec.size() == perm.size());
		Assert(isPermutation(perm));
		std::vector<bool> seen(vec.size(), false);
		for (int i = 0; i < vec.size(); i++) {
			if (seen[i]) continue;
			seen[i] = true;
			int index = perm[i];
			while (!seen[index]) {
				seen[index] = true;
				vec[i].swap(vec[index]);
				index = perm[index];
			}
		}
		return vec;
	}

	inline Permutation getInverse(const Permutation& vec) {
		Assert(isPermutation(vec));
		Permutation result(vec.size());
		for (int i = 0; i < vec.size(); i++) result[vec[i]] = i;
		return result;
	}
	inline Permutation& applyInversion(Permutation& vec) {
		Assert(isPermutation(vec));
		std::vector<bool> seen(vec.size(), false);
		for (int i = 0; i < vec.size(); i++) {
			if (seen[i]) continue;
			int value = i;
			int index = vec[value];
			while (!seen[index]) {
				seen[index] = true;
				vec[i] = vec[index];
				vec[index] = value;
				value = index;
				index = vec[i];
			}
		}
		return vec;
	}

    inline void mapPermutationAssertFree(std::vector<int>& vec, const Permutation& perm) {
        for (int i = 0; i < vec.size(); i++) {
            if (vec[i] >= 0 && vec[i] < perm.size()) vec[i] = perm[vec[i]];
        }
    }

    template<typename T>
    inline void mapPermutationAssertFree(std::vector<T>& vec, const Permutation& perm) {
        for (T& e : vec) mapPermutationAssertFree(e, perm);
    }

	inline void mapPermutation(std::vector<int>& vec, const Permutation& perm) {
		Assert(isPermutation(perm));
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] >= 0 && vec[i] < perm.size()) vec[i] = perm[vec[i]];
		}
	}

	template<typename T>
	inline void mapPermutation(std::vector<T>& vec, const Permutation& perm) {
        Assert(isPermutation(perm));
		for (T& e : vec) mapPermutationAssertFree(e, perm);
	}

}
