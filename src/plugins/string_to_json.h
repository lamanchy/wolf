//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_STRING_TO_JSON_H
#define WOLF_STRING_TO_JSON_H

#include <base/plugins/plugin.h>

namespace wolf {

class string_to_json : public plugin {
protected:
  void process(json &&message) {
    output(tao::json::from_string(message.as<std::string>()));
  }
};

}

#endif //WOLF_STRING_TO_JSON_H
