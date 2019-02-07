//
// Created by lamanchy on 8/19/18.
//

#ifndef WOLF_CONVERT_TIME_H
#define WOLF_CONVERT_TIME_H

#include <vector>
#include <string>
#include <algorithm>
#include <date/date.h>

namespace wolf {
namespace extras {

namespace {

const std::vector<std::string> parsers = {
    "%F %H:%M:%15S",
    "%FT%H:%M:%15S",
    "%FT%H:%M:%15S%Ez"
};


bool is_number(const std::string &s) {
  return !s.empty() && std::find_if(s.begin(),
                                    s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

}


std::chrono::time_point<std::chrono::system_clock> string_to_time(const std::string & input) {
  using namespace std;
  using namespace std::chrono;
  using namespace date;

  istringstream stream{input};
  sys_time<milliseconds> t;
  stream >> parse("%FT%TZ", t);
  if (not stream.fail() and stream.rdbuf()->in_avail() == 0) {
    return t;
  }
  throw runtime_error("failed to parse " + input);
}

std::uint64_t convert_time(const std::chrono::duration<uint64_t> input) {
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(input).count());
}


std::string convert_time(const std::string &input) {
  using namespace std;
  using namespace std::chrono;
  using namespace date;

  for (const auto &p: parsers) {
    istringstream stream{input};
    sys_time<milliseconds> t;
    stream >> parse(p, t);
    if (not stream.fail() and stream.rdbuf()->in_avail() == 0) {
      return format("%FT%TZ", t);
    }
  }
  throw runtime_error("failed to parse " + input);
}

std::string convert_time(const std::uint64_t &input) {
  using namespace date;
  using namespace std::chrono;

  auto zt = sys_time<milliseconds>(std::chrono::milliseconds(input));
  return format("%FT%TZ", zt);
}

std::string convert_time(const std::uint64_t &seconds, const std::uint64_t &nanos) {
  return convert_time(seconds*1000 + nanos/1000000);
}

}
}

#endif //WOLF_CONVERT_TIME_H
