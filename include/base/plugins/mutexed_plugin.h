#pragma once

#include <base/plugins/base_plugin.h>

namespace wolf {

class mutexed_plugin : public base_plugin {
 public:
  mutexed_plugin() {
    processors_should_prefer_buffering();
  }
 protected:
  void prepare(json &&message) override;

 private:
  std::mutex lock;
};

}


