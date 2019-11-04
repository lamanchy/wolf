#ifndef WOLF_SRC_DESERIALIZERS_LINE_H_
#define WOLF_SRC_DESERIALIZERS_LINE_H_

#include <iostream>
#include <base/plugins/base_plugin.h>

namespace wolf {
namespace from {

class line : public base_plugin {
 protected:
  void process(json &&message) override;
 private:
  std::string previous;
  std::mutex m;

};

}
}

#endif //WOLF_SRC_DESERIALIZERS_LINE_H_

