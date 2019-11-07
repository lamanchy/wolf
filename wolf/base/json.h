#pragma once

#include <tao/json/value.hpp>

namespace wolf {

class json : public tao::json::value {
 public:
  using taojson = tao::json::value;
  using taojson::taojson;

  json(taojson &&tj) : taojson(std::move(tj)) {}

  json copy_metadata(const json &source) {
    metadata = source.metadata;
    return *this;
  }
  json copy_size(const json &source) {
    size = source.size;
    return *this;
  }

  json copy_all(const json &source) {
    return copy_metadata(source).copy_size(source);
  }

  taojson metadata = tao::json::empty_object;
  unsigned long size = 1;
};

}


