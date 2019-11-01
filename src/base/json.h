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

//  basic_value<tao::json::traits, tao::json::internal::empty_base> *safe_find(const std::string &key) {
//    auto a = find(key);
//    if (a == nullptr)
//      throw std::runtime_error(tao::json::to_string(*this) + " does not have key " + key);
//
//    return a;
//  }
//
//  const basic_value<tao::json::traits, tao::json::internal::empty_base> *safe_find(const std::string &key) const {
//    auto a = find(key);
//    if (a == nullptr)
//      throw std::runtime_error(tao::json::to_string(*this) + " does not have key " + key);
//
//    return a;
//  }
};

}

#endif //WOLF_JSON_H
