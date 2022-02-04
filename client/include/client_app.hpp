// client_app.hpp

#ifndef CLIENT_APP_HPP_INCLUDED
#define CLIENT_APP_HPP_INCLUDED

#include <charlie_application.hpp>
#include "Game.h"
#include "ServerFinder.h"
#include "Lobby.h"
#include "StaticQueue.h"
#include "Inputinator.h"

using namespace charlie;

struct ClientApp final : Application, network::IConnectionListener {
   ClientApp();

   // note: Application
   virtual bool on_init();
   virtual void on_exit();
   virtual bool on_tick(const Time &dt);
   virtual void on_draw();

   // note: IConnectionListener 
   virtual void on_acknowledge(network::Connection *connection, const uint16 sequence, const uint32 bits);
   virtual void on_receive(network::Connection *connection, network::NetworkStreamReader &reader);
   virtual void on_send(network::Connection *connection, const uint16 sequence, network::NetworkStreamWriter &writer);
private:
   Mouse &mouse_;
   Keyboard &keyboard_;
   network::Connection connection_;
   Time serverTime;
   uint32 serverTick = 0;
   int64 tickOffset = 0;
   uint32 tick = 0;
   uint16 lastSequenceChecked = 0;
   Inputinator inputinator;
   EventQueue events;
   StaticArray<id_t, EVENT_ID_BUFFER> eventAck;
   uint16 oldestEventIndex = 0;
   uint16 nextEventIndex = 0;
   Lobby lobby;
   Game game;
   ServerFinder finder;

   uint32 toServerInput(const uint32 _tick);
   uint32 toServer(const uint32 _tick);
   uint32 toClient(const uint32 _tick);
   bool handleEventID(id_t eventID);
   void onDisconnect();
};

#endif // !CLIENT_APP_HPP_INCLUDED
