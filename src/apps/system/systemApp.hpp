#pragma once
#if 0
#include "baseApp.hpp"
#include "gui.h" // for xxxTime
#include "lua.hpp"
namespace Pda {

    class SystemApp : public BaseApp {
        private:
            float f         = 0.0f;
            unsigned long t = 0;
            lua_State *L;
    
        public:
            SystemApp();

            void setup();

            void idleLoop();

            void guiLoop();

            ~SystemApp();

    };
}
#endif