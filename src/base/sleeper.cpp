

#include "sleeper.h"
namespace wolf {

sleeper::sleeper() {
  reset();
}
void sleeper::reset() {
  current = min;
}
void sleeper::decrease_sleep_time() {
  if (current > min)
    current /= 2;
}
std::chrono::microseconds sleeper::get_sleep_time() {
  if (current > min)
    return std::chrono::microseconds(current);
  return std::chrono::microseconds(0);
}
void sleeper::increase_sleep_time() {
  if (current < max)
    current *= 2;
}
void sleeper::increasing_sleep() {
  auto sleep_time = get_sleep_time();
  if (sleep_time.count() > 0)
    sleep_for(sleep_time);
  increase_sleep_time();
}
void sleeper::sleep_for(std::chrono::microseconds duration) {
  // fastest sleep using cv.wait_for is 1 millisecond
  if (duration.count() < 10000) { // less than 10 milliseconds
    std::this_thread::sleep_for(duration);
    return;
  }

  std::unique_lock<std::mutex> lk(m);
  cv.wait_for(lk, duration, [this] { return should_wake_up; });
  should_wake_up = false;
}
void sleeper::sleep() {
  std::unique_lock<std::mutex> lk(m);
  cv.wait(lk, [this] { return should_wake_up; });
  should_wake_up = false;
}
void sleeper::wake_up() {
  {
    std::lock_guard<std::mutex> lg(m);
    should_wake_up = true;
  }
  cv.notify_one();
}
}