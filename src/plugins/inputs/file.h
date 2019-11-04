#pragma once

#include <base/plugins/threaded_plugin.h>
#include <base/options/base_option.h>
#include "stream.h"
namespace wolf {
namespace file {

class input : public stream::input {
 public:
  explicit input(const option<std::string> &file_name) : stream::input(file) {
    file.open(file_name->value(), std::ios_base::binary);
  }

 private:
  std::ifstream file;
};

}
}


