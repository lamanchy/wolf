//
// Created by lamanchy on 27.5.19.
//

#include "get_time.h"
std::string wolf::extras::get_date() {
  typedef std::chrono::system_clock Clock;

  auto now = Clock::now();
  std::time_t now_c = Clock::to_time_t(now);
  struct tm *parts = std::localtime(&now_c);

  return std::to_string(1900 + parts->tm_year) + "." +
      std::to_string(1 + parts->tm_mon) + "." +
      std::to_string(parts->tm_mday);
}
