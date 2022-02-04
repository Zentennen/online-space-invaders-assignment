#pragma once
#include "Config.h"
#include "Interpolator.h"
#include "charlie_messages.hpp"

using namespace charlie;

class Interpolator;

class Entity
{
protected:
	id_t id;
public:
	Vector2 position;
	Interpolator interpolator;
	static bool areColliding(const Rectangle& a, const Rectangle& b);
	Entity(const Vector2& _pos);
	virtual void update(const Time& _dt, const uint32 _tick);
	virtual void draw(Renderer& _rend);
	virtual bool checkCollision(const Rectangle& _rect, const id_t _projectileID);
	virtual void interpolate(const interp_t _t, const Time& _dt);
	virtual void reset() = 0;
	void initializeInterpolator(const interp_t _interp);
	void setID(const id_t _id);
	id_t getID() const;
};

