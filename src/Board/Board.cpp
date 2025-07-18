#include "Board.hpp"
#include <raylib.h>
#include <string>
#include <iostream>
#include <raymath.h>
#include <chrono>   // For time-related functionalities (timestamps)
#include <ctime>    // For std::time and std::gmtime/localtime
#include <iomanip>  // For std::put_time to format time
#include <algorithm>
#include <raygui.h>
#include <typeinfo>
#include <fstream>  // For File Operations
#include "../FileDialog/RecentProjects.hpp"


enum class ManhattanBendStyle {
  BendAtX2ThenY = 0, // Go horizontal (x1 to x2) then vertical (y1 to y2). Corner at (x2, y1)
  BendAtY2ThenX,     // Go vertical (y1 to y2) then horizontal (x1 to x2). Corner at (x1, y2)
  BendAtMidpoint     // Go horizontal (x1 to midpoint_x), vertical (midpoint_y), horizontal (midpoint_x to x2)
};

// Function to draw a Manhattan (orthogonal) connection between two points
void DrawLineManhattan(Vector2 startPos, Vector2 endPos, float thick, Color color, ManhattanBendStyle style = ManhattanBendStyle::BendAtX2ThenY) {
  // Ensure points are valid
  if (startPos.x == endPos.x && startPos.y == endPos.y) {
    DrawCircleV(startPos, thick / 2.0f, color); // Draw a point if they are the same
    return;
  }

  switch (style) {
  case ManhattanBendStyle::BendAtX2ThenY: {
    // First segment: Horizontal from start X to end X, at start Y
    Vector2 cornerPoint = { endPos.x, startPos.y };
    DrawLineEx(startPos, cornerPoint, thick, color);
    // Second segment: Vertical from corner Y to end Y, at end X
    DrawLineEx(cornerPoint, endPos, thick, color);
    break;
  }
  case ManhattanBendStyle::BendAtY2ThenX: {
    // First segment: Vertical from start Y to end Y, at start X
    Vector2 cornerPoint = { startPos.x, endPos.y };
    DrawLineEx(startPos, cornerPoint, thick, color);
    // Second segment: Horizontal from corner X to end X, at end Y
    DrawLineEx(cornerPoint, endPos, thick, color);
    break;
  }
  case ManhattanBendStyle::BendAtMidpoint: {
    // Calculate a midpoint along the X or Y axis for the bend
    float midX = (startPos.x + endPos.x) / 2.0f;
    float midY = (startPos.y + endPos.y) / 2.0f;

    // Option 1: Horizontal -> Vertical -> Horizontal (Z-like)
    Vector2 p1 = { midX, startPos.y };
    Vector2 p2 = { midX, endPos.y };

    DrawLineEx(startPos, p1, thick, color); // Horizontal to midX
    DrawLineEx(p1, p2, thick, color);       // Vertical across
    DrawLineEx(p2, endPos, thick, color);   // Horizontal to endX
    break;
  }
  }
}

// Custom function to generate the uniques string Ids
std::string generateUniqueId(const std::string& customString) {
  // Get the current time point in milliseconds
  auto now = std::chrono::system_clock::now();
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

  // Extract seconds and remaining milliseconds
  long long total_milliseconds = milliseconds.count();
  time_t seconds = total_milliseconds / 1000;
  long long remaining_milliseconds = total_milliseconds % 1000;

  // Convert time_t to tm structure for formatting
  struct tm* ptm = std::gmtime(&seconds); // Use gmtime for UTC time, or localtime for local time

  // Format the date and time into a string stream
  std::ostringstream oss;
  oss << std::put_time(ptm, "%Y%m%d_%H%M%S"); // YYYYMMDD_HHMMSS
  oss << "_" << std::setfill('0') << std::setw(3) << remaining_milliseconds; // Add milliseconds with leading zeros

  // Combine the custom string with the formatted timestamp
  return customString + "_" + oss.str();
}

// Function to Split the given string into a list of strings based on some delimiter string
std::vector<std::string> splitStringByDelimiter(const std::string& str, const std::string& delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter); // Find first occurrence of delimiter

  while (end != std::string::npos) { // Loop while delimiter is found
    tokens.push_back(str.substr(start, end - start)); // Extract substring
    start = end + delimiter.length(); // Move start past the delimiter
    end = str.find(delimiter, start); // Find next occurrence from new start
  }
  tokens.push_back(str.substr(start)); // Add the last token

  return tokens;
}
// Function to get the File Name from the File Path
std::string getFileName(const std::string& filePath) {
  // Determine the appropriate path separator
  char separator = '/';
#ifdef _WIN32
  separator = '\\';
#endif

  size_t lastSeparatorPos = filePath.rfind(separator);
  if (lastSeparatorPos != std::string::npos) {
    std::string fileName = filePath.substr(lastSeparatorPos + 1);
    int dotPosition = fileName.rfind(".");
    return fileName.substr(dotPosition + 1);
  }
  return filePath.substr(filePath.rfind(".") + 1); // If no separator found, the whole path is the filename
}

// Board Class
Board::Board(Rectangle dimension) {
  this->inputPin = nullptr;
  this->outputPin = nullptr;
  this->dimension = dimension;
  this->activeComponent = nullptr;
  this->simulationRunning = false;
  this->dropDown = new DropDown(this);

  this->textures["and"] = LoadTexture("../asset/graphics/Gate_and.png");
  this->textures["or"] = LoadTexture("../asset/graphics/Gate_or.png");
  this->textures["not"] = LoadTexture("../asset/graphics/Gate_not.png");
  this->textures["nor"] = LoadTexture("../asset/graphics/Gate_nor.png");
  this->textures["nand"] = LoadTexture("../asset/graphics/Gate_nand.png");
  this->textures["xor"] = LoadTexture("../asset/graphics/Gate_xor.png");
  this->textures["switch_on"] = LoadTexture("../asset/graphics/Switch_on.png");
  this->textures["switch_off"] = LoadTexture("../asset/graphics/Switch_off.png");
  this->textures["bulb_on"] = LoadTexture("../asset/graphics/Bulb_on.png");
  this->textures["bulb_off"] = LoadTexture("../asset/graphics/Bulb_off.png");
  this->textures["clock_1"] = LoadTexture("../asset/graphics/clock_1.png");
  this->textures["clock_5"] = LoadTexture("../asset/graphics/clock_5.png");
  this->textures["clock_10"] = LoadTexture("../asset/graphics/clock_10.png");
  this->textures["clock_30"] = LoadTexture("../asset/graphics/clock_30.png");
  this->textures["clock_60"] = LoadTexture("../asset/graphics/clock_60.png");

}
Board::~Board() {
  UnloadTexture(this->textures["and"]);
  UnloadTexture(this->textures["or"]);
  UnloadTexture(this->textures["not"]);
  UnloadTexture(this->textures["nor"]);
  UnloadTexture(this->textures["nand"]);
  UnloadTexture(this->textures["xor"]);
  UnloadTexture(this->textures["switch_on"]);
  UnloadTexture(this->textures["switch_off"]);
  UnloadTexture(this->textures["bulb_on"]);
  UnloadTexture(this->textures["bulb_off"]);
  UnloadTexture(this->textures["clock_1"]);
  UnloadTexture(this->textures["clock_5"]);
  UnloadTexture(this->textures["clock_10"]);
  UnloadTexture(this->textures["clock_30"]);
  UnloadTexture(this->textures["clock_60"]);
}
void Board::DrawConnection(Component* comp) {
  // Draw only the input Connections from every component
  for (auto inPin : (*comp->inputs)) {
    if (inPin.child) {
      if (inPin.child->self->state)DrawLineManhattan(inPin.position, inPin.child->position, 2.0f, { 255,29,13,255 }, ManhattanBendStyle::BendAtMidpoint);
      else DrawLineManhattan(inPin.position, inPin.child->position, 2.0f, { 110,168,74,255 }, ManhattanBendStyle::BendAtMidpoint);
    }
  }
}

void Board::Draw() {
  // DrawRectangleRec(this->dimension, RED);
  for (auto& it : this->components) {
    it.second->Draw();
    this->DrawConnection(it.second); // NOTE: This Line is added here after rewritting the DrawConnection Method
  }
  if (this->inputPin != nullptr && this->outputPin != nullptr) {
    DrawLineManhattan(this->inputPin->position, this->outputPin->position, 2.0f, BLACK, ManhattanBendStyle::BendAtMidpoint);
  }
  if (this->inputPin != nullptr) {
    DrawCircleV(this->inputPin->position, 7, GREEN);
  }
  if (this->outputPin != nullptr)DrawCircleV(this->outputPin->position, 7, GREEN);
  this->dropDown->Draw();

}
void Board::Update() {
  for (auto& it : this->components) {
    it.second->Update();
    if (this->simulationRunning &&
      typeid(*(it.second)) == typeid(Switch) &&
      IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) &&
      CheckCollisionPointRec(GetMousePosition(), { it.second->position.x,it.second->position.y,it.second->size.x,it.second->size.y })) {
      dynamic_cast<Switch*>(it.second)->ToggleState();
    }
    else if (this->simulationRunning &&
      typeid(*(it.second)) == typeid(Clock) &&
      IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) &&
      CheckCollisionPointRec(GetMousePosition(), { it.second->position.x,it.second->position.y,it.second->size.x,it.second->size.y })) {
      dynamic_cast<Clock*>(it.second)->ToggleState();
    }
  }
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && this->activeComponent != nullptr) {
    Vector2 delta = GetMouseDelta();
    float posX = this->activeComponent->position.x + delta.x;
    float posY = this->activeComponent->position.y + delta.y;
    if (posX < this->GetCoordinates({ 0,0 }).x)posX = this->GetCoordinates({ 0,0 }).x;
    else if (posX > this->GetCoordinates({ 0,0 }).x + this->dimension.width - this->activeComponent->size.x)posX = this->GetCoordinates({ 0,0 }).x + this->dimension.width - this->activeComponent->size.x;

    if (posY < this->GetCoordinates({ 0,0 }).y)posY = this->GetCoordinates({ 0,0 }).y;
    else if (posY > this->GetCoordinates({ 0,0 }).y + this->dimension.height - this->activeComponent->size.y + 20)posY = this->GetCoordinates({ 0,0 }).y + this->dimension.height - this->activeComponent->size.y + 20;


    this->activeComponent->position = {
      posX,
      posY
    };
  }
  if (IsMouseButtonUp(MOUSE_LEFT_BUTTON))this->activeComponent = nullptr;
  if (this->inputPin != nullptr && this->outputPin != nullptr) {
    // Connect the Pins
    if (this->inputPin->child == nullptr) {
      this->inputPin->child = this->outputPin;
      this->outputPin->parent->push_back(this->inputPin);
      this->heads.erase(std::remove(this->heads.begin(), this->heads.end(), this->outputPin->self->id), this->heads.end());
      // std::cout << "Pins Connected: " << this->inputPin->self->id << " " << this->outputPin->self->id << std::endl;
      // Printing all the Head Components
      //for (std::string ids : this->heads)std::cout << ids << " ";
      //std::cout << std::endl;
    }
    else {
      //std::cout << "Can't Connect Pins\n";
    }
    this->inputPin = nullptr;
    this->outputPin = nullptr;
  }

  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    //std::cout << "Right Click\n";
    Vector2 pos = GetMousePosition();
    for (auto it : this->components) {
      Component* comp = it.second;
      if (CheckCollisionPointRec(pos, { comp->position.x,comp->position.y,comp->size.x,comp->size.y }) && !this->simulationRunning) {
        this->dropDown->SetComponent(comp);
        break;
      }
    }
  }
  if (this->simulationRunning)this->Simulate();

}
void Board::InsertGate(std::string compId, GateType type, Texture texture) {
  std::string id = compId.empty() ? generateUniqueId("Gate") : compId;
  this->components[id] = new Gate(type, texture, id, this->GetCoordinates({ 50,50 }));
  this->components[id]->Register("select", [this, id](void*) {
    this->activeComponent = this->components[id];
    });

  this->components[id]->Register("inputPin", [this](void* pin) {
    std::cout << "Input Pin Selected" << std::endl;
    this->inputPin = (InputPin*)pin;
    });

  this->components[id]->Register("outputPin", [this](void* pin) {
    std::cout << "Output Pin Selected" << std::endl;
    this->outputPin = (OutputPin*)pin;
    std::cout << this->outputPin->self->id << std::endl;
    });

  this->heads.push_back(id);
  std::cout << id << std::endl;
}
void Board::InsertSwitch(std::string compId, Texture on, Texture off) {
  std::string id = compId.empty() ? generateUniqueId("Switch") : compId;
  this->components[id] = new Switch(id, on, off, this->GetCoordinates({ 50,50 }));
  this->components[id]->Register("select", [this, id](void*) {
    this->activeComponent = this->components[id];
    });
  this->components[id]->Register("inputPin", [this](void* pin) {
    std::cout << "Input Pin Selected" << std::endl;
    this->inputPin = (InputPin*)pin;
    });

  this->components[id]->Register("outputPin", [this](void* pin) {
    std::cout << "Output Pin Selected" << std::endl;
    this->outputPin = (OutputPin*)pin;
    std::cout << this->outputPin->self->id << std::endl;
    });
  this->heads.push_back(id);
  std::cout << id << std::endl;
}
void Board::InsertClock(std::string compId) {
  std::string id = compId.empty() ? generateUniqueId("Clock") : compId;
  std::map<int, Texture> texture;
  texture[1] = this->textures["clock_1"];
  texture[5] = this->textures["clock_5"];
  texture[10] = this->textures["clock_10"];
  texture[30] = this->textures["clock_30"];
  texture[60] = this->textures["clock_60"];

  this->components[id] = new Clock(id, texture, this->GetCoordinates({ 50,50 }));
  this->components[id]->Register("select", [this, id](void*) {
    this->activeComponent = this->components[id];
    });
  this->components[id]->Register("inputPin", [this](void* pin) {
    std::cout << "Input Pin Selected" << std::endl;
    this->inputPin = (InputPin*)pin;
    });

  this->components[id]->Register("outputPin", [this](void* pin) {
    std::cout << "Output Pin Selected" << std::endl;
    this->outputPin = (OutputPin*)pin;
    std::cout << this->outputPin->self->id << std::endl;
    });
  this->heads.push_back(id);
  std::cout << id << std::endl;
}
void Board::InsertBulb(std::string compId, Texture on, Texture off) {
  std::string id = compId.empty() ? generateUniqueId("Bulb") : compId;
  this->components[id] = new Bulb(id, on, off, this->GetCoordinates({ 50,50 }));
  this->components[id]->Register("select", [this, id](void*) {
    this->activeComponent = this->components[id];
    });
  this->components[id]->Register("inputPin", [this](void* pin) {
    std::cout << "Input Pin Selected" << std::endl;
    this->inputPin = (InputPin*)pin;
    });

  this->components[id]->Register("outputPin", [this](void* pin) {
    std::cout << "Output Pin Selected" << std::endl;
    this->outputPin = (OutputPin*)pin;
    std::cout << this->outputPin->self->id << std::endl;
    });
  this->heads.push_back(id);
  std::cout << id << std::endl;
}
void Board::DeleteComponent(Component* comp) {
  // Disconnect all input pins
  if (comp->inputs) {
    //std::cout << "Disconnecting Inputs\n";
    for (auto& input : *(comp->inputs)) {
      if (input.child) {
        // Remove this input from the parent's parent vector
        auto& parents = *(input.child->parent);
        parents.erase(std::remove(parents.begin(), parents.end(), &input), parents.end());
        input.child = nullptr;
      }
    }
  }

  // Disconnect all output pins
  if (comp->outputs) {
    //std::cout << "Disconnecting Outputs\n";
    for (auto& output : *(comp->outputs)) {
      if (output.parent) {
        for (auto* input : *(output.parent)) {
          if (input->child == &output) {
            input->child = nullptr;
          }
        }
        output.parent->clear();
      }
    }
  }

  // Remove from components map
  //std::cout << "Removing from map\n";
  for (auto it = components.begin(); it != components.end(); ++it) {
    if (it->second == comp) {
      components.erase(it);
      break;
    }
  }
  //std::cout << "Removing from heads\n";
  this->heads.erase(std::remove(this->heads.begin(), this->heads.end(), comp->id), this->heads.end());
  // Delete the component
  delete comp;
  //std::cout << "Component Deleted\n";

  // Update the Heads list
  this->heads.clear();
  for (auto& it : this->components) {
    Component* c = it.second;
    bool isHead = true;
    for (auto& pin : *(c->outputs)) {
      if (!pin.parent->empty()) {
        isHead = false;
        break;
      }
    }
    if (isHead)this->heads.push_back(c->id);
  }

  // Update InputPins and OutputPins and ActiveComponent
  if (this->inputPin != nullptr && this->inputPin->self == comp)this->inputPin = nullptr;
  if (this->outputPin != nullptr && this->outputPin->self == comp)this->outputPin = nullptr;
  if (this->activeComponent == comp)this->activeComponent = nullptr;

}
void Board::DeleteConnection(Component* comp, int pin) {
  if (comp != nullptr) {
    InputPin input = (*comp->inputs)[pin];
    //std::cout << "Self: " << input.self->id << std::endl;
    if (input.child) {
      //std::cout << "Child: " << input.child->self->id << std::endl;
      // Remove this input from the parent's parent vector
      auto& parents = *(input.child->parent);
      //std::cout << "Length: " << parents.size() << std::endl;
      parents.erase(std::remove(parents.begin(), parents.end(), &(*comp->inputs)[pin]), parents.end());
      //std::cout << "Length: " << parents.size() << std::endl;
      (*comp->inputs)[pin].child = nullptr;
    }
  }

  // Update the Heads list
  //std::cout << "Head Length: " << this->heads.size() << std::endl;
  this->heads.clear();
  for (auto& it : this->components) {
    Component* c = it.second;
    bool isHead = true;
    for (auto& pin : *(c->outputs)) {
      if (!pin.parent->empty()) {
        isHead = false;
        break;
      }
    }
    if (isHead)this->heads.push_back(c->id);
  }
  //std::cout << "Head Length: " << this->heads.size() << std::endl;
  //std::cout << "Pin Disconnected\n";
}
void Board::PrintBoard() {
  // for (int i = 0;i < 50;i++)std::cout << "-";
  // std::cout << std::endl;
  // /**
  //  * 1. Switch
  //  * 2. Bulb
  //  * 3. Cock
  //  * 4. AND Gate
  //  * 5. OR Gate
  //  * 6. NOT Gate
  //  * 7. NAND Gate
  //  * 8. NOR Gate
  //  * 9. XOR Gate
  //  */
  //  // Components

  // std::string board = "";
  // if (this->components.empty()) {
  //   std::cout << board;
  //   for (int i = 0;i < 50;i++)std::cout << "-";
  //   std::cout << std::endl;
  //   return;
  // }
  // for (auto it : this->components) {
  //   Component* comp = it.second;
  //   board.append(comp->id).append(",");
  //   int type;
  //   if (typeid(*comp) == typeid(Switch)) type = 1;
  //   else if (typeid(*comp) == typeid(Bulb)) type = 2;
  //   else if (typeid(*comp) == typeid(Clock)) type = 3;
  //   else if (typeid(*comp) == typeid(Gate)) {
  //     Gate* gate = static_cast<Gate*>(comp);
  //     if (gate->type == AND) type = 4;
  //     else if (gate->type == OR) type = 5;
  //     else if (gate->type == NOT) type = 6;
  //     else if (gate->type == NAND) type = 7;
  //     else if (gate->type == NOR) type = 8;
  //     else if (gate->type == XOR) type = 9;
  //   }
  //   board.append(std::to_string(type)).append(",");
  //   board.append(std::to_string(comp->position.x)).append(",");
  //   board.append(std::to_string(comp->position.y)).append(",");
  //   board.append(std::to_string(comp->inputCount)).append(",");
  //   board.append(std::to_string(comp->outputCount)).append(",");
  //   board.append((comp->state ? "1" : "0")).append(";");
  // }
  // board.append("|");
  // // Connections
  // for (auto it : this->components) {
  //   Component* comp = it.second;
  //   for (auto ipin : (*comp->inputs)) {
  //     board.append(comp->id).append(","); // To
  //     board.append((ipin.child ? ipin.child->self->id : "-")).append(","); // From
  //     board.append(std::to_string(ipin.pinNo)).append(","); // toPin Number
  //     board.append((ipin.child ? std::to_string(ipin.child->pinNo) : "-")).append(";");
  //   }
  // }
  // board.append("|");
  // // Heads
  // for (int i = 0;i < this->heads.size() - 1;i++) {
  //   board.append(this->heads[i]).append(",");
  // }
  // board.append(this->heads[this->heads.size() - 1]);
  // std::cout << board << std::endl;
  // for (int i = 0;i < 50;i++)std::cout << "-";
  // std::cout << std::endl;
}
Vector2 Board::GetCoordinates(Vector2 coordinate) {
  return Vector2{
    coordinate.x + this->dimension.x,
    coordinate.y + this->dimension.y
  };
}
void Board::Simulate() {
  for (std::string id : (this->heads)) {
    Component* comp = this->components[id];
    std::map<Component*, bool> map;
    map[comp] = true;
    comp->GetState(map);
    map[comp] = false;
  }
}
std::string Board::Serialize() {
  /**
   * 1. Switch
   * 2. Bulb
   * 3. Cock
   * 4. AND Gate
   * 5. OR Gate
   * 6. NOT Gate
   * 7. NAND Gate
   * 8. NOR Gate
   * 9. XOR Gate
   */
   // Components

  std::string board = "";
  if (this->components.empty()) {
    // std::cout << board;
    // for (int i = 0;i < 50;i++)std::cout << "-";
    // std::cout << std::endl;
    return board;
  }
  for (auto it : this->components) {
    Component* comp = it.second;
    board.append(comp->id).append(",");
    int type;
    if (typeid(*comp) == typeid(Switch)) type = 1;
    else if (typeid(*comp) == typeid(Bulb)) type = 2;
    else if (typeid(*comp) == typeid(Clock)) type = 3;
    else if (typeid(*comp) == typeid(Gate)) {
      Gate* gate = static_cast<Gate*>(comp);
      if (gate->type == AND) type = 4;
      else if (gate->type == OR) type = 5;
      else if (gate->type == NOT) type = 6;
      else if (gate->type == NAND) type = 7;
      else if (gate->type == NOR) type = 8;
      else if (gate->type == XOR) type = 9;
    }
    board.append(std::to_string(type)).append(",");
    board.append(std::to_string(comp->position.x)).append(",");
    board.append(std::to_string(comp->position.y)).append(",");
    board.append(std::to_string(comp->inputCount)).append(",");
    board.append(std::to_string(comp->outputCount)).append(",");
    board.append((comp->state ? "1" : "0")).append(";");
  }
  board.append("|");
  // Connections
  for (auto it : this->components) {
    Component* comp = it.second;
    for (auto ipin : (*comp->inputs)) {
      board.append(comp->id).append(","); // To
      board.append((ipin.child ? ipin.child->self->id : "-")).append(","); // From
      board.append(std::to_string(ipin.pinNo)).append(","); // toPin Number
      board.append((ipin.child ? std::to_string(ipin.child->pinNo) : "-")).append(";");
    }
  }
  board.append("|");
  // Heads
  for (int i = 0;i < this->heads.size() - 1;i++) {
    board.append(this->heads[i]).append(",");
  }
  board.append(this->heads[this->heads.size() - 1]);
  return board;
}
void Board::Deserialize(std::string fileName) {
  std::ifstream file(fileName);
  std::string board;
  file >> board; // Read the string as the file doesn't contains any newline character
  file.close();
  std::vector<std::string> sections = splitStringByDelimiter(board, "|");
  //std::cout << "Sections Size: " << sections.size() << std::endl;
  // Load Components First
  if (sections[0].size() != 0) {
    //std::cout << "Loading Components..." << std::endl;
    std::vector<std::string> componentsStr = splitStringByDelimiter(sections[0], ";");
    for (int i = 0;i < componentsStr.size() - 1;i++) {
      std::string comp = componentsStr[i];
      std::vector<std::string> values = splitStringByDelimiter(comp, ",");
      std::string id = values[0];
      int type = atoi(values[1].c_str());
      float posX = atof(values[2].c_str());
      float posY = atof(values[3].c_str());
      int inputCount = atoi(values[4].c_str());
      int outputCount = atoi(values[5].c_str());
      bool state = values[6] == "1" ? true : false;
      if (type == 1) {
        this->InsertSwitch(id, this->textures["switch_on"], this->textures["switch_off"]);
      }
      else if (type == 2) {
        this->InsertBulb(id, this->textures["bulb_on"], this->textures["bulb_off"]);
      }
      else if (type == 3) {
        this->InsertClock(id);
      }
      else if (type == 4) {
        this->InsertGate(id, AND, this->textures["and"]);
      }
      else if (type == 5) {
        this->InsertGate(id, OR, this->textures["or"]);
      }
      else if (type == 6) {
        this->InsertGate(id, NOT, this->textures["not"]);
      }
      else if (type == 7) {
        this->InsertGate(id, NAND, this->textures["nand"]);
      }
      else if (type == 8) {
        this->InsertGate(id, NOR, this->textures["nor"]);
      }
      else if (type == 9) {
        this->InsertGate(id, XOR, this->textures["xor"]);
      }
      else {
        //std::cout << "Invalid Gate Found\n";
        this->components.clear();
        this->heads.clear();
        return;
      }
      //std::cout << this->components.size() << std::endl;
      this->components[id]->position = { posX,posY };
      this->components[id]->inputCount = inputCount;
      this->components[id]->outputCount = outputCount;
      this->components[id]->state = state;
    }
    //std::cout << "Successfully Loaded Components\n";
  }
  // Load the Connections
  //std::cout << sections[1] << std::endl;
  if (sections[1].size() != 0) {
    //std::cout << "Loading Connections" << std::endl;
    std::vector<std::string> connectionData = splitStringByDelimiter(sections[1], ";");
    for (int i = 0;i < connectionData.size() - 1;i++) {
      std::string connectionStr = connectionData[i];
      //std::cout << connectionStr << std::endl;
      std::vector<std::string> tokens = splitStringByDelimiter(connectionStr, ",");
      if (tokens[1] == "-" || tokens[3] == "-")continue;
      else {
        int from = atoi(tokens[3].c_str());
        int to = atoi(tokens[2].c_str());
        Component* toComp = this->components[tokens[0]];
        InputPin* inputPin = &(*toComp->inputs)[to];
        Component* fromComp = this->components[tokens[1]];
        OutputPin* outputPin = &(*fromComp->outputs)[from];
        if (&(*toComp->inputs)[to] != nullptr && &(*fromComp->outputs)[from] != nullptr) {
          if ((*toComp->inputs)[to].child == nullptr) {
            (*toComp->inputs)[to].child = &(*fromComp->outputs)[from];
            (*fromComp->outputs)[from].parent->push_back(&(*toComp->inputs)[to]);
            //std::cout << "Pins Connected: " << fromComp->id << " " << toComp->id << std::endl;
          }
          else {
            //std::cout << "Can't Connect Pins\n";
          }
        }
      }
      //std::cout << "Connections Loaded Successfully" << std::endl;
    }
  }
  // Load the Heads
  if (sections[2].size() != 0) {
    //std::cout << "Loading Heads" << std::endl;
    std::vector<std::string> heads = splitStringByDelimiter(sections[2], ",");
    this->heads.clear();
    for (auto head : heads)this->heads.push_back(head);
  }
}
void Board::SaveBoard(std::string filePath) {
  std::string board = this->Serialize();
  std::ofstream file(filePath);
  if (file.is_open()) {
    file << board;
    std::string fileName = splitStringByDelimiter(GetFileName(filePath.c_str()), ".")[0];
    //std::cout << "File Name: " << fileName << std::endl;
    //std::cout << "Saved Board Successfully" << std::endl;
    ProjectList::Insert({ fileName,filePath });
  }
  else {
    //std::cout << "Failed to Open the File" << std::endl;
  }
  file.close();
}
void Board::ClearBoard() {
  for (auto& it : this->components)delete it.second;
  this->components.clear();
  this->heads.clear();
  this->activeComponent = nullptr;
  this->inputPin = nullptr;
  this->outputPin = nullptr;
}




// Drop Down Class
DropDown::DropDown(Board* parent) {
  // Button Dimension: Width: 50px, Height: 10px 
  this->parent = parent;
  this->active = false;
}
void DropDown::SetComponent(Component* comp) {
  this->comp = comp;
  this->active = true;
  this->dimension = { 150,(float)30 * (comp->inputCount + 2) };
  float posX = comp->position.x + comp->size.x, posY = comp->position.y + comp->size.y;
  if (posX < parent->GetCoordinates({ 0,0 }).x)posX = parent->GetCoordinates({ 0,0 }).x;
  else if (posX + this->dimension.x > this->parent->dimension.x + this->parent->dimension.width)posX = this->parent->dimension.x + this->parent->dimension.width - this->dimension.x;

  if (posY < parent->GetCoordinates({ 0,0 }).y)posY = parent->GetCoordinates({ 0,0 }).y;
  else if (posY + this->dimension.y > this->parent->dimension.y + this->parent->dimension.height)posY = this->parent->dimension.y + this->parent->dimension.height - this->dimension.y;
  this->position = { posX,posY };
}
Vector2 DropDown::GetPosition(Vector2 old) {
  return { this->position.x + old.x,this->position.y + old.y };
}
Rectangle DropDown::GetRectangle(Rectangle old) {
  return { this->position.x + old.x,this->position.y + old.y,old.width,old.height };
}
void DropDown::Draw() {
  if (this->active) {
    GuiPanel({ this->position.x,this->position.y,this->dimension.x,this->dimension.y }, NULL);
    GuiLabel(this->GetRectangle({ 0,0,120,30 }), "DropDown");
    if (GuiButton(this->GetRectangle({ 120,0,30,30 }), "#113#")) {
      this->active = false;
    }
    if (GuiButton(this->GetRectangle({ 0,30,150,30 }), "Delete")) {
      //std::cout << "Delete: " << this->comp->id << std::endl;
      this->parent->DeleteComponent(this->comp);
      this->comp = nullptr;
      this->active = false;
    }
    if (this->comp) {
      int y = 60;
      for (int i = 0;i < this->comp->inputCount;i++) {
        std::string label = "Disconnect Pin " + std::to_string(i);
        if ((*this->comp->inputs)[i].child != nullptr) {
          if (GuiButton(this->GetRectangle({ 0,(float)y,150,30 }), label.c_str())) {
            //std::cout << "Disconnect Pin " << i << std::endl;
            this->parent->DeleteConnection(this->comp, i);
          }
        }
        else {
          Rectangle rec = this->GetRectangle({ 0,(float)y,150,30 });
          if (GuiButton(rec, label.c_str())) {
          }
          DrawRectangleRec(rec, { 230, 41, 55, 50 });
        }

        y += 30;
      }
    }
  }
}



// Component Class
Component::Component(std::string id, Vector2 position) {
  this->id = id;
  this->position = position;
  this->pinSize = 5;
  this->size = { 50,50 };
  this->inputs = new std::vector<InputPin>();
  this->outputs = new std::vector<OutputPin>();

  this->eventHandler["select"] = [](void*) {};
  this->eventHandler["inputPin"] = [](void*) {};
  this->eventHandler["outputPin"] = [](void*) {};
  this->state = false;

}
Component::~Component() {}
void Component::Draw() {
  DrawRectangleV(this->position, this->size, BLACK);
  for (int i = 0;i < this->inputCount;i++) {
    InputPin pin = (*this->inputs)[i];
    DrawCircleV(pin.position, this->pinSize, WHITE);
  }

  for (int i = 0;i < this->outputCount;i++) {
    OutputPin pin = (*this->outputs)[i];
    DrawCircleV(pin.position, this->pinSize, WHITE);
  }

}
void Component::Update() {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), { this->position.x,this->position.y,this->size.x,this->size.y })) {
    this->eventHandler["select"](nullptr);
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    for (int i = 0;i < this->inputCount;i++) {
      InputPin* pin = &(*this->inputs)[i];
      if (CheckCollisionPointCircle(GetMousePosition(), (*pin).position, this->pinSize)) {
        this->eventHandler["inputPin"](pin);
        //std::cout << (*pin).position.x << ", " << (*pin).position.y << std::endl;
        break;
      }
    }
    for (int i = 0;i < this->outputCount;i++) {
      OutputPin* pin = &(*this->outputs)[i];
      if (CheckCollisionPointCircle(GetMousePosition(), (*pin).position, this->pinSize)) {
        this->eventHandler["outputPin"](pin);
        break;
      }
    }
  }



  int delta = 0;
  if (this->inputCount == 1) {
    (*this->inputs)[0].position = { this->position.x,this->position.y + this->size.y / 2 };
  }
  else if (this->inputCount > 1) {
    double spacing = (this->size.y - 10) / (this->inputCount - 1.0);
    double S_end = this->size.y / (2.0 * this->inputCount);
    double S_between = 2.0 * S_end;
    for (int i = 0; i < this->inputCount; i++) {
      (*this->inputs)[i].position = { this->position.x, static_cast<float>(this->position.y + S_end + (i * S_between)) };
    }
  }

  if (this->outputCount == 1) {
    (*this->outputs)[0].position = { this->position.x + this->size.x,this->position.y + this->size.y / 2 };
  }
  else if (this->outputCount > 1) {
    double spacing = (this->size.y - 10) / (this->inputCount - 1.0);
    double S_end = this->size.y / (2.0 * this->inputCount);
    double S_between = 2.0 * S_end;
    for (int i = 0; i < this->inputCount; i++) {
      (*this->outputs)[i].position = { this->position.x + this->size.x,static_cast<float>(this->position.y + S_end + (i * S_between)) };
    }
  }


}
void Component::Register(std::string event, std::function<void(void*)> handler) {
  this->eventHandler[event] = handler != nullptr ? handler : [](void*) {};
}



// Gate Class
Gate::Gate(GateType type, Texture texture, std::string id, Vector2 position) :Component(id, position) {
  this->type = type;
  this->texture = texture;
  this->inputCount = 2;
  this->outputCount = 1;
  int delta = 10;

  if (type == NOT)this->inputCount = 1;

  if (this->inputCount == 1) {
    this->inputs->push_back(InputPin{ {this->position.x,this->position.y + this->size.y / 2},0,nullptr,this });
  }
  else {
    double spacing = this->size.y / (this->inputCount - 1.0);
    for (int i = 0; i < this->inputCount; i++) {
      this->inputs->push_back(InputPin{ {this->position.x, static_cast<float>(this->position.y + (i * spacing)) + delta},i,nullptr,this });
    }
  }

  if (this->outputCount == 1) {
    this->outputs->push_back(OutputPin{ {this->position.x + this->size.x,this->position.y + this->size.y / 2},0, new std::vector<InputPin*>(), this });
  }
  else {
    double spacing = this->size.y / (this->inputCount - 1.0);
    for (int i = 0; i < this->inputCount; i++) {
      this->outputs->push_back(OutputPin{ {this->position.x + this->size.x,static_cast<float>(this->position.y + (i * spacing)) + delta},i, new std::vector<InputPin*>(), this });
    }
  }

}
Gate::~Gate() {}
bool Gate::GetState(std::map<Component*, bool> visited) {
  if (visited[this])return this->state;
  // If the gate is NOT gate there are only one input pin thus if connected invert and return else return false
  if (this->type == NOT) {
    visited[this] = true;
    this->state = (*this->inputs)[0].child ? !(*this->inputs)[0].child->self->GetState(visited) : false;
    visited[this] = false;
    return this->state;
  }
  // Else calculate the state by the states of the child components
  bool temp_state = (this->type == OR || this->type == XOR || this->type == NOR) ? false : true;
  for (auto& input : (*this->inputs)) {
    visited[this] = true;
    bool val = input.child ? input.child->self->GetState(visited) : false; // TODO:: You Are Here
    switch (this->type) {
    case AND:
    case NAND:
      temp_state = temp_state & val;
      break;
    case OR:
    case NOR:
      temp_state = temp_state | val;
      break;
    case XOR:
      temp_state = temp_state ^ val;
      break;
    default:
      temp_state = false;
    }

    visited[this] = false;
  }

  if (this->type == NAND || this->type == NOR) {
    this->state = !temp_state; // Invert the state for the NAND and NOR gates
  }
  else {
    this->state = temp_state; // Set the State
  }
  return this->state; // Return the State
}
void Gate::Draw() {
  DrawTexture(this->texture, this->position.x, this->position.y, WHITE);

}



// Switch Class
Switch::Switch(std::string id, Texture on, Texture off, Vector2 position) :Component(id, position) {
  this->on = on;
  this->off = off;
  this->inputCount = 0;
  this->outputCount = 1;
  this->outputs->push_back(OutputPin{ {this->position.x + this->size.x,this->position.y + this->size.y / 2},0, new std::vector<InputPin*>(), this });
}
Switch::~Switch() {}
bool Switch::GetState(std::map<Component*, bool> visited) {
  return this->state;
}
void Switch::ToggleState() {
  this->state = !this->state;
}
void Switch::Draw() {
  if (this->state) {
    DrawTexture(this->on, this->position.x, this->position.y, WHITE);
  }
  else {
    DrawTexture(this->off, this->position.x, this->position.y, WHITE);
  }
}



// Bulb Class
Bulb::Bulb(std::string id, Texture on, Texture off, Vector2 position) :Component(id, position) {
  this->on = on;
  this->off = off;
  this->inputCount = 1;
  this->outputCount = 0;
  this->inputs->push_back(InputPin{ {this->position.x,this->position.y + this->size.y / 2},0,nullptr,this });
}
Bulb::~Bulb() {}
bool Bulb::GetState(std::map<Component*, bool> visited) {
  this->state = (*this->inputs)[0].child ? (*this->inputs)[0].child->self->GetState(visited) : false;
  return this->state;
}
void Bulb::Draw() {
  if (this->state) {
    DrawTexture(this->on, this->position.x, this->position.y, WHITE);
  }
  else {
    DrawTexture(this->off, this->position.x, this->position.y, WHITE);
  }
}


Clock::Clock(std::string id, std::map<int, Texture> texture, Vector2 position) : Component(id, position) {
  this->inputCount = 0;
  this->outputCount = 1;
  this->lastToggleTime = 0.0;
  this->outputs->push_back(OutputPin{ {this->position.x + this->size.x,this->position.y + this->size.y / 2},0, new std::vector<InputPin*>(), this });
  this->hertz = 1;
  this->interval = 1.0 / this->hertz;
  for (auto it : texture) {
    this->texture[it.first] = it.second;
  }
}
Clock::~Clock() {}
bool Clock::GetState(std::map<Component*, bool> visited) {
  if (GetTime() - this->lastToggleTime >= this->interval) {
    this->state = !this->state;
    this->lastToggleTime = GetTime();
  }
  return this->state;
}
void Clock::ToggleState() {
  if (this->hertz == 1) {
    this->hertz = 5;
  }
  else if (this->hertz == 5) {
    this->hertz = 10;
  }
  else if (this->hertz == 10) {
    this->hertz = 30;
  }
  else if (this->hertz == 30) {
    this->hertz = 60;
  }
  else if (this->hertz == 60) {
    this->hertz = 1;
  }
  else {
    this->hertz = 1;
  }
  this->interval = 1.0 / this->hertz;
}
void Clock::Draw() {
  DrawTexture(this->texture[this->hertz], this->position.x, this->position.y, WHITE);
}


