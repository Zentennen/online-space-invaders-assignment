#include "Entity.h"

//id_t Entity::generateID()
//{
//	static id_t counter = 0;
//	return counter++;
//}

bool Entity::areColliding(const Rectangle& a, const Rectangle& b)
{
	return a.x_ + a.width_ > b.x_ &&
		b.x_ + b.width_ > a.x_ &&
		a.y_ + a.height_ > b.y_ &&
		b.y_ + b.height_ > a.y_;
}

Entity::Entity(const Vector2& _pos) :
	id(INVALID_ENTITY_ID), position(_pos), interpolator(_pos)
{

}

void Entity::update(const Time& _dt, const uint32 _tick)
{
	
}

void Entity::draw(Renderer& _rend)
{

}

bool Entity::checkCollision(const Rectangle& _rect, const id_t projectileID)
{
	return false;
}

void Entity::interpolate(const interp_t _t, const Time& _dt)
{
	interpolator.increaseInterp(1);
	position = interpolator.lerp();
}

void Entity::initializeInterpolator(const interp_t _interp)
{
	interpolator.setInterp(_interp);
	position = interpolator.lerp();
}

void Entity::setID(const id_t _id)
{
	auto oldid = id;
	id = _id;
	if (oldid != INVALID_ENTITY_ID && id != INVALID_ENTITY_ID) {
		printf("WRN: Entity with valid id was set to id %c\n", _id);
		assert(false); //todo
	}
}

id_t Entity::getID() const
{
	return id;
}
