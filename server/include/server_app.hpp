// server_app.hpp

#ifndef SERVER_APP_HPP_INCLUDED
#define SERVER_APP_HPP_INCLUDED
#pragma once
#include "Game.h"
#include "Lobby.h"
#include <charlie_application.hpp>

using namespace charlie;

struct ServerApp final : Application, network::IServiceListener, network::IConnectionListener, public INetEventManager {
   ServerApp();

   // note: Application
   virtual bool on_init();
   virtual void on_exit();
   virtual bool on_tick(const Time &dt);
   virtual void on_draw();

   // note: IServiceListener
   virtual void on_timeout(network::Connection* connection);
   virtual void on_connect(network::Connection* connection);
   virtual void on_disconnect(network::Connection* connection);

   // note: IConnectionListener 
   virtual void on_acknowledge(network::Connection* connection, const uint16 sequence, const uint32 bits);
   virtual void on_receive(network::Connection* connection, network::NetworkStreamReader& reader);
   virtual void on_send(network::Connection* connection, const uint16 sequence, network::NetworkStreamWriter& writer);

   // note: IEventManager
   void pushEvent(std::shared_ptr<NetEvent>&& e, const bool replace) override;
private:
	uint8 getPlayerIndex(network::Connection* connection);
	void startIfAllPlayersAreConnected();
	Time lastRegister;
	uint32 tick = 0;
	uint32 startCounter = 0;
	uint32 gameOverDelayCounter = 0;
	Game game;
	Lobby lobby;
	StaticArray<Player, MAX_PLAYERS> players;

	void disconnectPlayer(const id_t playerIndex);
};

#endif // !SERVER_APP_HPP_INCLUDED
