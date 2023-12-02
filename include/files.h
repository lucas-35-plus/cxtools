#ifndef FILE_H
#define FILE_H
#include <vector>
#include <string>

namespace base {
  namespace files {
    using std::vector;
    using std::string;

    void GetWorkingDirectory(char*, int);

    void GetAllFilePaths(const char* file, vector<string> &paths);
  
    bool Exists(const char* file);

    bool IsDirectory(const char* file);

  }
}

#endif