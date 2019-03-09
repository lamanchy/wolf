//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_JSON_TO_STRING_H
#define WOLF_JSON_TO_STRING_H

#include <base/plugins/plugin.h>

namespace wolf {

class json_to_string : public plugin {
protected:
  void process(json &&message) override {
    output(json(tao::json::to_string(message)));
  }
};

}

#endif //WOLF_JSON_TO_STRING_H
