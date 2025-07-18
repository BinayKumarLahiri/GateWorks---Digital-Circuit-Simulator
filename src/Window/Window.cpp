#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
#include <chrono>
#include <iostream>
#include <string>
#include "Window.hpp"
#include "../FileDialog/Dialog.hpp"
#include "../FileDialog/RecentProjects.hpp"
#include "../Simulator/Simulator.hpp"
// #include <tinyfiledialogs.h>

Window::Window(std::string windowName) {
  this->windowName = windowName;
}
Window::~Window() {}
void Window::Register(std::string methodName, std::function<void(Data)> method) {
  this->eventHandler[methodName] = method;
}



// Menu Screen
MenuScreen::MenuScreen() :Window("menu") {}
MenuScreen::~MenuScreen() {}
void MenuScreen::Load() {
  //std::cout << this->windowName << " is Loaded." << std::endl;
  this->LoadHandlers();
}
void MenuScreen::Unload() {
  //std::cout << this->windowName << " is Unloaded." << std::endl;
}
void MenuScreen::Update() {}
void MenuScreen::LoadHandlers() {
  // this->Register("loadFile", []() {
  //   std::cout << "Loading File." << std::endl;
  //   });
  this->Register("quit", [](Data) {
    //std::cout << "Quiting." << std::endl;
    });
}
void MenuScreen::Draw() {

  // Rectangle ScrollPanel007ScrollView = { 0, 0, 0, 0 };
  // Vector2 ScrollPanel007ScrollOffset = { 0, 0 };
  // Vector2 ScrollPanel007BoundsOffset = { 0, 0 };

  GuiPanel((Rectangle) { 0, 0, 1000, 600 }, NULL);
  GuiLabel((Rectangle) { 176, 200, 280, 32 }, "GATE WORKS : DIGITAL CIRCUIT SIMULATOR");
  GuiGroupBox((Rectangle) { 120, 240, 360, 168 }, "MENU OPTIONS");
  if (GuiButton((Rectangle) { 128, 256, 344, 40 }, "CREATE NEW CIRCUIT")) this->eventHandler["goToHome"](Data{ "" });
  if (GuiButton((Rectangle) { 128, 304, 344, 40 }, "LOAD FROM FILE")) this->eventHandler["loadFile"](Data{ "" });
  if (GuiButton((Rectangle) { 128, 352, 344, 40 }, "QUIT PROGRAM")) {
    // this->eventHandler["quit"](Data{ "" });
    Simulator::running = false;
  }
  GuiLabel((Rectangle) { 760, 560, 224, 24 }, "CREATED BY: BINAY KUMAR LAHIRI");
  // GuiScrollPanel((Rectangle) { 616, 48, 360 - ScrollPanel007BoundsOffset.x, 488 - ScrollPanel007BoundsOffset.y }, NULL, (Rectangle) { 616, 48, 360, 488 }, & ScrollPanel007ScrollOffset, & ScrollPanel007ScrollView);
  GuiGroupBox((Rectangle) { 608, 32, 376, 512 }, "RECENT PROJECTS");
  if (ProjectList::projects.size() > 0) {
    // --- Recent Projects Scroll Panel ---
    // static Vector2 ScrollPanel007ScrollOffset = { 0, 0 };
    // Remove horizontal offset, only use vertical
    Rectangle scrollPanelBounds = { 616, 48, 360, 488 };
    // Height grows with items, width fixed
    Rectangle contentBounds = { 0, 0, 360, 10 + (int)(ProjectList::projects.size() * 40) };

    // Only vertical scrolling: set .x to 0 after GuiScrollPanel
    GuiScrollPanel(
      scrollPanelBounds, NULL,
      contentBounds,
      &ScrollPanel007ScrollOffset,
      &ScrollPanel007ScrollView
    );
    ScrollPanel007ScrollOffset.x = 0; // Lock horizontal scroll

    BeginScissorMode(
      (int)ScrollPanel007ScrollView.x,
      (int)ScrollPanel007ScrollView.y,
      (int)ScrollPanel007ScrollView.width,
      (int)ScrollPanel007ScrollView.height
    );

    // Fix: invert scroll direction by using +ScrollPanel007ScrollOffset.y
    for (size_t i = 0; i < ProjectList::projects.size(); ++i) {
      float y = 10 + i * 40 + ScrollPanel007ScrollOffset.y; // Use + for correct direction
      float x = 0;

      float panelWidth = scrollPanelBounds.width - 16;
      Rectangle panelRect = { scrollPanelBounds.x + x, scrollPanelBounds.y + y, panelWidth, 32 };
      Rectangle labelRect = { panelRect.x + 8, panelRect.y + 4, panelWidth - 70, 24 };
      Rectangle openBtnRect = { panelRect.x + panelWidth - 50, panelRect.y + 4, 20, 24 };
      Rectangle deleteBtnRect = { panelRect.x + panelWidth - 25, panelRect.y + 4, 20, 24 };

      GuiPanel(panelRect, NULL);
      GuiLabel(labelRect, ProjectList::projects[i].projectName.c_str());

      if (GuiButton(openBtnRect, "#023#")) {
        // Open project logic here
        this->eventHandler["openFile"]({ ProjectList::projects[i].projectFile });
      }
      if (GuiButton(deleteBtnRect, "#143#")) {
        ProjectList::Remove(i);
        break;
      }
    }

    EndScissorMode();
  }
  else {
    GuiLabel((Rectangle) { 638, 50, 346, 50 }, "Create a Circuit and Save it for it to be Seen Here");
  }
}



// Home Screen
HomeScreen::HomeScreen() :Window("home") {
  this->board = new Board((Rectangle) { 8, 80, 984, 480 });
  this->toBeLoaded = false;
  this->filePath = "";
  this->toShowMessageBox = false;
  this->messageBoxResponse = -1;
}
HomeScreen::~HomeScreen() {
  delete this->board;
}
void HomeScreen::Load() {
  //std::cout << this->windowName << " is Loaded." << std::endl;
  this->LoadHandlers();

}
void HomeScreen::Unload() {
  //std::cout << this->windowName << " is Unloaded." << std::endl;
}
void HomeScreen::Update() {
  this->board->Update();
  if (this->toBeLoaded) {
    this->board->Deserialize(this->filePath);
    this->toBeLoaded = false;
  }
}
void HomeScreen::LoadHandlers() {}
void HomeScreen::Draw() {

  // The GUI Part
  GuiPanel((Rectangle) { 0, 0, 1000, 600 }, NULL);
  GuiGroupBox((Rectangle) { 8, 72, 984, 520 }, "CIRCUIT BOARD");
  GuiGroupBox((Rectangle) { 8, 16, 520, 40 }, "COMPONENTS");
  if (GuiButton((Rectangle) { 16, 24, 56, 24 }, "AND")) {
    this->board->InsertGate("", AND, this->board->textures["and"]);
    //std::cout << "And Gate" << std::endl;
  }
  if (GuiButton((Rectangle) { 408, 24, 56, 24 }, "BULB")) {
    this->board->InsertBulb("", this->board->textures["bulb_on"], this->board->textures["bulb_off"]);
    std::cout << "Bulb" << std::endl;
  }
  if (GuiButton((Rectangle) { 72, 24, 56, 24 }, "OR")) {
    this->board->InsertGate("", OR, this->board->textures["or"]);
    std::cout << "Or Gate" << std::endl;
  }
  if (GuiButton((Rectangle) { 184, 24, 56, 24 }, "NAND")) {
    this->board->InsertGate("", NAND, this->board->textures["nand"]);
    std::cout << "Nand Gate" << std::endl;
  }
  if (GuiButton((Rectangle) { 352, 24, 56, 24 }, "SWITCH")) {
    this->board->InsertSwitch("", this->board->textures["switch_on"], this->board->textures["switch_off"]);
    std::cout << "Switch" << std::endl;
  }
  if (GuiButton((Rectangle) { 464, 24, 56, 24 }, "CLOCK")) {
    this->board->InsertClock("");
    std::cout << "Clock Inserted\n";
  }
  if (GuiButton((Rectangle) { 128, 24, 56, 24 }, "NOT")) {
    this->board->InsertGate("", NOT, this->board->textures["not"]);
    std::cout << "Not Gate" << std::endl;
  }
  if (GuiButton((Rectangle) { 296, 24, 56, 24 }, "XOR")) {
    this->board->InsertGate("", XOR, this->board->textures["xor"]);
    std::cout << "XOR Gate" << std::endl;
  }
  if (GuiButton((Rectangle) { 240, 24, 56, 24 }, "NOR")) {
    this->board->InsertGate("", NOR, this->board->textures["nor"]);
    std::cout << "NOR Gate" << std::endl;
  }
  GuiGroupBox((Rectangle) { 536, 16, 216, 40 }, "MENU");
  if (GuiButton((Rectangle) { 688, 24, 56, 24 }, "#113#QUIT")) {
    this->toShowMessageBox = true;
  }
  if (GuiButton((Rectangle) { 544, 24, 72, 24 }, "#002#SAVE")) {
    //std::cout << "Saving File." << std::endl;
    Dialog::RequestSaveDialog();
    if (Dialog::success) {
      //std::cout << Dialog::file << std::endl;
      this->board->SaveBoard(Dialog::file);
    }
    else {
      Dialog::file.clear();
      //std::cout << "No File Specified\n";
    }
  }
  if (GuiButton((Rectangle) { 616, 24, 72, 24 }, (this->board->simulationRunning ? "#132#STOP" : "#131#START"))) {
    this->board->simulationRunning = !this->board->simulationRunning;
  }


  this->board->Draw();

  if (this->toShowMessageBox) {
    Rectangle messageBoxBound = Rectangle{
      300,
      200,
      400,
      200
    };
    this->messageBoxResponse = GuiMessageBox(messageBoxBound, "Confirm Exit", "Are you sure you want to exit?", "Save & Exit;Discard & Exit;Cancel");
  }
  if (this->messageBoxResponse == 0 || this->messageBoxResponse == 3) {
    this->toShowMessageBox = false;
  }
  if (messageBoxResponse == 1) {
    //std::cout << "0\n";
    //std::cout << "Saving File." << std::endl;
    Dialog::RequestSaveDialog();
    if (Dialog::success) {
      //std::cout << Dialog::file << std::endl;
      this->board->SaveBoard(Dialog::file);
      this->toBeLoaded = false;
      this->toShowMessageBox = false;
      this->messageBoxResponse = -1;
      this->eventHandler["goToMenu"](Data{ "" });
    }
    else {
      Dialog::file.clear();
      //std::cout << "No File Specified\n";
    }
    this->toBeLoaded = false;
    this->toShowMessageBox = false;
    this->messageBoxResponse = -1;
  }
  else if (messageBoxResponse == 2) {
    this->board->ClearBoard();
    this->toBeLoaded = false;
    this->toShowMessageBox = false;
    this->messageBoxResponse = -1;
    this->eventHandler["goToMenu"](Data{ "" });
  }
  // if (this->messageBoxResponse != -1)std::cout << this->messageBoxResponse << std::endl;



}
