

#ifndef WOLF_GET_EXECUTABLE_PATH_H
#define WOLF_GET_EXECUTABLE_PATH_H

#include <string>
#include <whereami/whereami.h>
#include <stdexcept>

namespace wolf {
namespace extras {

inline std::string get_executable_path() {
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

}
}

#endif //WOLF_GET_EXECUTABLE_PATH_H
