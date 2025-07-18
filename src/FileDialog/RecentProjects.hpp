#pragma once
#include <string>
#include <vector>

typedef struct Project {
  std::string projectName;
  std::string projectFile;
}Project;

class ProjectList {
public:
  static const std::string project_cache;
  static std::vector<Project> projects;
  static void Insert(Project project);
  static bool SaveProject();
  static bool LoadProjects();
  static void Remove(int index);
};