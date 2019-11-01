//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_STRING_TO_JSON_H
#define WOLF_STRING_TO_JSON_H

#include <base/plugins/plugin.h>

namespace wolf {

class string_to_json : public base_plugin {
 protected:
  void process(json &&message) {
    auto res = json(tao::json::from_string(message.as<std::string>()));
    res.metadata = message.metadata;
    res.size = message.size;
    output(std::move(res));
  }
};

}

#endif //WOLF_STRING_TO_JSON_H
