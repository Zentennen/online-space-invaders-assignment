#include "Lobby.h"
#include "Config.h"

void Lobby::draw(Renderer& _rend)
{
	switch (lobbyState) {
	case LobbyState::NOT_PLAYED: {
		_rend.render_text(Point(0, 0), Color::White, LOBBY_TEXT_SIZE, "Waiting for players...");
	} break;
	case LobbyState::VICTORY: {
		_rend.render_text(Point(0, 0), Color::White, LOBBY_TEXT_SIZE, "Victory!");
	} break;
	case LobbyState::DEFEAT: {
		_rend.render_text(Point(0, 0), Color::White, LOBBY_TEXT_SIZE, "Defeat!");
	} break;
	}
}

bool Lobby::update(const Time& _dt, const uint32 _tick)
{
	return true;
}

void Lobby::gameOver(const bool win)
{
	if (win) lobbyState = LobbyState::VICTORY;
	else lobbyState = LobbyState::DEFEAT;
}
