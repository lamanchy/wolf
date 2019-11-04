//
// Created by lamanchy on 5.2.19.
//

#ifndef WOLF_CIN_H
#define WOLF_CIN_H

#include "istream.h"

namespace wolf {
namespace from {

class cin : public istream {
 public:
  explicit cin(const option<std::string> &file_name) : istream(std::cin) {}
};

}
}

#endif //WOLF_CIN_H
