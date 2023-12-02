#include "files.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

namespace base {
namespace files {
using std::cout;

void GetWorkingDirectory(char* path, int len) {
  getcwd(path, len);
}

void GetAllFilePaths(const char* directory, vector<string>& paths) {
  DIR* p_dir;
  struct dirent* ptr;

  if (!(p_dir = opendir(directory))) {
    cout << directory << " can't open\n";
    return;
  }

  while ((ptr = readdir(p_dir)) != 0) {
    if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
      string path = directory;
      path = path + "/" + ptr->d_name;
      paths.push_back(path);
      cout << path << "\n";
    }
  }
}

bool Exists(const char* file) {
  if (!file || strlen(file) == 0) {
    return false;
  }

  struct stat s;
  return stat(file, &s) == 0;
}

bool IsDirectory(const char* file) {
  if (!Exists(file)) {
    return false;
  }

  struct stat s;
  if (stat(file, &s) == 0) {
    return S_ISDIR(s.st_mode);
  }

  return false;
}
}  // namespace files
}  // namespace base