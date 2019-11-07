#pragma once

#include <mutex>
#include <base/plugins/base_plugin.h>
#include <plugins/deserializers/compressed.h>

namespace wolf {
namespace to {

class compressed : public base_plugin {
 protected:
  void process(json &&message) override;
};

}
}

