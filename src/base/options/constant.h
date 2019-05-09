//
// Created by lamanchy on 7.2.19.
//

#ifndef WOLF_CONSTANT_H
#define WOLF_CONSTANT_H

#include "option.h"
namespace wolf {

template<typename T>
class constant : public not_event_option_type<T> {
 public:
  explicit constant(const T &value) : value(value) {}

  T get_value() {
    return value;
  }

 private:
  T value;
};

}

#endif //WOLF_CONSTANT_H
