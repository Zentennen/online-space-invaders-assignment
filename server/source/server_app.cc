// server_app.cc

#include "server_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include <cmath>

ServerApp::ServerApp()
    : tick(0), players({})
    , lastRegister(Time::now() + 0.2 - SERVER_REGISTER_RATE)
    , game(true)
{
    state = &lobby;
}

bool ServerApp::on_init()
{
   network_.set_send_rate(SERVER_SEND_RATE);
   network_.set_connection_limit(MAX_PLAYERS);
   network_.set_allow_connections(true);
   updatesPerSend = SERVER_UPDATES_PER_SEND;
   INetEventManager::manager = this;
   if (!network_.initialize(network::IPAddress(network::IPAddress::ANY_HOST, 54345))) {
      return false;
   }
   uint8 msg = network::REGISTER;
   if (!network_.socket_.send(masterAddr, &msg, 1)) {
       assert(!"Could not send message");
   }
   network_.add_service_listener(this);
   return true;
}

void ServerApp::on_exit()
{
    for (auto connection : network_.established_connections_) {
        network_.send_connection_disconnect(connection);
    }
}

bool ServerApp::on_tick(const Time &dt)
{
   overlay.onTick(dt);
   ++tick;

   if (startCounter > 0) {
       if (--startCounter == 0) {
           changeState(&game);
           uint8 msg = network::UNREGISTER;
           if (!network_.socket_.send(masterAddr, &msg, 1)) {
               assert(!"Could not send message");
           }
           printf("Unregistered at %s\n", masterAddr.as_string());
       }
   }
   if (gameOverDelayCounter > 0) {
       if (--gameOverDelayCounter == 0) {
           startIfAllPlayersAreConnected();
       }
   }

   state->update(UPDATE_RATE, tick);

   if (Time::now() > lastRegister + SERVER_REGISTER_RATE && state == &lobby) {
       uint8 msg = network::REGISTER;
       if (!network_.socket_.send(masterAddr, &msg, 1)) {
           assert(!"Failed to serialize message");
       }
       printf("Registered at %s\n", masterAddr.as_string());
       lastRegister = Time::now();
   }
   return true;
}

void ServerApp::on_draw()
{
   renderer_.render_text({ WINW - SERVER_TEXT_MARGIN, WINH - UI_MARGIN }, Color::White, GAME_TEXT_SIZE, "SERVER");
   state->draw(renderer_);
   overlay.render(renderer_);
}

void ServerApp::on_timeout(network::Connection *connection)
{
   connection->set_listener(nullptr);
   id_t index = getPlayerIndex(connection) + 1;
   push(network::EventPlayerDisconnected(tick, index));
}

void ServerApp::on_connect(network::Connection *connection)
{
   connection->set_listener(this);
   for (auto&& player : players) {
       if (player.id == 0) {
           player.id = (uint64)connection;
           break;
       }
   }
   if (gameOverDelayCounter == 0) startIfAllPlayersAreConnected();
}

void ServerApp::on_disconnect(network::Connection *connection)
{
    connection->set_listener(nullptr);
    id_t index = getPlayerIndex(connection) + 1;
    push(network::EventPlayerDisconnected(tick, index));
}

void ServerApp::on_acknowledge(network::Connection *connection, 
                               const uint16 sequence, const uint32 bits)
{
    auto pi = getPlayerIndex(connection);
    players[pi].eventQueue.onAcknowledge(sequence, bits);
    uint16 diff = sequence - players[pi].lastSequenceChecked;
    for (uint8 i = 1; i < diff; i++) {
        if (i < 32) {
            uint8 bitsToShift = 31 - i;
            uint32 bitshift = (bits >> bitsToShift) & 1;
            if (bitshift == 1) {
                continue;
            }
        }
        overlay.onPacketLoss();
    }
    players[pi].lastSequenceChecked = sequence;
}

void ServerApp::on_receive(network::Connection *connection, 
                           network::NetworkStreamReader &reader)
{
    if (state != &game) return;
    overlay_t messages = 0;
    while (reader.peek() == network::INPUT && reader.hasData()) {
        network::MessageInput message;
        if (!message.read(reader)) {
            assert(!"Could not serialize message");
        }
        auto index = getPlayerIndex(connection);
        if (index <= MAX_PLAYERS) {
            game.spaceships[index].inputQueue.push(Input(message.data, message.tick));
        }
        else {
            printf("WRN: Input from invalid player\n");
        }
        ++messages;
    }
    overlay.onReceive(reader.length(), messages, connection->round_trip_time());
}

void ServerApp::on_send(network::Connection *connection, 
                        const uint16 sequence, 
                        network::NetworkStreamWriter &writer)
{
   overlay_t messages = 2;
   {
      network::MessageServerTick message(Time::now().as_ticks(), tick);
      if (!message.serialize(writer)) {
         assert(!"Could not serialize message");
      }
   }
   auto pi = getPlayerIndex(connection);
   messages += players[pi].eventQueue.onSend(sequence, writer);
   if (state == &game) {
       for (auto&& spaceship : game.spaceships) {
           if (spaceship.getID() == INVALID_ENTITY_ID) {
               continue;
           }
           if (!spaceship.getMessage().write(writer)) {
               assert(!"Could not serialize message");
           }
           ++messages;
       }
       if (!game.spaceInvaders.getMessage().write(writer)) {
           assert(!"Could not serialize message");
       }
   }
   overlay.onSend(writer.length(), messages);
}

void ServerApp::pushEvent(std::shared_ptr<NetEvent>&& e, const bool replace)
{
    if (e->type == network::GAME_START) {
        startCounter = GAME_START_DELAY;
    }
    else if (e->type == network::GAME_OVER) {
        changeState(&lobby);
        auto cast = (network::EventGameOver*)e.get();
        lobby.gameOver(cast->win);
        gameOverDelayCounter = GAME_RESTART_DELAY;
    }
    else if (e->type == network::PLAYER_DISCONNECTED) {
        auto cast = (network::EventPlayerDisconnected*)e.get();
        disconnectPlayer(cast->id);
    }
    else state->handleEvent(e.get());
    for (uint8 i = 0; i < MAX_PLAYERS; i++) {
        if (e->type == network::GAME_START) {
            auto start = std::make_shared<network::EventGameStart>((network::EventGameStart&)*e.get());
            start->localID = i + 1;
            players[i].eventQueue.push(start, replace);
        }
        else {
            players[i].eventQueue.push(e, replace);
        }
    }
}

uint8 ServerApp::getPlayerIndex(network::Connection* connection)
{
    for (uint8 i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].id == (uint64)connection) return i;
    }
    return MAX_PLAYERS;
}

void ServerApp::startIfAllPlayersAreConnected()
{
    if (state != &lobby) return;
    for (auto&& player : players) {
        if (player.id == 0) return;
    }
    push(network::EventGameStart(tick + GAME_START_DELAY, INVALID_ENTITY_ID));
}

void ServerApp::disconnectPlayer(const id_t playerIndex)
{
    players[playerIndex - 1] = Player();
    game.spaceships[playerIndex - 1].setID(INVALID_ENTITY_ID);
    for (auto&& p : players) {
        if (p.id != 0) return;
    }
    changeState(&lobby);
}
