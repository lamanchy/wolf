//
// Created by lamanchy on 17.12.18.
//

#ifndef WOLF_GET_TIME_H
#define WOLF_GET_TIME_H

#include <string>
#include <chrono>
namespace wolf {
namespace extras {

std::string get_date() {
  typedef std::chrono::system_clock Clock;

  auto now = Clock::now();
  std::time_t now_c = Clock::to_time_t(now);
  struct tm *parts = std::localtime(&now_c);

  return std::to_string(1900 + parts->tm_year) + "." +
      std::to_string(1 + parts->tm_mon) + "." +
      std::to_string(parts->tm_mday);
}

}
}

#endif //WOLF_GET_TIME_H
