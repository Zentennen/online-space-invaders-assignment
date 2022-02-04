#pragma once
#include "Entity.h"
#include "StaticQueue.h"

class SpaceInvaders final : public Entity
{
	enum class MovementState {
		LEFT, RIGHT, DOWN
	};
	MovementState movementState = MovementState::RIGHT;
	Time downTime;
	Time shootCooldown;
	uint16 columns = ENEMY_COLUMNS;
	uint16 w = ENEMY_COLUMNS * (ENEMY_SIZE + ENEMY_SPACING);
	uint16 h = ENEMY_ROWS * (ENEMY_SIZE + ENEMY_SPACING);
	uint32 currentTick = 0;
	StaticArray<StaticArray<bool, ENEMY_ROWS>, ENEMY_COLUMNS> enemyStates;
	void onCollision(const id_t _projectileID);
	void shoot();
public:
	SpaceInvaders();
	void update(const Time& _dt, const uint32 _tick) override;
	void draw(Renderer& _renderer) override;
	bool checkCollision(const Rectangle& _rect, const id_t _projectileID) override;
	void setStates(const uint64 _newStates);
	bool isDead() const;
	void reset() override;
	Rectangle getRect(const uint8 _x, const uint8 _y) const;
	Vector2 getVec(const uint8 _x, const uint8 _y) const;
	network::MessageEnemies getMessage() const;
	StaticQueue<Vector2, ENEMY_COLUMNS * ENEMY_ROWS> getLiving() const;
};

