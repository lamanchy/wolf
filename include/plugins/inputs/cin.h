#pragma once

#include <plugins/inputs/stream.h>

namespace wolf {

class cin : public stream::input {
 public:
  cin() : stream::input(std::cin) {}
};

}


