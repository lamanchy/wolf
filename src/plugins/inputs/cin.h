//
// Created by lamanchy on 5.2.19.
//

#ifndef WOLF_CIN_H
#define WOLF_CIN_H

#include "stream.h"

namespace wolf {

 class cin : public stream::input {
 public:
  explicit cin(const option<std::string> &file_name) : stream::input(std::cin) {}
};

}

#endif //WOLF_CIN_H
