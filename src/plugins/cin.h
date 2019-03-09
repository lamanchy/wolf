//
// Created by lamanchy on 5.2.19.
//

#ifndef WOLF_CIN_H
#define WOLF_CIN_H

#include <base/plugins/threaded_plugin.h>

namespace wolf {

class cin : public threaded_plugin {
 protected:

  void run() override {
//    Plugin::is_thread_processor = true;
//    sleep(100);
//    running = false;
    for (std::string line; std::getline(std::cin, line);) {
      output(json(line));
    }
    running = false;
    }
};

}

#endif //WOLF_CIN_H
