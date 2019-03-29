//
// Created by lamanchy on 8/13/18.
//

#ifndef WOLF_LINE_H
#define WOLF_LINE_H

#include <base/serializer.h>
#include <iostream>

namespace wolf {

class line : public serializer {
 public:

  std::string serialize(json &&json) override {
    return json.as<std::string>() + "\n";
  }

  void deserialize(std::string &&string, const std::function<void(json &&)> &fn) override {
    auto begin = string.begin();
    auto mark = string.begin();
    auto end = string.end();
    while (mark != end) {
      if (*mark == '\n') {
        std::string res;
        if (not previous.empty()) {
          res += previous;
          previous.clear();
        }
        res += std::string(begin, mark);
        if (not res.empty()) {
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
 private:
  std::string previous;

};

}

#endif //WOLF_LINE_H

