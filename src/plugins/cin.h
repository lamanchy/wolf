//
// Created by lamanchy on 5.2.19.
//

#ifndef WOLF_CIN_H
#define WOLF_CIN_H

#include <base/plugins/threaded_plugin.h>

namespace wolf {

class cin : public threaded_plugin {
 protected:
  void setup() override {
    for (std::string line; std::getline(std::cin, line);) {
      output(json(line));
    }
    end_loop();
  }
};

}

#endif //WOLF_CIN_H
