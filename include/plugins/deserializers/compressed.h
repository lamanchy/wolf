#pragma once

#include <base/plugins/base_plugin.h>
#include <iso646.h>
#include <extras/gzip.h>
#include <libs/mi_tls.h>

namespace wolf {
namespace from {

class compressed : public base_plugin {
 protected:
  void process(json &&message) override;

 private:
  mi_tls<std::string> message;
};

}
}

