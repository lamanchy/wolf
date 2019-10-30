//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_JSON_TO_STRING_H
#define WOLF_JSON_TO_STRING_H

#include <base/plugins/plugin.h>

namespace wolf {

class json_to_string : public base_plugin {
 public:
  json_to_string(const option<bool>& add_newline = false) : add_newline(add_newline->value()) {}

 protected:
  void process(json &&message) override {
    auto res = json(tao::json::to_string(message) + (add_newline ? "\n" : ""));
    res.metadata = message.metadata;
    output(std::move(res));
  }

 private:
  bool add_newline;
};

}

#endif //WOLF_JSON_TO_STRING_H
