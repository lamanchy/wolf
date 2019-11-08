#pragma once

#include <base/plugins/base_plugin.h>
namespace wolf {

class drop : public base_plugin {
 protected:
  void process(json &&message) override {}
};

}


