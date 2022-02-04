#pragma once
#include "Entity.h"

class Projectile final : public Entity
{
	bool allied;
	bool alive;
public:
	Projectile();
	Projectile(const bool _allied, const Vector2& _pos);
	void update(const Time& _dt, const uint32 _tick) override;
	void draw(Renderer& _rend) override;
	void reset() override;
	Rectangle getRect() const;
	void kill();
	bool isAlive() const;
	bool isAllied() const;
};

