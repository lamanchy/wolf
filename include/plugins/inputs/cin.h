#pragma once

#include <plugins/inputs/stream.h>

namespace wolf {

class cin : public stream::input {
 public:
  explicit cin(const option<std::string> &file_name) : stream::input(std::cin) {}
};

}


