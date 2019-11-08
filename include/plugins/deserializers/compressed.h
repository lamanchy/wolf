#pragma once

#include <mutex>
#include <base/plugins/base_plugin.h>
#include <iso646.h>
#include <extras/gzip.h>

namespace wolf {
namespace from {

class compressed : public base_plugin {
 protected:
  void process(json &&message) override;

 private:
  bool is_gzip = false;
  bool is_line = false;
  std::string message;
  std::mutex m;
};

}
}

