#pragma once
#include "AppState.h"

class Lobby final : public AppState
{
	enum class LobbyState {
		NOT_PLAYED, VICTORY, DEFEAT
	} lobbyState = LobbyState::NOT_PLAYED;
public:
	void draw(charlie::Renderer& _rend) override;
	bool update(const charlie::Time& _dt, const charlie::uint32 _tick) override;
	void gameOver(const bool win);
};

