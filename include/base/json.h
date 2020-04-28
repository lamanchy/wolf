#pragma once

#include <tao/json/value.hpp>

namespace wolf {

class json : public tao::json::value {
 public:
  using taojson = tao::json::value;
  using taojson::taojson;

  json(taojson &&tj) : taojson(std::move(tj)) {}
  json(taojson &&tj, taojson metadata, unsigned long size) : taojson(std::move(tj)), metadata(std::move(metadata)) {}
  json(taojson &&tj, json & j) : taojson(std::move(tj)), size(j.size), metadata(std::move(j.metadata)) {}

  taojson metadata = tao::json::empty_object;
  unsigned long size = 1;
};

}


