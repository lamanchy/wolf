
#include <chrono>
#include <ctime>
#include <extras/get_time.h>

std::string wolf::extras::get_date() {
  typedef std::chrono::system_clock Clock;

  auto now = Clock::now();
  std::time_t now_c = Clock::to_time_t(now);

#pragma warning( push )
#pragma warning( disable : 4996 )

  struct tm *parts = localtime(&now_c);

#pragma warning( pop )

  auto year = std::to_string(1900 + parts->tm_year);
  auto month = std::to_string(1 + parts->tm_mon);
  auto day = std::to_string(parts->tm_mday);

  if (month.size() == 1) month = "0" + month;
  if (day.size() == 1) day = "0" + day;

  return year + "." + month + "." + day;
}
