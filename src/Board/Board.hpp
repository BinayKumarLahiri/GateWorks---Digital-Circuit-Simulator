#pragma once
#include <raylib.h>
#include <string>
#include <map>
#include <functional>
#include <vector>

class Component;
struct OutputPin;
typedef struct InputPin {
  Vector2 position;
  int pinNo;
  OutputPin* child;
  Component* self;
}InputPins;

typedef struct OutputPin {
  Vector2 position;
  int pinNo;
  std::vector<InputPin*>* parent;
  Component* self;
}OutputPin;


typedef enum GateType {
  AND, OR, NOT, NOR, NAND, XOR
}GateType;

class Component {
public:
  std::string id;
  Vector2 position;
  Vector2 size;
  float pinSize;
  std::map<std::string, std::function<void(void*)>> eventHandler;
  int inputCount;
  int outputCount;
  std::vector<InputPin>* inputs;
  std::vector<OutputPin>* outputs;
  bool state;
  Texture2D texture;
  Component(std::string id, Vector2 position);
  ~Component();
  void virtual Draw();
  void Update();
  void Register(std::string event, std::function<void(void*)> handler);
  bool virtual GetState(std::map<Component*, bool> visited) = 0; // NOTE: Here the map is inserted lately and must be removed if any problem is caused
};

class Gate :public Component {
public:
  GateType type;
  Gate(GateType type, Texture texture, std::string id, Vector2 position);
  ~Gate();
  bool GetState(std::map<Component*, bool> visited);
  void Draw() override;
};

class Switch :public Component {
public:
  Texture on, off;
  Switch(std::string id, Texture on, Texture off, Vector2 position);
  ~Switch();
  bool GetState(std::map<Component*, bool> visited);
  void ToggleState();
  void Draw() override;
};

class Clock :public Component {
public:
  std::map<int, Texture> texture;
  int hertz;
  double lastToggleTime;
  double interval;
  Clock(std::string id, std::map<int, Texture> texture, Vector2 position);
  ~Clock();
  bool GetState(std::map<Component*, bool> visited);
  void ToggleState();
  void Draw() override;
};

class Bulb :public Component {
public:
  Texture on, off;
  Bulb(std::string id, Texture on, Texture off, Vector2 position);
  ~Bulb();
  bool GetState(std::map<Component*, bool> visited);
  void Draw() override;
};

// Forward declaration of Board
class Board;

class DropDown {
public:
  Vector2 position;
  Vector2 dimension;
  Component* comp;
  Board* parent;
  bool active;
  DropDown(Board* parent);
  void SetComponent(Component* comp);
  Vector2 GetPosition(Vector2 old);
  Rectangle GetRectangle(Rectangle old);
  void Draw();
};

class Board {
public:
  Rectangle dimension;
  std::map<std::string, Component*> components;
  std::map<std::string, Texture> textures;
  std::vector<std::string> heads;
  Component* activeComponent;
  InputPin* inputPin;
  OutputPin* outputPin;
  bool simulationRunning;
  DropDown* dropDown;
  Board(Rectangle dimesion);
  ~Board();
  void Draw();
  void Update();
  Vector2 GetCoordinates(Vector2 coordinate);
  void InsertGate(std::string id, GateType type, Texture texture);
  void InsertSwitch(std::string id, Texture on, Texture off);
  void InsertBulb(std::string id, Texture on, Texture off);
  void InsertClock(std::string id);
  void DeleteComponent(Component* comp);
  void DeleteConnection(Component* comp, int pin);
  void DrawConnection(Component* comp);
  void Simulate();
  void PrintBoard(); // Just to Debug the Program
  std::string Serialize();
  void Deserialize(std::string fileName);
  void SaveBoard(std::string fileName);
  void ClearBoard();
};
