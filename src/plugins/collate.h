//
// Created by lamanchy on 8/15/18.
//

#ifndef WOLF_COLLATE_H
#define WOLF_COLLATE_H

#include <base/mutexed_plugin.h>


// TODO not finished not finished not finished not finished not finished not finished not finished

namespace wolf {

class collate : public mutexed_plugin {
protected:
  void process(json &&message) override {
    buffer += message.as<std::string>();
    count += 1;
    if (count > 1000) {
      output(json(std::move(buffer)));
      buffer.clear();
      count = 0;
    }
  }
private:
  std::string buffer;
  unsigned int count = 0;

};

}

#endif //WOLF_COLLATE_H
