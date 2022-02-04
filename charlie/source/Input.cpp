#include "Input.h"
#include "Config.h"

InputQueue::InputQueue()
{
}

void InputQueue::push(const Input& _input)
{
    bool inputIsAhead = true;
    for (auto it = inputQueue.begin(); it != inputQueue.end(); ++it) {
        if (_input.tick < it->tick) {
            inputQueue.insert(it, _input);
            inputIsAhead = false;
            break;
        }
        else if (_input.tick == it->tick) {
            *it = _input;
            inputIsAhead = false;
            break;
        }
    }
    if (inputIsAhead) inputQueue.push_back(_input);
    while (inputQueue.size() > INPUT_QUEUE_SIZE) {
        inputQueue.pop_front();
    }
}

void InputQueue::pruneQueue(const uint32 _tick)
{
    if (inputQueue.size() == 0) return;
    while (inputQueue.front().tick + INPUT_TICK_BUFFER < _tick) {
        inputQueue.pop_front();
        if (inputQueue.size() == 0) return;
    }
}

void InputQueue::clear()
{
    inputQueue.clear();
}

Input InputQueue::getInput(const uint32 _tick)
{
    for (auto&& input : inputQueue) {
        if (input.tick == _tick) return input;
    }
    return Input();
}

size_t InputQueue::size() const
{
    return inputQueue.size();
}
