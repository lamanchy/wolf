//
// Created by lamanchy on 9.5.19.
//

#ifndef WOLF_COMPRESS_H
#define WOLF_COMPRESS_H

#include <base/plugins/plugin.h>
#include <tao/json/cbor/to_string.hpp>
namespace wolf {

class compres : public plugin {

 protected:
  void process(json &&message) override {
    std::string s = message.get_string();
    std::string result;
    for (char c: get_serialized_size(s.size())) result.push_back(c);
    result.append(gzip::compress(s.data(), s.size(), Z_BEST_SPEED));

    output(result);
  }

 private:
  constexpr static unsigned string_serializer_divisor = 100;
  constexpr static char string_serializer_end = 101;

  std::string get_serialized_size(size_t size) {
    return "gzip_size:" + std::to_string(size) + ";";
//    std::string result;
//    result.reserve(4);
//    while (size > 0) {
//      result.push_back((char) (size % string_serializer_divisor));
//      size /= string_serializer_divisor;
//    }
//    result.push_back(string_serializer_end);
//    return result;
  }

  size_t get_deserialized_size(std::string serialized_form) {
    size_t result = 0;
    size_t base = 1;
    serialized_form.pop_back();
    for (char &c : serialized_form) {
      result += c * base;
      base *= string_serializer_divisor;
    }
    return result;
  }

};
}

#endif //WOLF_COMPRESS_H
