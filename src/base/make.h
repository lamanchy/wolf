#pragma once

#include <memory>

namespace wolf {

template<typename T, typename... Args>
std::shared_ptr<T> make(Args &&... args) {
  return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
}

}


