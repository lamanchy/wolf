//
// Created by lamanchy on 28.5.19.
//

#include <mutex>
#include "line.h"
void wolf::line::deserialize(std::string &&string, const std::function<void(json &&)> &fn) {
  std::lock_guard<std::mutex> lg(m);
  auto begin = string.begin();
  auto mark = string.begin();
  auto end = string.end();
  while (mark != end) {
    if (*mark == '\n') {
      std::string res;
      if (!previous.empty()) {
        res += previous;
        previous.clear();
      }
      res += std::string(begin, mark);
      if (!res.empty()) {
        fn(json(res));
      }
      begin = ++mark;
    } else {
      ++mark;
    }
  }
  if (begin != end) {
    previous += std::string(begin, end);
  }
}
