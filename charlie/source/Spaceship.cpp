#include "Spaceship.h"
#include "NetEventManager.h"
#include "DataOverlay.h"
#include "Inputinator.h"

const StaticArray<Color, 4> Spaceship::colors = { Color::Magenta, Color::Blue, Color::Green, Color::Yellow };

void Spaceship::moveLeft()
{
	if (position.x_ > 0.0f) {
		position.x_ -= SPACESHIP_SPEED;
		if (position.x_ < 0.0f) position.x_ = 0.0f;
	}
}

void Spaceship::moveRight()
{
	if (position.x_ + SPACESHIP_WIDTH < WINW) {
		position.x_ += SPACESHIP_SPEED;
		if (position.x_ > WINW - SPACESHIP_WIDTH) position.x_ = WINW - SPACESHIP_WIDTH;
	}
}

Spaceship::Spaceship()
	: Entity( Vector2(static_cast<float>(WINW / 2 - SPACESHIP_WIDTH / 2), SPACESHIP_Y) )
{
	static uint64 counter = 0;
	color = colors[counter];
	counter = posMod(++counter, colors.size());
}

void Spaceship::update(const Time& _dt, const uint32 _tick)
{
	clientUpdate(_dt, _tick);
	if (shootCooldown > 0.0f) shootCooldown -= _dt;
	if (shootCooldown < 0.0f) shootCooldown = 0.0f;

	inputQueue.pruneQueue(_tick);
	auto in = inputQueue.getInput(_tick);
	if (in.left) {
		moveLeft();
	}
	if (in.right) {
		moveRight();
	}
	if (in.shoot && shootCooldown <= 0.0f) {
		INetEventManager::push(network::EventProjectileSpawned(_tick, getProjectilePos(), true));
		shootCooldown = SPACESHIP_SHOOT_COOLDOWN;
	}
}

void Spaceship::draw(Renderer& _rend)
{
	Color col;
	col = (hitTimer <= 0.0f) ? color : Color::Red;
	_rend.render_rectangle({ static_cast<int>(position.x_), static_cast<int>(position.y_), SPACESHIP_WIDTH, SPACESHIP_HEIGHT }, col);
}

void Spaceship::reset()
{
	position = Vector2(static_cast<float>(WINW / 2 - SPACESHIP_WIDTH / 2), SPACESHIP_Y);
	shootCooldown = 0.0f;
	hitTimer = 0.0f;
	inputQueue.clear();
	interpolator.reset(position);
}

void Spaceship::correctPrediction(const Vector2& _authorativePos, const uint32 _tick, Inputinator& inputinator)
{
	if (!inputinator.checkAndPrunePredictions(_authorativePos, _tick)) {
		position = inputinator.getCorrectedPosition(_authorativePos, _tick, SPACESHIP_SPEED, SPACESHIP_WIDTH);
		DataOverlay::onMispredict();
	}
}

void Spaceship::predictInput(const bool left, const bool right, const uint32 _tick)
{
	if (left) {
		moveLeft();
	}
	if (right) {
		moveRight();
	}
}

bool Spaceship::checkCollision(const Rectangle& _rect, const id_t _projectileID)
{
	if (areColliding(getRect(), _rect)) {
		INetEventManager::push(network::EventSpaceshipHit(currentTick, id, _projectileID));
		return true;
	}
	return false;
}

network::MessageSpaceship Spaceship::getMessage() const
{
	return network::MessageSpaceship(id, static_cast<uint16>(position.x_));
}

Vector2 Spaceship::getProjectilePos() const
{
	return { position.x_ + SPACESHIP_WIDTH / 2 + PROJECTILE_W / 2, position.y_ - PROJECTILE_H };
}

Rectangle Spaceship::getRect() const
{
	return { static_cast<int32>(position.x_), static_cast<int32>(position.y_), SPACESHIP_WIDTH, SPACESHIP_HEIGHT };
}

void Spaceship::hit()
{
	hitTimer = SPACESHIP_HIT_FLASH_TIME;
}

void Spaceship::clientUpdate(const Time& _dt, const uint32 _tick)
{
	currentTick = _tick;
	if (hitTimer > 0.0f) hitTimer -= _dt;
	if (hitTimer < 0.0f) hitTimer = 0.0f;
}
