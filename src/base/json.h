//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_JSON_H
#define WOLF_JSON_H

#include <tao/json/value.hpp>

namespace wolf {

class json : public tao::json::value {
 public:
  using taojson = tao::json::value;
  using taojson::taojson;

  json(taojson &&tj) : taojson(std::move(tj)) {}

  taojson metadata = tao::json::empty_object;
  unsigned long size = 1;
};

}

#endif //WOLF_JSON_H
