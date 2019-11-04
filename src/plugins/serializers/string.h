//
// Created by lomic on 11/3/2019.
//

#ifndef WOLF_SRC_SERIALIZERS_STRING_H_
#define WOLF_SRC_SERIALIZERS_STRING_H_

#include <tao/json.hpp>
#include <base/plugins/base_plugin.h>

namespace wolf {
namespace to {
class string : public base_plugin {
 protected:
  void process(json &&message) override {
    output(json(tao::json::to_string(message)).copy_all(message));
  }
};

}
}

#endif //WOLF_SRC_SERIALIZERS_STRING_H_
