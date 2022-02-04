#include "Interpolator.h"

Vector2 Interpolator::lerp()
{
	if (interp >= next.t) {
		current = next;
		for (auto it = deq.begin(); it != deq.end();) {
			if (interp >= it->t) {
				current = *it;
				if (interp > it->t + TICKS_PER_SECOND) it = deq.erase(it);
				else ++it;
			}
			else {
				next = *it;
				break;
			}
		}
	}
	if (current.t >= next.t) {
		return current.vector;
	}
	interp_t currentToInterp = interp - current.t;
	interp_t currentToNext = next.t - current.t;
	float progress = static_cast<float>(currentToInterp) / static_cast<float>(currentToNext);
	Vector2 ret = Vector2::lerp(current.vector, next.vector, progress);
	return ret;
}

void Interpolator::push(const Vector2& _pos, const interp_t& _t)
{
	if (_t < next.t) return;
	if (interp > _t) interp = _t - INTERP_DELAY_TICKS;
	for (auto it = deq.begin(); it != deq.end(); ++it) {
		if (_t < it->t) {
			deq.insert(it, TimePlace(_pos, _t));
			return;
		}
	}
	deq.push_back(TimePlace(_pos, _t));
}

void Interpolator::increaseInterp(const interp_t& _dt)
{
	interp += _dt;
}

void Interpolator::setInterp(const interp_t& _t)
{
	interp = _t - INTERP_DELAY_TICKS;
}

void Interpolator::reset(const Vector2& _startPos)
{
	interp = INTERP_DELAY_TICKS;
	current = TimePlace(_startPos, -INTERP_DELAY_TICKS);
	next = current;
}

void Interpolator::clearQueue()
{
	deq.clear();
}

TimePlace Interpolator::getCurrent() const
{
	return current;
}

//void Interpolator::initialize(const interp_t& _t)
//{
//	if (initialized) return;
//	printf("Interpolation initialized to %lli\n", _t);
//	initialized = true;
//	interp = _t - INTERP_DELAY_TICKS;
//}

Time Interpolator::toTime(const uint32& _tick)
{
	float seconds = static_cast<float>(_tick * UPDATE_RATE.as_seconds());
	return Time(seconds);
}

Interpolator::Interpolator(const Vector2& _startPos)
	: interp(-INTERP_DELAY_TICKS)
	, current(TimePlace(_startPos, -INTERP_DELAY_TICKS))
	, next(TimePlace(_startPos, -INTERP_DELAY_TICKS))
{
	
}
