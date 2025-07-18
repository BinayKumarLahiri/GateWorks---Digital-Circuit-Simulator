// #define RAYGUI_IMPLEMENTATION
#include <iostream>
#include <raylib.h>
#include <raygui.h>
#include <string>
#include "Simulator.hpp"
#include "../FileDialog/Dialog.hpp"

bool Simulator::running = true;
Simulator::Simulator(float windowWidth, float windowHeight, std::string simulatorName, float fps) {
  this->windowWidth = windowWidth;
  this->windowHeight = windowHeight;
  this->simulatorName = simulatorName;
  this->fps = fps;
}
void Simulator::Start() {
  InitWindow(this->windowWidth, this->windowHeight, this->simulatorName.c_str());
  SetTargetFPS(this->fps);
  GuiLoadStyle("../asset/style_terminal.rgs");
  this->guihandler.Init();
  while (!WindowShouldClose() && Simulator::running)
  {
    this->Update();
    BeginDrawing();
    ClearBackground(WHITE);
    this->Draw();
    // DrawFPS(800, 10);
    EndDrawing();
  }
  CloseWindow();
}
void Simulator::Update() {
  // Do something related to updation
  this->guihandler.Update();
}
void Simulator::Draw() {
  this->guihandler.Draw();
}