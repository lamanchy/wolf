

#include <extras/convert_time.h>
#include <array>

namespace {

const std::array<const std::string, 3> parsers{
    "%FT%H:%M:%10S",
    "%FT%H:%M:%10S%Ez",
    "%F %H:%M:%10S"
};

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

std::string wolf::extras::utc_time(const std::string &input, const std::string &parser) {
  using namespace std;
  using namespace std::chrono;
  using namespace date;

  istringstream stream{input};
  sys_time<milliseconds> t;
  stream >> parse(parser, t);
  if (!stream.fail() && stream.rdbuf()->in_avail() == 0) {
    return format("%FT%TZ", t);
  }

  // backup parsers
  for (const auto &p: parsers) {
    stream.str(input);
    stream.clear();
    stream >> parse(p, t);
    if (!stream.fail() && stream.rdbuf()->in_avail() == 0) {
      return format("%FT%TZ", t);
    }
  }
  throw runtime_error("failed to parse " + input);
}
std::string wolf::extras::utc_time(const std::uint64_t &input) {
  using namespace date;
  using namespace std::chrono;

  auto zt = sys_time<milliseconds>(std::chrono::milliseconds(input));
  return format("%FT%TZ", zt);
}
std::string wolf::extras::utc_time(const std::uint64_t &seconds, const std::uint64_t &nanos) {
  return utc_time(seconds * 1000 + nanos / 1000000);
}
