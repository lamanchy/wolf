//
// Created by lamanchy on 5.2.19.
//

#ifndef WOLF_CIN_H
#define WOLF_CIN_H

#include "istream_in.h"

namespace wolf {

template<typename Serializer>
class cin : public istream_in<Serializer> {
 public:
  explicit cin(const option<std::string> &file_name) : istream_in<Serializer>(std::cin) {}
};

}

#endif //WOLF_CIN_H
