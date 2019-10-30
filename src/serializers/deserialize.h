//
// Created by lamanchy on 9.5.19.
//

#ifndef WOLF_DESERIALIZE_H
#define WOLF_DESERIALIZE_H

#include <base/plugins/plugin.h>
namespace wolf {

template<typename Serializer>
class deserialize : public base_plugin {
 protected:
  void process(json &&message) override {
    s.deserialize(std::move(message.get_string()), [this](json &&out) {
      output(std::move(out));
    });
  }

 private:
  Serializer s;
};

}

#endif //WOLF_DESERIALIZE_H
