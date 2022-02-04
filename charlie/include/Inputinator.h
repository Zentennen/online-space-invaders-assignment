#pragma once
#include "Config.h"
#include "Input.h"
#include <deque>

class Inputinator
{
	struct Prediction {
		Input input;
		Vector2 position;
	};
	std::deque<Prediction> predictions;
public:
	Inputinator();
	void recordInput(const Input& _input, const Vector2& _position);
	Vector2 getCorrectedPosition(Vector2 _authorativePosition, const uint32 _tick, const float _speed, const float _width);
	bool checkAndPrunePredictions(const Vector2& _authorativePosition, const uint32 _tick);
	void pruneQueue(const uint32 _tick);
	void clear();
	DynamicArray<Input> getInputs();
};