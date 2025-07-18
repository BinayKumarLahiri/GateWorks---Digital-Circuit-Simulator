#pragma once
#include <map>
#include <string>
#include "../Window/Window.hpp"

class GUIHandler {
public:
  std::map<std::string, Window*> windows;
  std::string currentWindow;
  GUIHandler();
  ~GUIHandler();
  void Update();
  void Draw();
  void ChangeWindow(std::string windowName);
  void LoadWindows();
  void Init();
};