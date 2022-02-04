// charlie_messages.cc

#include "charlie_messages.hpp"
#include "charlie_network.hpp"

namespace charlie {
   namespace network {
      MessageServerTick::MessageServerTick()
         : type(TICK)
         , serverTime(0)
         , serverTick(0)
      {
      }

      MessageServerTick::MessageServerTick(const int64  server_time, const uint32 server_tick)
         : type(TICK)
         , serverTime(server_time)
         , serverTick(server_tick)
      {
      }

      MessageSpaceship::MessageSpaceship()
      {
      }

      MessageSpaceship::MessageSpaceship(const id_t _id, const uint16 _x) : 
          id(_id), x(_x)
      {
          
      }

      MessageEnemies::MessageEnemies()
      {
      }

      MessageEnemies::MessageEnemies(const Vector2& _vec) :
          x(static_cast<uint16>(_vec.x_)), y(static_cast<uint16>(_vec.y_))
      {
      }

      MessageInput::MessageInput()
      {
      }

      MessageInput::MessageInput(const Input& _input) : 
          data(_input.pack()), tick(_input.tick)
      {
      }

      EventProjectileSpawned::EventProjectileSpawned() : 
          NetEvent(PROJECTILE_SPAWNED)
      {
      }

      EventProjectileSpawned::EventProjectileSpawned(const uint32 _tick, const Vector2& _vec, const bool _allied) :
          NetEvent(PROJECTILE_SPAWNED, _tick)
      {
          if (_allied) {
              data = ((static_cast<uint32>(_vec.y_)) | 0x8000) << 16;
          }
          else {
              data = ((static_cast<uint32>(_vec.y_)) & 0x7fff) << 16;
          }
          data |= static_cast<uint32>(_vec.x_) & 0xffff;
      }

      void EventProjectileSpawned::translate(Vector2& _vec, bool& _allied)
      {
          _vec.x_ = static_cast<float>(data & 0xffff);
          _vec.y_ = static_cast<float>((data >> 16) & 0x7fff);
          _allied = (data >> 31) == 1;
      }

      EventEnemyHit::EventEnemyHit() : 
          NetEvent(ENEMY_HIT)
      {
      }

      EventEnemyHit::EventEnemyHit(const uint32 _tick, const StaticArray<StaticArray<bool, ENEMY_ROWS>, ENEMY_COLUMNS>& _states, const id_t _projectileID)
          : NetEvent(ENEMY_HIT, _tick), projectileID(_projectileID)
      {
          for (uint64 i = 0; i < _states.size(); i++) {
              for (uint64 j = 0; j < _states[i].size(); j++) {
                  if (_states[i][j]) {
                      enemyBitField |= (uint64)1 << (i * ENEMY_ROWS + j);
                  }
              }
          }
      }

      EventSpaceshipHit::EventSpaceshipHit() : 
          NetEvent(SPACESHIP_HIT)
      {

      }
      EventSpaceshipHit::EventSpaceshipHit(const uint32 _tick, const id_t _id, const id_t _projectileID) :
          NetEvent(SPACESHIP_HIT, _tick), id(_id), projectileID(_projectileID)
      {
      }
      //EventShieldHit::EventShieldHit() : 
      //    NetEvent(SHIELD_HIT), hp(0), id(0)
      //{
      //}
      //EventShieldHit::EventShieldHit(const uint32 _tick, const id_t _id, const uint8 _hp) :
      //    NetEvent(SHIELD_HIT, _tick), id(_id), hp(_hp)
      //{
      //}
      EventGameStart::EventGameStart() : 
          NetEvent(GAME_START)
      {
      }
      EventGameStart::EventGameStart(const uint32 _tick, const id_t _id) 
          : NetEvent(GAME_START, _tick)
          , localID(_id)
      {
      }
      EventGameOver::EventGameOver() : 
          NetEvent(GAME_OVER)
      {
      }
      EventGameOver::EventGameOver(const uint32 _tick, const bool _win) :
          NetEvent(GAME_OVER, _tick), win(_win)
      {
      }
      MessageSendServers::MessageSendServers()
      {
          
      }
      MessageSendServers::MessageSendServers(const DynamicArray<IPAddress>& _servers) 
          : servers(_servers)
          , numServers(static_cast<uint16>(_servers.size()))
      {
      }
      EventPlayerDisconnected::EventPlayerDisconnected() : 
          NetEvent(PLAYER_DISCONNECTED)
      {
      }
      EventPlayerDisconnected::EventPlayerDisconnected(const uint32 _tick, const id_t _id) : 
          NetEvent(PLAYER_DISCONNECTED, _tick), id(_id)
      {
      }
} // !network
} // !messages
