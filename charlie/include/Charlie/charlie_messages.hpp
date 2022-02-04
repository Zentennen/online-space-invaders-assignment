// charlie_messages.hpp

#ifndef CHARLIE_MESSAGES_HPP_INCLUDED
#define CHARLIE_MESSAGES_HPP_INCLUDED
#include "charlie_network.hpp"
#include "Input.h"
#include "Event.h"

#define READWRITE bool read(network::NetworkStreamReader& reader) { return serialize(reader); } bool write(network::NetworkStreamWriter& writer) { return serialize(writer); }

namespace charlie {
   namespace network {
      enum MessageType {
         TICK,
         REGISTER,
         UNREGISTER,
         GET_SERVERS,
         SEND_SERVERS,
         INPUT,
         SPACESHIP,
         ENEMIES,
         PROJECTILE_SPAWNED,
         ENEMY_HIT,
         SHIELD_HIT,
         SPACESHIP_HIT,
         GAME_START,
         GAME_OVER,
         PLAYER_DISCONNECTED,
         COUNT
      };

      static_assert(COUNT <= 255, "network message type cannot exceed 255!");

      struct MessageServerTick {
         MessageServerTick();
         explicit MessageServerTick(const int64  server_time, const uint32 server_tick);
         READWRITE
         template <typename Stream>
         bool serialize(Stream &stream)
         {
            bool result = true;
            result &= stream.serialize(type);
            result &= stream.serialize(serverTime);
            result &= stream.serialize(serverTick);
            return result;
         }

         uint8 type = TICK;
         int64 serverTime = 0;
         uint32 serverTick = 0;
      };

      struct MessageSendServers {
          MessageSendServers();
          MessageSendServers(const DynamicArray<IPAddress>& _servers);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(numServers);
              servers.resize(numServers);
              for (uint8 i = 0; i < numServers; i++) {
                  result &= stream.serialize(servers[i].host_);
                  result &= stream.serialize(servers[i].port_);
              }
              return result;
          }
          uint8 type = SEND_SERVERS;
          uint16 numServers = 0;
          DynamicArray<IPAddress> servers;
      };

      struct MessageInput {
          MessageInput();
          explicit MessageInput(const Input& _input);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(tick);
              result &= stream.serialize(data);
              return result;
          }
          uint8 type = INPUT;
          uint32 tick = 0;
          uint8 data = 0;
      };

      struct MessageSpaceship {
          MessageSpaceship();
          explicit MessageSpaceship(const id_t _id, const uint16 _x);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(id);
              result &= stream.serialize(x);
              return result;
          }
          uint8 type = SPACESHIP;
          id_t id = INVALID_ENTITY_ID;
          uint16 x = 0;
      };

      struct MessageEnemies {
          MessageEnemies();
          explicit MessageEnemies(const Vector2& _vec);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(x);
              result &= stream.serialize(y);
              return result;
          }
          uint8 type = ENEMIES;
          uint16 x = 0;
          uint16 y = 0;
      };

      struct EventProjectileSpawned final : public NetEvent {
          EventProjectileSpawned();
          explicit EventProjectileSpawned(const uint32 _tick, const Vector2& _vec, const bool _allied);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(eventID);
              result &= stream.serialize(tick);
              result &= stream.serialize(id);
              result &= stream.serialize(data);
              return result;
          }
          id_t id = INVALID_ENTITY_ID;
          uint32 data = 0;
          void translate(Vector2& _vec, bool& _allied);
      };

      struct EventEnemyHit final : public NetEvent {
          EventEnemyHit();
          explicit EventEnemyHit(const uint32 _tick, const StaticArray<StaticArray<bool, ENEMY_ROWS>, ENEMY_COLUMNS>& _states, const id_t _projectileID);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(eventID);
              result &= stream.serialize(tick);
              result &= stream.serialize(enemyBitField);
              result &= stream.serialize(projectileID);
              return result;
          }
          uint64 enemyBitField = 0;
          id_t projectileID = INVALID_ENTITY_ID;
      };

      struct EventSpaceshipHit final : public NetEvent {
          EventSpaceshipHit();
          explicit EventSpaceshipHit(const uint32 _tick, const id_t _id, const id_t _projectileID);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(eventID);
              result &= stream.serialize(tick);
              result &= stream.serialize(id);
              result &= stream.serialize(lives);
              result &= stream.serialize(projectileID);
              return result;
          }
          id_t id = INVALID_ENTITY_ID;
          id_t projectileID = INVALID_ENTITY_ID;
          uint8 lives = LIVES;
      };

      //struct EventShieldHit final : public NetEvent {
      //    EventShieldHit();
      //    explicit EventShieldHit(const uint32 _tick, const id_t _id, const uint8 _hp);
      //    READWRITE
      //    template <typename Stream>
      //    bool serialize(Stream& stream)
      //    {
      //        bool result = true;
      //        result &= stream.serialize(type);
      //        result &= stream.serialize(eventID);
      //        result &= stream.serialize(tick);
      //        result &= stream.serialize(hp);
      //        result &= stream.serialize(id);
      //        return result;
      //    }
      //    id_t id;
      //    uint8 hp;
      //};

      struct EventGameStart final : public NetEvent {
          EventGameStart();
          explicit EventGameStart(const uint32 _tick, const id_t _id);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(eventID);
              result &= stream.serialize(tick);
              result &= stream.serialize(localID);
              return result;
          }
          id_t localID = INVALID_ENTITY_ID;
      };

      struct EventGameOver final : public NetEvent {
          EventGameOver();
          explicit EventGameOver(const uint32 _tick, const bool _win);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(eventID);
              result &= stream.serialize(tick);
              result &= stream.serialize(win);
              return result;
          }
          bool win = false;
      };

      struct EventPlayerDisconnected final : public NetEvent {
          EventPlayerDisconnected();
          explicit EventPlayerDisconnected(const uint32 _tick, const id_t _id);
          READWRITE
          template <typename Stream>
          bool serialize(Stream& stream)
          {
              bool result = true;
              result &= stream.serialize(type);
              result &= stream.serialize(eventID);
              result &= stream.serialize(tick);
              result &= stream.serialize(id);
              return result;
          }
          id_t id = 0;
      };
   } // !network
} // !charlie

#endif // !CHARLIE_MESSAGES_HPP_INCLUDED
