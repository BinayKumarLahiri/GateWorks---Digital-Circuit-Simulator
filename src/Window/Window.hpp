#pragma once
#include <map>
#include <string>
#include <functional>
#include <array>
#include <raylib.h>
#include "../Board/Board.hpp"

typedef struct Data {
  std::string fileName;
}Data;
class Window {
public:
  std::string windowName;
  std::map<std::string, std::function<void(Data data)>> eventHandler;
  Window(std::string windowName);
  ~Window();
  virtual void Update() = 0;
  virtual void Draw() = 0;
  virtual void Load() = 0;
  virtual void Unload() = 0;
  virtual void LoadHandlers() = 0;
  void Register(std::string methodName, std::function<void(Data data)> method);
};

class MenuScreen :public Window {
public:
  Rectangle ScrollPanel007ScrollView = { 0, 0, 0, 0 };
  Vector2 ScrollPanel007ScrollOffset = { 0, 0 };
  Vector2 ScrollPanel007BoundsOffset = { 0, 0 };
  MenuScreen();
  ~MenuScreen();
  void Update() override;
  void Draw() override;
  void Load() override;
  void Unload() override;
  void LoadHandlers() override;
};

class HomeScreen :public Window {
public:
  bool toShowMessageBox;
  int messageBoxResponse;
  HomeScreen();
  ~HomeScreen();
  std::string filePath;
  bool toBeLoaded;
  Board* board;
  void Update() override;
  void Draw() override;
  void Load() override;
  void Unload() override;
  void LoadHandlers() override;
};