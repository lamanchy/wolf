#pragma once

#include <iso646.h>
#include <string>
#include <libs/whereami/whereami.h>
#include <stdexcept>
#include <vector>

namespace wolf {
namespace extras {

std::string get_executable_dir();
std::string get_executable_name();
char get_separator();
std::string normalize_path(std::string path);

}
}


