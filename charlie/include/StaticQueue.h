#pragma once
#include <assert.h>
#include "Config.h"

template<typename T, size_t capacity>
class StaticQueue
{
	T data[capacity] = {};
	size_t queueSize = 0;
	size_t oldest = 0;
	size_t index = 0;
public:
	T& operator[](size_t i) {
		assert(i < queueSize);
		i += oldest;
		if (i >= capacity) i -= queueSize;
		return data[i];
	}
	size_t size() {
		return queueSize;
	}
	void push(const T& value) {
		data[index] = value;
		++index;
		if (index == capacity) index = 0;
		if (queueSize < capacity) ++queueSize;
		else {
			++oldest;
			if (oldest == capacity) oldest = 0;
		}
	}
	void push(T&& value) {
		data[index] = value;
		if (++index == capacity) index = 0;
		if (queueSize < capacity) ++queueSize;
		else {
			if (++oldest == capacity) oldest = 0;
		}
	}
	T& front() {
		assert(queueSize != 0);
		if (index == 0) return data[capacity - 1];
		else return data[index - 1];
	}
	T& back() {
		assert(queueSize != 0);
		return data[oldest];
	}
	void clear() {
		data = {};
		queueSize = 0;
		index = 0;
		oldest = 0;
	}
	void popFront() {
		if (++oldest == capacity) oldest = 0;
		if (queueSize != 0) --queueSize;
	}
	void popBack() {
		if (queueSize == 0) return;
		else if (index == 0) {
			index = capacity - 1;
		}
		else {
			--index;
		}
		--queueSize;
	}
	void removeAtScrambled(size_t at) {
		assert(at < queueSize);
		std::swap(*this[at], back());
		popBack();
	}
};