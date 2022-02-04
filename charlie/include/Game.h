#pragma once
#include "Spaceship.h"
#include "AppState.h"
#include "SpaceInvaders.h"
#include "NetEventManager.h"
#include "Projectile.h"
#include <map>

struct Player {
	uint64 id = 0;
	ReliableEventQueue eventQueue;
	uint16 lastSequenceChecked = 0;
};

class Game final : public AppState
{
	bool server;
public:
	StaticArray<Spaceship, MAX_PLAYERS> spaceships; 
	StaticArray<Projectile, MAX_PLAYERS* PROJECTILES_PER_PLAYER + PROJECTILES_FOR_ENEMIES> projectiles;
	uint16 projectileIndex = 0;
	SpaceInvaders spaceInvaders;
	uint32 tick = 0;
	uint8 lives = LIVES;
	id_t localShipID = INVALID_ENTITY_ID;
	Game(const bool _server);
	void enter() override;
	void exit() override;
	void draw(Renderer& _rend) override;
	bool update(const Time& _dt, const uint32 _tick) override;
	void interpolate(const Time& _dt, const uint32 _tick);
	void handleEvent(NetEvent* e) override;
	void onKeyDown(const charlie::Keyboard::Key key) override;
	void processEvent(NetEvent* e);
	void initializeInterpolators(const interp_t& _interp);
	Spaceship* getSpaceship(const id_t _id);
};

