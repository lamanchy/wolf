//
// Created by lomic on 11/3/2019.
//

#ifndef WOLF_SRC_BASE_MAKE_H_
#define WOLF_SRC_BASE_MAKE_H_

#include <memory>

namespace wolf {

template<typename T, typename... Args>
std::shared_ptr<T> make(Args &&... args) {
  return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
}

}

#endif //WOLF_SRC_BASE_MAKE_H_
