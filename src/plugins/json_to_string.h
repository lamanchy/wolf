//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_JSON_TO_STRING_H
#define WOLF_JSON_TO_STRING_H

#include <base/plugin.h>

namespace wolf {

class json_to_string : public plugin {
public:
  void print_name() override {
    std::cout << "jts" << std::endl;
  }

protected:
  void process(json &&message) override {
    output(json(tao::json::to_string(message)));
  }
};

}

#endif //WOLF_JSON_TO_STRING_H
