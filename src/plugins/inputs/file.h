//
// Created by lamanchy on 17.7.19.
//

#ifndef WOLF_FILE_IN_H
#define WOLF_FILE_IN_H

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

#endif //WOLF_FILE_IN_H
