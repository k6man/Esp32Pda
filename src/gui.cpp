//#include <BBQ10Keyboard.h>

#include "gui.h"
#include "minikeyboard.h"
#include "touch.h"
#include "lua.hpp"

//#include "./apps/system/systemApp.hpp"

//Pda::SystemApp SYSTEMAPP;

unsigned long imguiDrawTime;
unsigned long imguiRenderTime;
unsigned long imguiRasterTime;
unsigned long imguiLastTime{0};


#ifdef MAKERFABS_PARALLEL_TFT // a déplacer dans un board.c ??
LGFX lcd;

//texture_color16_t screen;
texture_color24_t screen;
//texture_value8_t screen;
#endif

#ifdef SHARPMEMORYDISPLAY

texture_value8_t screen;

// Set the size of the display here, e.g. 144x168!
//Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 144, 168);
Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, screenWidth, screenHeight);
// The currently-available SHARP Memory Display (144x168 pixels)
// requires > 4K of microcontroller RAM; it WILL NOT WORK on Arduino Uno
// or other <4K "classic" devices!  The original display (96x96 pixels)
// does work there, but is no longer produced.

#endif

#ifdef UM_FEATHER_S3
UMS3 ums3;
#endif

texture_alpha8_t fontAtlas;

ImGuiContext *context;

extern lua_State* lState;
extern void LoadImguiBindings(void) ;


void imGuiSetup( void ){

  lcdSetup();

  context = ImGui::CreateContext();

  ImGui_ImplSoftraster_Init(&screen);

  ImGuiStyle &style      = ImGui::GetStyle();
  style.AntiAliasedLines = false;
  style.AntiAliasedFill  = false;
  style.WindowRounding   = 0.0f;

#ifdef IMGUIMONOCHROME
  imguiMonochromeTheme();
#endif

  ImGuiIO &io = ImGui::GetIO();

  /* initilise  physical Keyboard */
  minikeyboardSetup(io);

  io.Fonts->Flags =
    ImFontAtlasFlags_NoPowerOfTwoHeight /* | ImFontAtlasFlags_NoMouseCursors */;

  uint8_t *pixels;
  int width, height;
  io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
  fontAtlas.init(width, height, (alpha8_t *)pixels);
  io.Fonts->TexID = &fontAtlas;

  screen.init(screenWidth, screenHeight);

  io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
  io.MouseDown[0] = false; // left
  io.MouseDown[1] = false; // right
  io.MouseDown[2] = false; // middle + extras
  io.MouseDown[3] = false; // 
  io.MouseDown[4] = false; // 


  lState = luaL_newstate();
  luaL_openlibs(lState);
  LoadImguiBindings();  

}


void imGuiLoop()
{

  ImGuiIO &io  = ImGui::GetIO();
  unsigned long current_time = millis();
  io.DeltaTime =  (static_cast<float>(current_time - imguiLastTime)) / 1000;
  imguiLastTime = current_time;
  minikeyboardLoop(io);
  lcdLoop(io);

  /* [0.0f - 1.0f] */
  io.NavInputs[ImGuiNavInput_Activate] =
    0.0f; // activate / open / toggle / tweak value       // e.g. Circle (PS4),
          // A (Xbox), B (Switch), Space (Keyboard)
  io.NavInputs[ImGuiNavInput_Cancel] =
    0.0f; // cancel / close / exit                        // e.g. Cross (PS4),
          // B (Xbox), A (Switch), Escape (Keyboard)
  io.NavInputs[ImGuiNavInput_Input] =
    0.0f; // text input / on-screen keyboard              // e.g. Triang.(PS4),
          // Y (Xbox), X (Switch), Return (Keyboard)
  io.NavInputs[ImGuiNavInput_Menu] =
    0.0f; // tap: toggle menu / hold: focus, move, resize // e.g. Square (PS4),
          // X (Xbox), Y (Switch), Alt (Keyboard)
  io.NavInputs[ImGuiNavInput_DpadLeft] =
    0.0f; // move / tweak / resize window (w/ PadMenu)    // e.g. D-pad
          // Left/Right/Up/Down (Gamepads), Arrow keys (Keyboard)
  io.NavInputs[ImGuiNavInput_DpadRight] = 0.0f;
  io.NavInputs[ImGuiNavInput_DpadUp]    = 0.0f;
  io.NavInputs[ImGuiNavInput_DpadDown]  = 0.0f;
  io.NavInputs[ImGuiNavInput_TweakSlow] =
    0.0f; // slower tweaks                                // e.g. L1 or L2
          // (PS4), LB or LT (Xbox), L or ZL (Switch)
  io.NavInputs[ImGuiNavInput_TweakFast] =
    0.0f; // faster tweaks                                // e.g. R1 or R2
          // (PS4), RB or RT (Xbox), R or ZL (Switch)

  ImGui_ImplSoftraster_NewFrame();
  ImGui::NewFrame();

  // here call current App gui.
  //SYSTEMAPP.guiLoop();
  ImGuiWindowFlags window_flags = 0;
  window_flags |= ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoResize;  
  ImGui::Begin("Launcher", nullptr, window_flags);
  ImGuiStyle& style = ImGui::GetStyle();
  style.TouchExtraPadding = ImVec2(5.0, 5.0);
  ImGui::SetWindowPos(ImVec2(0.0, 0.0));
  // ImGui::SetWindowSize(ImVec2(SCREENX, SCREENY));
  ImGui::SetWindowSize(ImVec2(screenWidth, screenHeight));

  int ret = luaL_loadstring (lState, "imgui.Text([[ hello I m lua]]);");
  lua_call (lState, 0, 0);
  
  ImGui::End(); // launcher

  imguiRenderTime = millis();
  ImGui::Render();
  imguiRenderTime = millis() - imguiRenderTime;

  imguiRasterTime = millis();
  #ifdef SHARPMEMORYDISPLAY
  display.clearDisplayBuffer();
  #endif
  ImGui_ImplSoftraster_RenderDrawData(ImGui::GetDrawData());
  imguiRasterTime = millis() - imguiRasterTime;

  imguiDrawTime = millis();
  screen_draw();
  imguiDrawTime = millis() - imguiDrawTime;
}
