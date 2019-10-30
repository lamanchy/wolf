//
// Created by lamanchy on 9.5.19.
//

#ifndef WOLF_SERIALIZE_H
#define WOLF_SERIALIZE_H

#include <base/plugins/plugin.h>
namespace wolf {

template<typename Serializer>
class serialize : public base_plugin {
 protected:
  void process(json &&message) override {
    output(json(s.serialize(std::move(message))));
  }

 private:
  Serializer s;
};

}

#endif //WOLF_SERIALIZE_H
