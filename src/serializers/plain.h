//
// Created by lamanchy on 22.3.19.
//

#ifndef WOLF_PLAIN_H
#define WOLF_PLAIN_H

#include <base/serializer.h>
namespace wolf {

class plain : public serializer {
 public:
  std::string serialize(json &&json) override {
    return json.get_string();
  }
  void deserialize(std::string &&string, const std::function<void(json &&)> &fn) override {
    // this should convert "{1}{2}{3}{4" into three json and wait for the fourth one
    throw std::runtime_error("Not implemented");
  }
};

}


#endif //WOLF_PLAIN_H
