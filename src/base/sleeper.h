//
// Created by lomic on 10/31/2019.
//

#ifndef WOLF_SRC_BASE_SLEEPER_H_
#define WOLF_SRC_BASE_SLEEPER_H_

#include <chrono>
#include <thread>
namespace wolf {

class sleeper {
 public:
  void reset() {
    current = min;
  }

  void decrease() {
    if (current > min)
      current /= 2;
  }

  void sleep() {
    if (current > min)
      std::this_thread::sleep_for(std::chrono::microseconds(current));
    if (current < max)
      current *= 2;
  }

 private:
  unsigned min{500}, max{1000000}, current{min};

};

}

#endif //WOLF_SRC_BASE_SLEEPER_H_
