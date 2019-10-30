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
  explicit constant(const T &value) : _value(value) {}

  T value() {
    return _value;
  }

 private:
  T _value;
};

}

#endif //WOLF_CONSTANT_H
