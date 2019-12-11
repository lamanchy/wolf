#pragma once

#include <iostream>
#include <base/plugins/base_plugin.h>
#include <libs/mi_tls.h>

namespace wolf {
namespace from {

class line : public base_plugin {
 protected:
  void process(json &&message) override;
 private:
  mi_tls<std::string> previous;
};

}
}



