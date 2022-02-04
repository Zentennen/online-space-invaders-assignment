// charlie_application.cc
// code in this file was mostly written by me

#include "charlie_application.hpp"
#include "Entity.h"

namespace charlie {
   Application::Application() 
       : masterAddr(getMasterIP()), state(nullptr)
   {

   }

   Application::~Application()
   {
   }

   bool Application::init()
   {
      if (!window_.open(Settings("charlie", WINW, WINH))) {
         return false;
      }

      if (!renderer_.initialize(window_)) {
         return false;
      }
      state->enter();
      return on_init();
   }

   void Application::run()
   {
      bool running = true;
      while (running) {
         if (!window_.update()) {
            running = false;
         }
         network_.update();
         const auto dt = Time::deltatime();
         accumulator += dt;

         if (accumulator >= updateRate) {
             accumulator -= updateRate;
             Time now = Time::now();
             Time delta = now - lastUpdateTime;
             if (!on_tick(delta)) {
                 running = false;
             }
             lastUpdateTime = now;
         }

         renderer_.clear(Color::Black);
         on_draw();
         window_.present();
      }
   }

   void Application::exit()
   {
      on_exit();

      network_.shutdown();
      renderer_.shutdown();
      window_.close();
   }

   bool Application::on_init()
   {
      return true;
   }

   void Application::on_exit()
   {
   }

   bool Application::on_tick(const Time &dt)
   {
      return true;
   }

   void Application::on_draw()
   {
   }
   void Application::changeState(AppState* _state)
   {
       state->exit();
       _state->enter();
       state = _state;
   }
} // !charlie
