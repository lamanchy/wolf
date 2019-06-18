//
// Created by lamanchy on 27.5.19.
//

#include <chrono>
#include <ctime>
#include "get_time.h"

std::string wolf::extras::get_date() {
  typedef std::chrono::system_clock Clock;

  auto now = Clock::now();
  std::time_t now_c = Clock::to_time_t(now);
  struct tm *parts = std::localtime(&now_c);

  auto year = std::to_string(1900 + parts->tm_year);
  auto month = std::to_string(1 + parts->tm_mon);
  auto day = std::to_string(parts->tm_mday);

  if (month.size() == 1) month = "0" + month;
  if (day.size() == 1) day = "0" + day;

  return year + "." + month + "." + day;
}
