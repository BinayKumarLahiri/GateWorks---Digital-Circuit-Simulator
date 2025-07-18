#pragma once
#include <string>
#include <vector>

class Dialog {
public:
  static bool success;
  static std::string file;
  static void RequestOpenDialog();
  static void RequestSaveDialog();
};