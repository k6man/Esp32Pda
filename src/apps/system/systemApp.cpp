#include "systemApp.hpp"

Pda::SystemApp::SystemApp() {

}

void Pda::SystemApp::setup() {

}

void Pda::SystemApp::idleLoop() {

}

void Pda::SystemApp::guiLoop() {

  ImGuiWindowFlags window_flags = 0;
  window_flags |= ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoResize;
  ImGui::Begin("Launcher", nullptr, window_flags);

  ImGuiStyle& style = ImGui::GetStyle();
  style.TouchExtraPadding = ImVec2(5.0, 5.0);
  ImGui::SetWindowPos(ImVec2(0.0, 0.0));
  // ImGui::SetWindowSize(ImVec2(SCREENX, SCREENY));
  ImGui::SetWindowSize(ImVec2(screenWidth, screenHeight));

  f += 0.05;
  if (f > 1.0f) f = 0.0f;

  unsigned int deltaTime = millis() - t;
  t += deltaTime;

  deltaTime -= (imguiDrawTime + imguiRenderTime + imguiRasterTime);

  ImGui::Text("Frame rate: %f x:%f y:%f", ImGui::GetIO().Framerate, ImGui::GetIO().MousePos[0], ImGui::GetIO().MousePos[1]);
  ImGui::Text("Render time %d ms", imguiRenderTime);
  ImGui::Text("Raster time %d ms", imguiRasterTime);
  ImGui::Text("Hardware write time %d ms", imguiDrawTime);
  ImGui::Text("Remaining time %d ms", deltaTime);
  ImGui::SliderFloat("SliderFloat", &f, 0.0f, 1.0f);

  // ImGui::Text("mouse x:%f y:%f ", io.MousePos[0], io.MousePos[1] );
  // ImGui::Text("io.MouseDown[0]:%s left", io.MouseDown[0] == true?"true":"false");
  // ImGui::Text("io.MouseDown[1]:%s right", io.MouseDown[1] == true?"true":"false");
  // ImGui::Text("io.MouseDown[2]:%s center", io.MouseDown[2] == true?"true":"false");
        static int counter = 0;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
        ImGui::PopButtonRepeat();
        ImGui::SameLine();
        ImGui::Text("%d", counter);

  static char str0[128] = "Hello, world!";
  ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));

  static int clicked = 0;
  if (ImGui::Button("Button"))
      clicked++;
  // if (clicked & 1)
  // {
      ImGui::SameLine();
      ImGui::Text(" %u ",clicked);
  // }

  static bool check = true;
  ImGui::Checkbox("checkbox", &check);ImGui::SameLine();

  static int e = 0;
  ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
  ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
  ImGui::RadioButton("radio c", &e, 2);



}

Pda::SystemApp::~SystemApp() {

}
