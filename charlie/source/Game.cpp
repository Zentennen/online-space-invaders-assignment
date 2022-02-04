#include "Game.h"

using namespace network;

Game::Game(const bool _server)
	: spaceInvaders()
	, server(_server)
{

}

void Game::enter()
{
	lives = LIVES;
	tick = 0;
	projectileIndex = 0;
	localShipID = INVALID_ENTITY_ID;
	for (auto&& spaceship : spaceships) {
		spaceship.reset();
	}
	for (auto&& projectile : projectiles) {
		projectile.reset();
	}
	spaceInvaders.reset();
	for (id_t i = 0; i < spaceships.size(); i++) {
		spaceships[i].setID(i + 1);
	}
}

void Game::exit()
{
	spaceInvaders.interpolator.clearQueue();
	for (id_t i = 0; i < spaceships.size(); i++) {
		spaceships[i].setID(INVALID_ENTITY_ID);
		spaceships[i].interpolator.clearQueue();
	}
}

void Game::draw(Renderer& _rend)
{
	std::string lifeText("LIVES: ");
	lifeText += std::to_string(lives);
	_rend.render_text({ 1, WINH - UI_MARGIN }, Color::Lime, GAME_TEXT_SIZE, lifeText.c_str());
	for (auto&& projectile : projectiles) {
		projectile.draw(_rend);
	}
	spaceInvaders.draw(_rend);
	for (auto&& ship : spaceships) {
		if (ship.getID() != INVALID_ENTITY_ID) ship.draw(_rend);
	}
}

bool Game::update(const Time& _dt, const uint32 _tick)
{
	tick = _tick;
	if (!server) {
		interpolate(_dt, _tick);
		for (auto&& projectile : projectiles) {
			if (projectile.isAlive()) projectile.update(_dt, _tick);
		}
		for (auto&& ship : spaceships) {
			if (ship.getID() != INVALID_ENTITY_ID) ship.clientUpdate(_dt, _tick);
		}
		return true;
	}

	spaceInvaders.update(_dt, _tick);
	for (auto&& ship : spaceships) {
		if (ship.getID() != INVALID_ENTITY_ID) ship.update(_dt, _tick);
	}
	for (auto&& projectile : projectiles) {
		if (!projectile.isAlive()) continue;
		projectile.update(_dt, _tick);
		if (projectile.isAllied()) if (spaceInvaders.checkCollision(projectile.getRect(), projectile.getID())) {
			projectile.kill();
			continue;
		}
		if (!projectile.isAllied()) for (auto&& ship : spaceships) {
			if (ship.getID() != INVALID_ENTITY_ID) {
				if (ship.checkCollision(projectile.getRect(), projectile.getID())) {
					projectile.kill();
					continue;
				}
			}
		}
	}
	return true;
}

void Game::interpolate(const Time& _dt, const uint32 _tick)
{
	spaceInvaders.interpolate(_tick, _dt);
	for (auto&& ship : spaceships) {
		if (ship.getID() != INVALID_ENTITY_ID && ship.getID() != localShipID) ship.interpolate(_tick, _dt);
	}
}

void Game::handleEvent(NetEvent* e)
{
	switch (e->type) {
	case PROJECTILE_SPAWNED: {
		auto ev = (EventProjectileSpawned*)e;
		Vector2 vec;
		bool allied;
		ev->translate(vec, allied);
		projectileIndex = posMod(++projectileIndex, static_cast<uint16>(projectiles.size()));
		projectiles[projectileIndex] = Projectile(allied, vec);
		projectiles[projectileIndex].setID(projectileIndex + 1);
		ev->id = projectiles[projectileIndex].getID();
	} break;
	case ENEMY_HIT: {
		if (spaceInvaders.isDead()) {
			INetEventManager::push(EventGameOver(tick, true));
		}
	} break;
	case SPACESHIP_HIT: {
		auto ev = (EventSpaceshipHit*)e;
		spaceships[ev->id - 1].hit();
		if (lives > 0) --lives;
		ev->lives = lives;
		if (lives == 0) {
			INetEventManager::push(EventGameOver(tick, false));
		}
	} break;
	default:
		return;
	}
}

void Game::onKeyDown(const charlie::Keyboard::Key key)
{
	switch (key) {
	case Keyboard::Key::A: {
		if (localShipID != INVALID_ENTITY_ID && localShipID <= spaceships.size()) {
			spaceships[localShipID - 1].predictInput(true, false, tick);
		}
	} break;
	case Keyboard::Key::D: {
		if (localShipID != INVALID_ENTITY_ID && localShipID <= spaceships.size()) {
			spaceships[localShipID - 1].predictInput(false, true, tick);
		}
	} break;
	}
}

void Game::processEvent(NetEvent* e)
{
	switch (e->type) {
	case PROJECTILE_SPAWNED: {
		auto cast = (EventProjectileSpawned*)e;
		Vector2 vec;
		bool allied;
		cast->translate(vec, allied);
		projectiles[cast->id - 1] = Projectile(allied, vec);
		projectiles[cast->id - 1].setID(cast->id);
	} break;
	case ENEMY_HIT: {
		auto cast = (EventEnemyHit*)e;
		spaceInvaders.setStates(cast->enemyBitField);
		projectiles[cast->projectileID - 1].kill();
	} break;
	case SPACESHIP_HIT: {
		auto cast = (EventSpaceshipHit*)e;
		auto ship = getSpaceship(cast->id);
		ship->hit();
		lives = cast->lives;
		projectiles[cast->projectileID - 1].kill();
	} break;
	default: 
		return;
	}
}

void Game::initializeInterpolators(const interp_t& _interp)
{
	spaceInvaders.initializeInterpolator(_interp);
	for (uint16 i = 0; i < spaceships.size(); i++) {
		if (i + 1 == localShipID) continue;
		spaceships[i].initializeInterpolator(_interp);
	}
}

Spaceship* Game::getSpaceship(const id_t _id)
{
	if (_id > 0 && _id <= MAX_PLAYERS) {
		return &spaceships[_id - 1];
	}
	return nullptr;
}
