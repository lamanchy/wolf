

#ifndef WOLF_GET_EXECUTABLE_PATH_H
#define WOLF_GET_EXECUTABLE_PATH_H

#include <iso646.h>
#include <string>
#include <whereami/whereami.h>
#include <stdexcept>
#include <vector>

namespace wolf {
namespace extras {

std::string get_executable_dir();
std::string get_executable_name();
std::string get_separator();

}
}

#endif //WOLF_GET_EXECUTABLE_PATH_H
