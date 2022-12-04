#pragma once

#include "common.h"

namespace Pda {
    class BaseApp {
    public:
        BaseApp();

        virtual void setup() = 0;

        virtual void idleLoop() = 0;

        virtual void guiLoop() = 0;
        
        ~BaseApp();
    };
}