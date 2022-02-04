#include "Inputinator.h"

Inputinator::Inputinator()
{
    
}

void Inputinator::recordInput(const Input& _input, const Vector2& _position)
{
    bool inputIsAhead = true;
    for (auto it = predictions.begin(); it != predictions.end(); ++it) {
        if (_input.tick < it->input.tick) {
            predictions.insert(it, { _input, _position });
            inputIsAhead = false;
            break;
        }
        else if (_input.tick == it->input.tick) {
            it->input = _input;
            inputIsAhead = false;
            break;
        }
    }
    if (inputIsAhead) predictions.push_back({ _input, _position });
    while (predictions.size() > MAX_INPUTS_SAVED) {
        predictions.pop_front();
    }
}

Vector2 Inputinator::getCorrectedPosition(Vector2 _authorativePosition, const uint32 _tick, const float _speed, const float _width)
{
    for (auto it = predictions.begin(); it != predictions.end(); ++it) {
        assert(it->input.tick > _tick);
        if (it->input.left) {
            _authorativePosition.x_ -= _speed;
            if (_authorativePosition.x_ < 0.0f) _authorativePosition.x_ = 0.0f;
        }
        if (it->input.right) {
            _authorativePosition.x_ += _speed;
            if (_authorativePosition.x_ + _width > WINW) _authorativePosition.x_ = WINW - _width;
        }
        it->position = _authorativePosition;
    }
    return _authorativePosition;
}

bool Inputinator::checkAndPrunePredictions(const Vector2& _authorativePosition, const uint32 _tick)
{
    for (auto it = predictions.begin(); it != predictions.end(); ++it) {
        if (it->input.tick == _tick) {
            Vector2 displacement = _authorativePosition - it->position;
            predictions.erase(predictions.begin(), it);
            return displacement.length() <= INPUT_TOLERANCE;
        }
    }
    return false;
}

void Inputinator::pruneQueue(const uint32 _tick)
{
    if (predictions.size() == 0) return;
    while (predictions.front().input.tick < _tick) {
        predictions.pop_front();
        if (predictions.size() == 0) {
            return;
        }
    }
}

void Inputinator::clear()
{
    predictions.clear();
}

DynamicArray<Input> Inputinator::getInputs()
{
    DynamicArray<Input> ret;
    ret.reserve(predictions.size());
    for (auto it = predictions.rbegin(); it != predictions.rend(); ++it) {
        ret.push_back(it->input);
    }
    return ret;
}
