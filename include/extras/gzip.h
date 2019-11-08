#pragma once

#include <string>

#pragma warning( push )
#pragma warning( disable : 4068 )
#include <gzip/compress.hpp>
#include <gzip/decompress.hpp>
#pragma warning( pop )

namespace wolf {
namespace extras {
class gzip {
 public:
  constexpr static unsigned string_serializer_divisor = 126;
  constexpr static char string_serializer_end = 127;
  static std::string get_prefix();

  static size_t get_deserialized_size(std::string serialized_form);
  static std::string get_serialized_size(size_t size);
};
}
}

