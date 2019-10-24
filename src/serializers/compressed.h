//
// Created by lamanchy on 9.5.19.
//

#ifndef WOLF_COMPRESSED_H
#define WOLF_COMPRESSED_H
#include <base/serializer.h>
#include <mutex>

namespace wolf {

class compressed : public serializer {
 public:
  std::string serialize(json &&next) override;
  void deserialize(std::string &&next, const std::function<void(json &&)> &fn) override;

 private:
  bool is_gzip = false;
  bool is_line = false;
  std::string message;
  constexpr static unsigned string_serializer_divisor = 126;
  constexpr static char string_serializer_end = 127;
  const std::string prefix = "gzipped:";
  std::string previous;
  std::mutex m;

  static std::string get_serialized_size(size_t size);

  static size_t get_deserialized_size(std::string serialized_form);

};

}
#endif //WOLF_COMPRESSED_H
