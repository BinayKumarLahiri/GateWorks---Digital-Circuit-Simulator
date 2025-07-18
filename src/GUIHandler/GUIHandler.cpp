#include "GUIHandler.hpp"
#include <string>
#include <iostream>
#include "../FileDialog/Dialog.hpp"
#include "../FileDialog/RecentProjects.hpp"
GUIHandler::GUIHandler() {
  this->currentWindow = "menu";
}
GUIHandler::~GUIHandler() {
  for (const auto& it : this->windows)delete it.second;
}
void GUIHandler::ChangeWindow(std::string windowName) {
  this->windows[this->currentWindow]->Unload();
  this->currentWindow = windowName;
  this->windows[this->currentWindow]->Load();
}
void GUIHandler::Update() {
  // for (const auto& it : this->windows)it.second->Update();
  this->windows[this->currentWindow]->Update();
}
void GUIHandler::Draw() {
  // TODO: Check if the current window is actually present
  this->windows[this->currentWindow]->Draw();
}
void GUIHandler::Init() {
  this->LoadWindows();
  if (ProjectList::LoadProjects()) {
    //std::cout << "Loaded Projects\n";
  }
  else {
    //std::cout << "Fialed to Load Projects\n";
  }
}
void GUIHandler::LoadWindows() {
  // Inserting the Menu Screen into the gui Handler
  this->windows["menu"] = new MenuScreen();
  this->windows["home"] = new HomeScreen();
  this->windows["menu"]->Register("goToHome", [this](Data) {
    this->ChangeWindow("home");
    });
  this->windows["menu"]->Register("loadFile", [this](Data) {
    //std::cout << "Trying to call Dialog\n";
    Dialog::RequestOpenDialog();
    if (Dialog::success) {
      //std::cout << Dialog::file << std::endl;
      static_cast<HomeScreen*>(this->windows["home"])->filePath = Dialog::file;
      static_cast<HomeScreen*>(this->windows["home"])->toBeLoaded = true;
      this->ChangeWindow("home");
    }
    else {
      Dialog::file.clear();
      //std::cout << "No File Specified\n";
    }
    });
  this->windows["menu"]->Register("openFile", [this](Data d) {
    if (d.fileName != "") {
      //std::cout << d.fileName << std::endl;
      static_cast<HomeScreen*>(this->windows["home"])->filePath = d.fileName;
      static_cast<HomeScreen*>(this->windows["home"])->toBeLoaded = true;
      this->ChangeWindow("home");
    }
    });
  this->windows["home"]->Register("goToMenu", [this](Data) {
    this->ChangeWindow("menu");
    });

}

