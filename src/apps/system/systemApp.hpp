#pragma once
#include "baseApp.hpp"
#include "gui.h" // for xxxTime
namespace Pda {

    class SystemApp : public BaseApp {
        private:
            float f         = 0.0f;
            unsigned long t = 0;
    
        public:
            SystemApp();

            void setup();

            void idleLoop();

            void guiLoop();

            ~SystemApp();

    };
}