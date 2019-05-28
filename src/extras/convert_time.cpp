//
// Created by lamanchy on 27.5.19.
//

#include "convert_time.h"

namespace {

const std::vector<std::string> parsers = {
    "%F %H:%M:%15S",
    "%FT%H:%M:%15S",
    "%FT%H:%M:%15S%Ez"
};

bool is_number(const std::string &s) {
  return not s.empty() and std::find_if(
      s.begin(),
      s.end(),
      [](char c) { return not std::isdigit(c); }
  ) == s.end();
}

}

std::chrono::time_point<std::chrono::system_clock> wolf::extras::string_to_time(const std::string &input) {
  using namespace std;
  using namespace std::chrono;
  using namespace date;

  istringstream stream{input};
  sys_time<milliseconds> t;
  stream >> parse("%FT%TZ", t);
  if (!stream.fail() && stream.rdbuf()->in_avail() == 0) {
    return t;
  }
  throw runtime_error("failed to parse " + input);
}
std::uint64_t wolf::extras::convert_time(const std::chrono::duration<uint64_t> input) {
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(input).count());
}
std::string wolf::extras::convert_time(const std::string &input) {
  using namespace std;
  using namespace std::chrono;
  using namespace date;

  for (const auto &p: parsers) {
    istringstream stream{input};
    sys_time<milliseconds> t;
    stream >> parse(p, t);
    if (!stream.fail() && stream.rdbuf()->in_avail() == 0) {
      return format("%FT%TZ", t);
    }
  }
  throw runtime_error("failed to parse " + input);
}
std::string wolf::extras::convert_time(const std::uint64_t &input) {
  using namespace date;
  using namespace std::chrono;

  auto zt = sys_time<milliseconds>(std::chrono::milliseconds(input));
  return format("%FT%TZ", zt);
}
std::string wolf::extras::convert_time(const std::uint64_t &seconds, const std::uint64_t &nanos) {
  return convert_time(seconds * 1000 + nanos / 1000000);
}
