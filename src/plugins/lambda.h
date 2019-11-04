#pragma once

#include <base/plugins/base_plugin.h>

namespace wolf {
class lambda : public base_plugin {
 public:
  lambda(std::function<void(json & )> fn) : fn(std::move(fn)) {}

 protected:
  void process(json &&message) override {
    fn(message);
    output(std::move(message));
  }

 private:
  std::function<void(json & )> fn;
};
}


