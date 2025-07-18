#pragma once
#include <string>
#include "../GUIHandler/GUIHandler.hpp"
class Simulator {
public:
  float windowWidth;
  float windowHeight;
  float fps;
  static bool running;
  GUIHandler guihandler;
  std::string simulatorName;
  Simulator(float windowWidth, float windowHeight, std::string simulatorName, float fps);
  void Update();
  void Draw();
  void Start();
};