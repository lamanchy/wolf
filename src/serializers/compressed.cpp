//
// Created by lamanchy on 27.5.19.
//

#include <iso646.h>
#include <gzip/compress.hpp>
#include <gzip/decompress.hpp>
#include "compressed.h"
std::string wolf::compressed::serialize(wolf::json &&next) {
  std::string s = next.get_string();
  std::string result = gzip::compress(s.data(), s.size(), Z_BEST_SPEED);

  result = prefix + get_serialized_size(result.size()) + result;

  return result;
}
void wolf::compressed::deserialize(std::string &&next, const std::function<void(json &&)> &fn) {
  std::lock_guard<std::mutex> lg(m);
  message.append(next);
  while (true) {
    if (not is_line and not is_gzip) {
      // determine if line or gzip
      if (message.length() >= prefix.length()) {
        if (message.rfind(prefix, 0) == 0)
          is_gzip = true;
        else
          is_line = true;
      } else {
        if (message.find('\n') != std::string::npos)
          is_line = true;
      }
      if (not is_line and not is_gzip) {
        break;
      }
    }
    // is line or is gzip
    if (is_line) {
      auto pos = message.find('\n');
      if (pos == std::string::npos) break;
      fn(json(message.substr(0, pos + 1)));
      message = message.substr(pos + 1);
      is_line = false;
    }

    if (is_gzip) {
      auto pos = message.find(string_serializer_end);
      if (pos == std::string::npos) break;

      size_t size = get_deserialized_size(message.substr(prefix.length(), pos - prefix.length() + 1));

      if (message.length() < 1 + pos + size) break;

      auto test1 = gzip::decompress(message.substr(pos + 1, size).data(), size);
      fn(json(gzip::decompress(message.substr(pos + 1, size).data(), size)));
      message = message.substr(pos + size + 1);
      is_gzip = false;
    }
  }
}
std::string wolf::compressed::get_serialized_size(size_t size) {
//    return "gzip_size:" + std::to_string(size) + ";";
  std::string result;
  result.reserve(4);
  while (size > 0) {
    result.push_back((char) (size % string_serializer_divisor));
    size /= string_serializer_divisor;
  }
  result.push_back(string_serializer_end);
  return result;
}
size_t wolf::compressed::get_deserialized_size(std::string serialized_form) {
  size_t result = 0;
  size_t base = 1;
  serialized_form.pop_back();
  for (char &c : serialized_form) {
    result += c * base;
    base *= string_serializer_divisor;
  }
  return result;
}
