//
// Created by lomic on 11/4/2019.
//

#ifndef WOLF_SRC_EXTRAS_GZIP_H_
#define WOLF_SRC_EXTRAS_GZIP_H_

#include <string>
#include <gzip/compress.hpp>
#include <gzip/decompress.hpp>

namespace wolf {
namespace extras {
class gzip {
 public:
  constexpr static unsigned string_serializer_divisor = 126;
  constexpr static char string_serializer_end = 127;
  static std::string get_prefix();;

  static size_t get_deserialized_size(std::string serialized_form);
  static std::string get_serialized_size(size_t size);
};
}
}
#endif //WOLF_SRC_EXTRAS_GZIP_H_
