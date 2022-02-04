#pragma once
#include "charlie.hpp"
#include <iostream>
#include <math.h>

namespace charlie {
	namespace network {
		struct IPAddress;
	}
}

using namespace charlie;

#define PRINT(x) std::cout << #x << " = " << x << "\n";

typedef uint16 id_t;
typedef int64 interp_t;
typedef uint64 overlay_t;

constexpr float HUGE_FLOAT = 10000000.0f;

//application
constexpr uint8 MAX_PLAYERS = 4;
constexpr int32 OVERLAY_MARGIN = 130;
constexpr int32 UI_MARGIN = OVERLAY_MARGIN + 50;
constexpr int32 UI_SPACING = 20;
constexpr int32 WINW = 1000;
constexpr int32 WINH = 800;

//entity & game
constexpr id_t INVALID_ENTITY_ID = 0;
constexpr uint8 INPUT_QUEUE_SIZE = 30;
constexpr int32 GAME_TEXT_SIZE = 2;
constexpr int32 LIFE_MARGIN = 100;
constexpr int32 LIFE_SIZE = 20;
constexpr uint32 GAME_START_DELAY = 30;
constexpr uint32 GAME_RESTART_DELAY = 300;

//spaceship
constexpr int32 SPACESHIP_WIDTH = 20;
constexpr int32 SPACESHIP_HEIGHT = 40;
constexpr float SPACESHIP_SPEED = 1.0f;
constexpr float SPACESHIP_Y = WINH - UI_MARGIN - SPACESHIP_HEIGHT;
constexpr uint8 PROJECTILES_PER_PLAYER = 10;
constexpr uint8 PROJECTILES_FOR_ENEMIES = 20;
constexpr uint8 LIVES = 10;
const float SPACESHIP_SHOOT_COOLDOWN = 0.5f;
const float SPACESHIP_HIT_FLASH_TIME = 0.5;

//enemies
constexpr float ENEMY_SPEED = 1.0f;
constexpr uint8 ENEMY_COLUMNS = 6;
constexpr uint8 ENEMY_ROWS = 3;
constexpr uint8 ENEMY_COUNT = ENEMY_COLUMNS * ENEMY_ROWS;
constexpr uint8 ENEMY_SIZE = 40;
constexpr uint8 ENEMY_SPACING = 4;
constexpr float ENEMY_MAX_Y = WINH - UI_MARGIN - SPACESHIP_HEIGHT * 2 - ENEMY_ROWS * ENEMY_SIZE;
const Color ENEMY_COLOR = Color::Red;
const Time ENEMY_DOWN_SWITCH_TIME(1.0f);
const Time ENEMY_SHOOT_COOLDOWN(0.5f);

//projectiles
constexpr float PROJECTILE_SPEED = 15.0f;
constexpr uint8 PROJECTILE_W = 5;
constexpr uint8 PROJECTILE_H = 8;
const Color ALLIED_PROJECTILE_COLOR = Color::Lime;
const Color ENEMY_PROJECTILE_COLOR = Color::Magenta;

//overlay
constexpr float FPS_UPDATES_PER_SECOND = 5.0f;
constexpr int32 OVERLAY_TEXT_SIZE = 1;
constexpr int32 OVERLAY_MESSAGE_TEXT_SIZE = 2;
const Time MESSAGE_DISPLAY_TIME = 5.0f;

//lobby state
constexpr int32 LOBBY_TEXT_SIZE = 5;

//network
constexpr float INPUT_TOLERANCE = 5.0f;
constexpr uint8 SERVER_RESERVE = 30;
constexpr int32 SERVER_FINDER_TEXT_SIZE = 2;
constexpr int32 SERVER_FINDER_BOX_W = 200 * SERVER_FINDER_TEXT_SIZE;
constexpr int32 SERVER_FINDER_BOX_H = 10 * SERVER_FINDER_TEXT_SIZE;
constexpr int32 SERVER_FINDER_TEXT_MARGIN = 2;
constexpr int32 SERVER_TEXT_MARGIN = 150;
constexpr uint32 INPUT_TICK_BUFFER = 3;
constexpr int32 EVENT_ID_BUFFER = 2000;
constexpr uint32 TICKS_PER_SECOND = 60;
constexpr uint32 MAX_INPUTS_SAVED = 20;
constexpr uint32 MAX_INPUTS_SENT = 5;
constexpr int64 INPUT_PRUNING_TICK_DELAY = 5;
constexpr uint8 SERVER_UPDATES_PER_SEND = 3;
constexpr int64 TICK_FLUCTUATION = 2;
const Time UPDATE_RATE = Time(1.0f / TICKS_PER_SECOND);
const Time INTERP_DELAY = Time(0.2f);
const int64 INTERP_DELAY_TICKS = lroundf(INTERP_DELAY.as_seconds() / UPDATE_RATE.as_seconds());
const Time SERVER_TIMEOUT = Time(15.0f);
const Time SERVER_REGISTER_RATE = Time(5.0f);
const Time SERVER_SEND_RATE = Time(1 / 20.0f);
extern network::IPAddress getMasterIP();