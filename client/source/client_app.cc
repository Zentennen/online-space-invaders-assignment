// client_app.cc

#include "client_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include <math.h>

const Time tickRate = Time(1.0 / 60.0);

ClientApp::ClientApp()
    : mouse_(window_.mouse_)
    , keyboard_(window_.keyboard_)
    , finder(connection_)
    , game(false)
{
    state = &finder;
    eventAck.fill(0);
}

bool ClientApp::on_init()
{
    network_.set_send_rate(UPDATE_RATE);
   if (!network_.initialize({})) {
      return false;
   }
   connection_.set_listener(this);
   return true;
}

void ClientApp::on_exit()
{
    network_.send_connection_disconnect(&connection_);
}

bool ClientApp::on_tick(const Time &dt)
{
   overlay.onTick(dt);
   ++tick;
   if (state == &game && (connection_.is_disconnected() || connection_.is_disconnecting() || !connection_.is_valid())) {
       onDisconnect();
   }
   uint32 currentTick;
   int64 delta = tickOffset + tick;
   if (delta > 0) currentTick = static_cast<uint32>(delta);
   else currentTick = 0;
   state->update(dt, currentTick);

   DynamicArray<std::shared_ptr<NetEvent>> currentEvents;
   int64 eventTick = toServer(tick) - INTERP_DELAY_TICKS;
   events.transferEvents(currentEvents, static_cast<uint32>(eventTick));
   for (auto&& e : currentEvents) {
       switch (e->type) {
       case network::GAME_START: {
           if (state == &game) {
               break;
           }
           auto cast = (network::EventGameStart*)e.get();
           changeState(&game);
           game.localShipID = cast->localID;
           game.initializeInterpolators(cast->tick);
           printf("Game started!\n");
       } break;
       case network::GAME_OVER: {
           if (state != &game) {
               break;
           }
           auto cast = (network::EventGameOver*)e.get();
           changeState(&lobby);
           lobby.gameOver(cast->win);
           printf("Game ended!\n");
       } break;
       case network::PLAYER_DISCONNECTED: {
           auto cast = (network::EventPlayerDisconnected*)e.get();
           auto ship = game.getSpaceship(cast->id);
           ship->setID(INVALID_ENTITY_ID);
       }
       default:
           game.processEvent(e.get());
           break;
       }
   }

   Input input;
   input.tick = tick;
   if (keyboard_.pressed(Keyboard::Key::Escape)) {
       return false;
   }
   if (keyboard_.down(Keyboard::Key::D)) {
       state->onKeyDown(Keyboard::Key::D);
       input.right = true;
   }
   if (keyboard_.down(Keyboard::Key::A)) {
       state->onKeyDown(Keyboard::Key::A);
       input.left = true;
   }
   if (keyboard_.down(Keyboard::Key::Space)) {
       state->onKeyDown(Keyboard::Key::Space);
   }
   if (keyboard_.down(Keyboard::Key::W)) {
       state->onKeyDown(Keyboard::Key::W);
   }
   if (keyboard_.down(Keyboard::Key::S)) {
       state->onKeyDown(Keyboard::Key::S);
       input.shoot = true;
   }
   if (keyboard_.pressed(Keyboard::Key::D)) {
       state->onPress(Keyboard::Key::D);
   }
   if (keyboard_.pressed(Keyboard::Key::A)) {
       state->onPress(Keyboard::Key::A);
   }
   if (keyboard_.pressed(Keyboard::Key::Space)) {
       state->onPress(Keyboard::Key::Space);
   }
   if (keyboard_.pressed(Keyboard::Key::W)) {
       state->onPress(Keyboard::Key::W);
   }
   if (keyboard_.pressed(Keyboard::Key::S)) {
       state->onPress(Keyboard::Key::S);
   }
   if (game.localShipID != INVALID_ENTITY_ID && state == &game) inputinator.recordInput(input, game.spaceships[game.localShipID - 1].position);
   return true;
}

void ClientApp::on_draw()
{
    state->draw(renderer_);
    overlay.render(renderer_);
}

void ClientApp::on_acknowledge(network::Connection *connection, 
                               const uint16 sequence, const uint32 bits)
{
    uint16 diff = sequence - lastSequenceChecked;
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
    lastSequenceChecked = sequence;
}

#define SERIALIZE if(!message.read(reader)) return
#define HANDLE_RECEIVED_EVENT(type) type message; SERIALIZE; if (handleEventID(message.eventID)) { \
events.push(std::make_shared<type>(message)); \
}

void ClientApp::on_receive(network::Connection *connection, 
                           network::NetworkStreamReader &reader)
{
   if (state == &finder) changeState(&lobby);
   {
      network::MessageServerTick message;
      if (!message.serialize(reader)) {
         assert(!"could not read message!");
      }

      int64 newOffset = (int64)message.serverTick - (int64)tick;
      if (abs(newOffset - tickOffset) > 2) {
          tickOffset = newOffset;
      }
      //if (offsetDelta < minOffsetDelta) {
      //    minOffsetDelta = offsetDelta;
      //    offsetRange = maxOffsetDelta - minOffsetDelta;
      //    tickOffset = newOffset;
      //}
      //if (offsetDelta > maxOffsetDelta) {
      //    maxOffsetDelta = offsetDelta;
      //    offsetRange = maxOffsetDelta - minOffsetDelta;
      //    tickOffset = newOffset;
      //}
      serverTime = message.serverTime;
      serverTick = message.serverTick;
   } 

   overlay_t messages = 1;

   while (reader.hasData()) {
       ++messages;
       switch (reader.peek()) {
       case network::ENEMIES: {
           network::MessageEnemies message;
           SERIALIZE;
           game.spaceInvaders.interpolator.push(Vector2(message.x, message.y), serverTick);
       } break;
       case network::SPACESHIP: {
           network::MessageSpaceship message;
           SERIALIZE;
           if (message.id == game.localShipID) {
               game.spaceships[game.localShipID - 1].correctPrediction({ static_cast<float>(message.x), SPACESHIP_Y }, toClient(serverTick) - INPUT_TICK_BUFFER, inputinator);
           }
           else {
               auto ship = game.getSpaceship(message.id);
               if (ship) {
                   ship->interpolator.push({ static_cast<float>(message.x), SPACESHIP_Y }, serverTick);
               }
           }
       } break;
       case network::PROJECTILE_SPAWNED: {
           HANDLE_RECEIVED_EVENT(network::EventProjectileSpawned);
       } break;
       case network::ENEMY_HIT: {
           HANDLE_RECEIVED_EVENT(network::EventEnemyHit);
       } break;
       case network::SPACESHIP_HIT: {
           HANDLE_RECEIVED_EVENT(network::EventSpaceshipHit);
       } break;
       case network::GAME_START: {
           HANDLE_RECEIVED_EVENT(network::EventGameStart);
       } break;
       case network::GAME_OVER: {
           HANDLE_RECEIVED_EVENT(network::EventGameOver);
       } break;
       case network::PLAYER_DISCONNECTED: {
           HANDLE_RECEIVED_EVENT(network::EventPlayerDisconnected);
       } break;
       default: 
           assert(!"Invalid message header");
           break;
       }
   }

   //inputinator.pruneQueue(toClient(serverTick));
   overlay.onReceive(reader.length(), messages, connection_.round_trip_time());
   return;
}

void ClientApp::on_send(network::Connection *connection, 
                        const uint16 sequence, 
                        network::NetworkStreamWriter &writer)
{
    overlay_t messages = 0;
    auto inputs = inputinator.getInputs();
    for(uint8 i = 0; i < inputs.size() && i < MAX_INPUTS_SENT; i++) {
        network::MessageInput inputMessage(inputs[i]);
        inputMessage.tick = toServerInput(inputMessage.tick);
        if (!inputMessage.write(writer)) {
            assert(!"Failed to serialize mssage");
        }
        ++messages;
    }
    overlay.onSend(writer.length(), messages);
}

uint32 ClientApp::toServerInput(const uint32 _tick)
{
    const float tickLatency = connection_.round_trip_time().as_seconds() / UPDATE_RATE.as_seconds();
    const uint32 ticksPerRoundTrip = lroundf(tickLatency);
    int64 answer = tickOffset + static_cast<int64>(_tick )+ static_cast<int64>(ticksPerRoundTrip) + static_cast<int64>(INPUT_TICK_BUFFER);
    return static_cast<uint32>(answer);
}

uint32 ClientApp::toServer(const uint32 _tick)
{
    int64 answer = tickOffset + static_cast<int64>(_tick);
    return static_cast<uint32>(answer);
}

uint32 ClientApp::toClient(const uint32 _tick)
{
    const Time tickLatency = connection_.latency() / UPDATE_RATE;
    const uint32 ticksPerTrip = static_cast<uint32>(tickLatency.as_ticks());
    int64 answer = static_cast<int64>(_tick) - tickOffset - static_cast<int64>(ticksPerTrip);
    return static_cast<uint32>(answer);
}

bool ClientApp::handleEventID(id_t eventID)
{
    static bool limitReached = false;
    size_t size = eventAck.size();
    int32 diff = nextEventIndex - oldestEventIndex;
    int32 loopEnd = posMod(diff, static_cast<int32>(size));

    for (id_t i = 0; i < loopEnd; i++) {
        auto index = posMod(i + oldestEventIndex, static_cast<int32>(size));
        if (eventAck[index] == eventID) {
            return false;
        }
    }
    
    eventAck[nextEventIndex] = eventID;
    ++nextEventIndex;
    if (nextEventIndex >= size) {
        limitReached = true;
        nextEventIndex -= static_cast<uint16>(size);
    }
    if (limitReached) ++oldestEventIndex;
    return true;
}

void ClientApp::onDisconnect()
{
    changeState(&finder);
    overlay.displayMessage("Disconnected");
}
