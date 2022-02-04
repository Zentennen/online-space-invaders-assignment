#pragma once
#include "Entity.h"
#include "Input.h"

class Inputinator;

class Spaceship final : public Entity
{
	static const StaticArray<Color, 4> colors;
	Color color;
	uint32 currentTick = 0;
	Time shootCooldown;
	Time hitTimer;
	void moveLeft();
	void moveRight();
public:
	InputQueue inputQueue;
	Spaceship();
	void update(const Time& _dt, const uint32 _tick) override;
	void draw(Renderer& _rend) override;
	void reset() override;
	void correctPrediction(const Vector2& _authorativePos, const uint32 _tick, Inputinator& inputinator);
	void predictInput(const bool left, const bool right, const uint32 _tick);
	bool checkCollision(const Rectangle& _rect, const id_t _projectileID) override;
	network::MessageSpaceship getMessage() const;
	Vector2 getProjectilePos() const;
	Rectangle getRect() const;
	void hit();
	void clientUpdate(const Time& _dt, const uint32 _tick);
};

