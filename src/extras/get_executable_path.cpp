//
// Created by lamanchy on 27.5.19.
//

#include "get_executable_path.h"

std::string wolf::extras::get_executable_dir() {
  char *path = nullptr;
  int length, dirname_length;
  std::string dir_path;

  length = wai_getExecutablePath(nullptr, 0, &dirname_length);
  if (length > 0) {
    path = (char *) malloc(length + 1);
    if (not path)
      abort();
    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';

    char separator = path[dirname_length];
    path[dirname_length] = '\0';
    dir_path = std::string(path) + separator;
    free(path);
  } else {
    throw std::runtime_error("coulndt get path");
  }
  return dir_path;
}
