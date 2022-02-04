#pragma once
#include "Config.h"
#include <deque>

using namespace charlie;

struct TimePlace {
    Vector2 vector;
    interp_t t;
    TimePlace() 
        : t(0)
    {

    }
    TimePlace(const Vector2& _vec, const interp_t _tick)
        : vector(_vec)
        , t(_tick)
    {

    }
};

class Interpolator
{
    std::deque<TimePlace> deq;
    interp_t interp;
    TimePlace current;
    TimePlace next;
    Time toTime(const uint32& _tick);
public:
    Interpolator(const Vector2& _startPos);
    Vector2 lerp();
    void push(const Vector2& _pos, const interp_t& _t);
    void increaseInterp(const interp_t& _dt);
    void setInterp(const interp_t& _t);
    void reset(const Vector2& _startPos);
    void clearQueue();
    TimePlace getCurrent() const;
};