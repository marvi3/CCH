#pragma once

#include <iostream>
#include <vector>

#include "Assert.h"
#include "Types.h"

template<typename TYPE>
class Range {

public:
	struct Iterator {
		inline bool operator!=(const Iterator& other) const {return i != other.i;}
		inline TYPE operator*() const {return i;}
		inline Iterator& operator++() {i++; return *this;}
		inline Iterator& operator+(int n) {i += n; return *this;}
		TYPE i;
	};

	Range(TYPE first = 0, TYPE last = -1) : first(first), last(last) {}

	inline Iterator begin() const {return Iterator({first});}
	inline Iterator end() const {return Iterator({last + 1});}
	inline int size() const {return last - first + 1;}
	inline TYPE operator[](int i) const {return first + i;}

	TYPE first;
	TYPE last;

};

template<typename RANGE>
class SubRange {

public:
	struct Iterator {
		inline bool operator!=(const Iterator& other) const {return i != other.i;}
		inline auto operator*() const {return range[i];}
		inline Iterator& operator++() {i++; return *this;}
		inline Iterator& operator+(size_t n) {i += n; return *this;}
		RANGE& range;
		size_t i;
	};

	SubRange(RANGE& range, int first = 0, int last = 0) : range(range), first(first), last(last) {}
    SubRange(RANGE& range, const std::vector<Id>& indices, const Id index) : range(range), first(indices[index]), last(indices[index + 1] - 1) {}
    SubRange(RANGE& range, const std::vector<size_t>& indices, const size_t index) : range(range), first(indices[index]), last(indices[index + 1] - 1) {}

	inline Iterator begin() const {return Iterator({range, first});}
	inline Iterator end() const {return Iterator({range, last + 1});}
	inline size_t size() const {return last - first + 1;}
	inline auto operator[](size_t i) const {return range[first + i];}

	RANGE& range;
	size_t first;
	size_t last;

};

template<typename INDEX, typename RANGE>
class IndexRange {

public:
	struct Iterator {
		inline bool operator!=(const Iterator& other) const {return i != other.i;}
		inline auto operator*() const {return index[range[i]];}
		inline Iterator& operator++() {i++; return *this;}
		inline Iterator& operator+(int n) {i += n; return *this;}
		INDEX& index;
		RANGE& range;
		int i;
	};

	IndexRange(INDEX& index, RANGE& range, int first = 0, int last = -1) : index(index), range(range), first(first), last(last) {}

	inline Iterator begin() const {return Iterator({index, range, first});}
	inline Iterator end() const {return Iterator({index, range, last + 1});}
	inline int size() const {return last - first + 1;}
	inline auto operator[](int i) const {return index[range[first + i]];}

	INDEX& index;
	RANGE& range;
	int first;
	int last;

};

class SparseRange {

public:
	struct Iterator {
		inline bool operator!=(const Iterator& other) const {return i != other.i;}
		inline int operator*() const {return i;}
		inline Iterator& operator++() {do i++; while (i < flag.size() && !flag[i]); return *this;}
		inline Iterator& operator+(int n) {for (int j = 0; j < n; j++) ++(*this); return *this;}
		const std::vector<bool>& flag;
		int i;
	};

	SparseRange(const std::vector<bool>& flag) : flag(flag), first(0), last(flag.size()) {
		while (first < last && !flag[first]) first++;
	}

	inline Iterator begin() const {return Iterator({flag, first});}
	inline Iterator end() const {return Iterator({flag, last});}

	const std::vector<bool>& flag;
	int first;
	int last;

};

template<typename RANGE>
class ReverseRange {

public:
	struct Iterator {
		inline bool operator!=(const Iterator& other) const {return i != other.i;}
		inline auto operator*() const {return range[i];}
		inline Iterator& operator++() {i--; return *this;}
		inline Iterator& operator+(int n) {i -= n; return *this;}
		RANGE& range;
		int i;
	};

	ReverseRange(RANGE& range) : range(range) {}

	inline Iterator begin() const {return Iterator({range, (int)(range.size()) - 1});}
	inline Iterator end() const {return Iterator({range, -1});}
	inline int size() const {return range.size();}
	inline auto operator[](int i) const {return range[i];}

	RANGE& range;

};

template<typename RANGE>
inline ReverseRange<const RANGE> descending(const RANGE& range) {
	return ReverseRange<const RANGE>(range);
}
