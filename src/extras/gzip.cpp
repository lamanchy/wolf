

#include "gzip.h"

namespace wolf {
namespace extras {
size_t gzip::get_deserialized_size(std::string serialized_form) {
  size_t result = 0;
  size_t base = 1;
  serialized_form.pop_back();
  for (char &c : serialized_form) {
    result += c * base;
    base *= string_serializer_divisor;
  }
  return result;
}

std::string gzip::get_serialized_size(size_t size) {
  std::string result;
  result.reserve(4);
  while (size > 0) {
    result.push_back((char) (size % string_serializer_divisor));
    size /= string_serializer_divisor;
  }
  result.push_back(string_serializer_end);
  return result;
}
std::string gzip::get_prefix() {
  return "gzipped:";
}
}
}