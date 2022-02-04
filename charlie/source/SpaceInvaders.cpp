#include "SpaceInvaders.h"
#include "NetEventManager.h"
#include <cmath>

SpaceInvaders::SpaceInvaders() 
	: Entity({ 0.0f, 0.0f })
	, enemyStates({})
{
	StaticArray<bool, ENEMY_ROWS> r;
	r.fill(true);
	enemyStates.fill(r);
}

void SpaceInvaders::update(const Time& _dt, const uint32 _tick)
{
	currentTick = _tick;
	shootCooldown -= _dt;
	if (shootCooldown <= 0.0f) {
		shoot();
	}

	switch (movementState) {
	case MovementState::LEFT: {
		position.x_ -= ENEMY_SPEED;
		if (position.x_ <= 0.0f) {
			position.x_ = 0.0f;
			movementState = (position.y_ >= ENEMY_MAX_Y) ? MovementState::RIGHT : MovementState::DOWN;
		}
	} break;
	case MovementState::RIGHT: {
		position.x_ += ENEMY_SPEED;
		if (position.x_ + w >= WINW) {
			position.x_ = static_cast<float>(WINW - w);
			movementState = (position.y_ >= ENEMY_MAX_Y) ? MovementState::LEFT : MovementState::DOWN;
		}
	} break;
	case MovementState::DOWN: {
		downTime += _dt;
		position.y_ += ENEMY_SPEED;
		if (position.y_ >= ENEMY_MAX_Y) {
			downTime += ENEMY_DOWN_SWITCH_TIME;
		}
		if (downTime >= ENEMY_DOWN_SWITCH_TIME) {
			downTime = 0.0f;
			if (position.x_ == 0) movementState = MovementState::RIGHT;
			else movementState = MovementState::LEFT;
		}
	} break;
	}
}

void SpaceInvaders::draw(Renderer& _renderer)
{
	for (uint8 i = 0; i < ENEMY_COLUMNS; i++) {
		for (uint8 j = 0; j < ENEMY_ROWS; j++) {
			if (enemyStates[i][j]) {
				_renderer.render_rectangle_fill(getRect(i, j), ENEMY_COLOR);
			}
		}
	}
}

bool SpaceInvaders::checkCollision(const Rectangle& _rect, const id_t _projectileID)
{
	if (!areColliding(_rect, { static_cast<int32>(position.x_), static_cast<int32>(position.y_), w, h })) return false;
	for (uint8 i = 0; i < ENEMY_COLUMNS; i++) {
		if (!areColliding(_rect, { static_cast<int32>(position.x_ + i * (ENEMY_SIZE + ENEMY_SPACING)), static_cast<int32>(position.y_), ENEMY_SIZE, h })) continue;
		for (uint8 j = 0; j < ENEMY_ROWS; j++) {
			if (enemyStates[i][j] && areColliding(getRect(i, j), _rect)) {
				enemyStates[i][j] = false;
				onCollision(_projectileID);
				return true;
			}
		}
	}
	return false;
}

void SpaceInvaders::setStates(const uint64 newStates)
{
	for (uint8 i = 0; i < ENEMY_COLUMNS; i++) {
		for (uint8 j = 0; j < ENEMY_ROWS; j++) {
			uint64 bitshift = newStates >> (i * ENEMY_ROWS + j);
			enemyStates[i][j] = (bitshift & 1) == 1;
		}
	}
}

bool SpaceInvaders::isDead() const
{
	return columns == 0;
}

void SpaceInvaders::reset()
{
	movementState = MovementState::RIGHT;
	downTime = 0.0f;
	shootCooldown = 0.0f;
	columns = ENEMY_COLUMNS;
	w = ENEMY_COLUMNS * (ENEMY_SIZE + ENEMY_SPACING);
	h = ENEMY_ROWS * (ENEMY_SIZE + ENEMY_SPACING);
	currentTick = 0;
	StaticArray<bool, ENEMY_ROWS> r;
	r.fill(true);
	enemyStates.fill(r);
	position = { 0.0f, 0.0f };
	interpolator.reset(position);
}

void SpaceInvaders::onCollision(const id_t _projectileID)
{
	if (columns > 0) {
		uint16 col = columns;
		while (enemyStates[--col] == StaticArray<bool, ENEMY_ROWS>({})) {
			--columns;
			w = columns * (ENEMY_SIZE + ENEMY_SPACING);
			if (columns == 0) break;
		}
	}
	INetEventManager::push(network::EventEnemyHit(currentTick, enemyStates, _projectileID));
}

void SpaceInvaders::shoot()
{
	StaticQueue<Vector2, ENEMY_COLUMNS * ENEMY_ROWS> living = getLiving();
	Random rng(rand());
	uint64 r = rng() % living.size();
	INetEventManager::push(network::EventProjectileSpawned(currentTick, living[r], false));
	shootCooldown = ENEMY_SHOOT_COOLDOWN;
}

Rectangle SpaceInvaders::getRect(const uint8 _x, const uint8 _y) const
{
	int32 xOffset = _x * (ENEMY_SIZE + ENEMY_SPACING);
	int32 yOffset = _y * (ENEMY_SIZE + ENEMY_SPACING);
	auto x = lround(position.x_) + xOffset;
	auto y = lround(position.y_) + yOffset;
	return { x, y, ENEMY_SIZE, ENEMY_SIZE };
}

Vector2 SpaceInvaders::getVec(const uint8 _x, const uint8 _y) const
{
	return Vector2(position.x_ +_x * (ENEMY_SIZE + ENEMY_SPACING), position.y_ + _y * (ENEMY_SIZE + ENEMY_SPACING));
}

network::MessageEnemies SpaceInvaders::getMessage() const
{
	return network::MessageEnemies(position);
}

StaticQueue<Vector2, ENEMY_COLUMNS* ENEMY_ROWS> SpaceInvaders::getLiving() const
{
	StaticQueue<Vector2, ENEMY_COLUMNS* ENEMY_ROWS> living;
	for (uint8 i = 0; i < enemyStates.size(); i++) {
		for (uint8 j = 0; j < enemyStates[i].size(); j++) {
			if (enemyStates[i][j]) {
				living.push(Vector2(ENEMY_SIZE / 2, ENEMY_SIZE / 2) + getVec(i, j));
			}
		}
	}
	return living;
}
