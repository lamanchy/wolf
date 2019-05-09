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
  void deserialize(std::string &&string, const std::function<void(json && )> &fn) override {
    fn(json(string));
  }
};

}

#endif //WOLF_PLAIN_H
