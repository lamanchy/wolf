

#include <extras/get_executable_path.h>

std::vector<std::string> get_executable_path() {
  char *path = nullptr;
  int length, dirname_length;
  std::string dir_path, name, separator;

  length = wai_getExecutablePath(nullptr, 0, &dirname_length);
  if (length > 0) {
    path = (char *) malloc(length + 1);
    if (not path)
      abort();
    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';

    name = std::string(&path[dirname_length + 1]);
    separator = std::string(&path[dirname_length], 1);
    path[dirname_length] = '\0';
    dir_path = std::string(path) + separator;
    free(path);
  } else {
    throw std::runtime_error("coulndt get path");
  }
  return std::vector<std::string>{dir_path, name, separator};
}

std::string wolf::extras::get_executable_dir() {
  return get_executable_path()[0];
}
std::string wolf::extras::get_executable_name() {
  return get_executable_path()[1];
}
std::string wolf::extras::get_separator() {
  return get_executable_path()[2];
}
