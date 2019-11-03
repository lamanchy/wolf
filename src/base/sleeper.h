//
// Created by lomic on 10/31/2019.
//

#ifndef WOLF_SRC_BASE_SLEEPER_H_
#define WOLF_SRC_BASE_SLEEPER_H_

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
namespace wolf {

class sleeper {
 public:
  sleeper ();

  void sleep();
  void increasing_sleep();
  void sleep_for(std::chrono::microseconds duration);
  void wake_up();

  std::chrono::microseconds get_sleep_time();
  void increase_sleep_time();
  void decrease_sleep_time();
  void reset();

 private:
  std::mutex m;
  std::condition_variable cv;
  bool should_wake_up = false;
  unsigned min{500}, max{1000000}, current{min};
};

}

#endif //WOLF_SRC_BASE_SLEEPER_H_
