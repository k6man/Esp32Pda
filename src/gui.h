#pragma once

#include "common.h"

extern unsigned long imguiDrawTime;
extern unsigned long imguiRenderTime;
extern unsigned long imguiRasterTime;
extern unsigned long imguiLastTime;

void imGuiSetup( void );
void imGuiLoop( void );
