#pragma once

#include <tao/json.hpp>
#include <base/plugins/base_plugin.h>

namespace wolf {
namespace from {
class string : public base_plugin {
 protected:
  void process(json &&message) override {
    output(json(tao::json::from_string(message.get_string())).copy_all(message));
  }
};

}
}


