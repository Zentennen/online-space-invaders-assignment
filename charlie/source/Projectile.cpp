#include "Projectile.h"

Projectile::Projectile()
	: Entity({})
	, allied(false)
	, alive(false)
{
}

Projectile::Projectile(const bool _allied, const Vector2& _pos)
	: Entity(_pos)
	, allied(_allied)
	, alive(true)
{
}

void Projectile::update(const Time& _dt, const uint32 _tick)
{
	if (!alive) return;
	if (allied) {
		position.y_ -= PROJECTILE_SPEED;
		if (position.y_ + PROJECTILE_H > WINH - UI_MARGIN) {
			alive = false;
		}
	}
	else {
		position.y_ += PROJECTILE_SPEED;
	}
}

void Projectile::draw(Renderer& _rend)
{
	if (alive) _rend.render_rectangle_fill(getRect(), allied ? ALLIED_PROJECTILE_COLOR : ENEMY_PROJECTILE_COLOR);
}

void Projectile::reset()
{
	alive = false;
}

Rectangle Projectile::getRect() const
{
	return { static_cast<int32>(position.x_), static_cast<int32>(position.y_), PROJECTILE_W, PROJECTILE_H };
}

void Projectile::kill()
{
	position = {};
	alive = false;
}

bool Projectile::isAlive() const
{
	return alive;
}

bool Projectile::isAllied() const
{
	return allied;
}
