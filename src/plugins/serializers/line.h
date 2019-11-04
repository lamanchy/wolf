#ifndef WOLF_SRC_SERIALIZERS_LINE_H_
#define WOLF_SRC_SERIALIZERS_LINE_H_

#include <iostream>
#include <base/plugins/base_plugin.h>

namespace wolf {
namespace to {

class line : public base_plugin {
 protected:
  void process(json &&message) override {
    output(json(message.get_string() + "\n").copy_all(message));
  }
};

}
}

#endif //WOLF_SRC_SERIALIZERS_LINE_H_

