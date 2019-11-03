#include <utility>

//
// Created by lamanchy on 7.2.19.
//

#ifndef WOLF_EVENT_H
#define WOLF_EVENT_H

#include "base_option.h"
namespace wolf {

template<typename T>
class event : public option_type<T> {
 public:
  explicit event(std::string field, bool metadata = false) : field(std::move(field)), metadata(metadata) {}

  T value(const json &event) override {
    if (metadata)
      return event.metadata.find(field)->as<T>();

    return event.find(field)->as<T>();
  }

  T value() override {
    throw std::runtime_error("Cannot get value of event option without event specified");
  }

 private:
  std::string field;
  bool metadata;

};

}

#endif //WOLF_EVENT_H
