#include "RecentProjects.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>


// Function to Split the given string into a list of strings based on some delimiter string
std::vector<std::string> splitString(const std::string& str, const std::string& delimiter) {
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

std::vector<Project> ProjectList::projects;
// C:\Binary\Coding\Projects\GateSimulator\project_cache.txt
const std::string ProjectList::project_cache = "C:/Binary/Coding/Projects/GateSimulator/project_cache.txt";

void ProjectList::Insert(Project project) {
  ProjectList::projects.push_back(project);
  SaveProject();
  // for (auto project : projects) {
  //   std::cout << "Name: " << project.projectName << "\nPath: " << project.projectFile << std::endl;
  // }
}
void ProjectList::Remove(int index) {
  ProjectList::projects.erase(ProjectList::projects.begin() + index);
  SaveProject();
}
bool ProjectList::SaveProject() {
  std::string projectsCache = "";
  for (auto project : projects) {
    projectsCache.append(project.projectName).append(",");
    projectsCache.append(project.projectFile).append(";");
  }

  std::ofstream file(project_cache);
  if (file.is_open()) {
    file << projectsCache;
    //std::cout << "Project Cache Saved Successfully\n";
    file.close();
    return true;
  }
  else {
    //std::cout << "Failed to Cache Project File\n";
    return false;
  }
}
bool ProjectList::LoadProjects() {
  projects.clear();
  std::ifstream file(project_cache);
  if (!file.is_open()) {
    //std::cout << "Failed to open project cache file: " << project_cache << std::endl;
    return false;
  }

  std::string data;
  file >> data;
  file.close();

  if (data.empty()) return false;

  std::vector<std::string> entries = splitString(data, ";");
  for (const auto& entry : entries) {
    if (entry.empty()) continue;
    std::vector<std::string> fields = splitString(entry, ",");
    if (fields.size() == 2) {
      Project proj;
      proj.projectName = fields[0];
      proj.projectFile = fields[1];
      projects.push_back(proj);
    }
    else {
      // Malformed entry, skip or handle as needed
      return false;
    }
  }
  for (auto project : projects) {
    //std::cout << "Name: " << project.projectName << "\nPath: " << project.projectFile << std::endl;
  }
  return true;
}