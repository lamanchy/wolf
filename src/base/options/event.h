#include <utility>

//
// Created by lamanchy on 7.2.19.
//

#ifndef WOLF_EVENT_H
#define WOLF_EVENT_H

#include "option.h"
namespace wolf {


template<typename T>
class event : public option_type<T> {
 public:
  explicit event(std::string field) : field(std::move(field)) { }

  T get_value(const json &event) override {
    return event.find(field)->as<T>();
  }

  T get_value() override {
    throw std::runtime_error("Cannot get value of event option without event specified");
  }

 private:
  std::string field;

};


}

#endif //WOLF_EVENT_H
