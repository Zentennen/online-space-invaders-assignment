#pragma once
#include "charlie.hpp"
#include <deque>

using namespace charlie;

struct Input {
	bool left = false;
	bool right = false;
	bool shoot = false;
	uint32 tick = 0;
	uint8 pack() const {
		uint8 ret = 0;
		if (left) ret |= 1;
		if (right) ret |= 1 << 1;
		if (shoot) ret |= 1 << 2;
		return ret;
	}
	Input() {}
	Input(const uint8 _bits, const uint32 _tick) : tick(_tick) {
		left = (_bits & 1) == 1;
		right = (_bits & (1 << 1)) == (1 << 1);
		shoot = (_bits & (1 << 2)) == (1 << 2);
	}
};

class InputQueue {
	std::deque<Input> inputQueue;
public:
	InputQueue();
	void push(const Input& _input);
	void pruneQueue(const uint32 _tick);
	void clear();
	Input getInput(const uint32 _tick);
	size_t size() const;
};