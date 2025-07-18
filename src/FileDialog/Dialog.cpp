#include <iostream>
#include <tinyfiledialogs.h>
#include "Dialog.hpp"

bool Dialog::success = false;
std::string Dialog::file = "";

void Dialog::RequestOpenDialog() {
  //std::cout << "Success Fully called the function\n";
  const char* filters[] = { "*.txt" };
  const char* filepath = tinyfd_openFileDialog(
    "Open a File", "", 1, filters, "Text", 0);
  if (filepath) {
    file = filepath;
    success = true;
  }
  else {
    file = "";
    success = false;
  }
  //std::cout << "Opened File\n";
}
void Dialog::RequestSaveDialog() {
  const char* filepath = tinyfd_saveFileDialog("Save File", "output.txt", 0, NULL, NULL);
  if (filepath) {
    file = filepath;
    success = true;
  }
  else {
    file = "";
    success = false;
  }
  //std::cout << "Saved File\n";
}