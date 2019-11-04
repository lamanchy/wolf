#pragma once

#include <iso646.h>
#include <vector>
#include <string>
#include <algorithm>
#include <date/date.h>

namespace wolf {
namespace extras {

std::chrono::time_point<std::chrono::system_clock> string_to_time(const std::string &input);

std::uint64_t convert_time(std::chrono::duration<uint64_t> input);

std::string convert_time(const std::string &input);

std::string convert_time(const std::uint64_t &input);

std::string convert_time(const std::uint64_t &seconds, const std::uint64_t &nanos);

}
}


