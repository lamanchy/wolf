#pragma once

#include <base/plugins/base_plugin.h>
namespace wolf {

class copy : public base_plugin {

 public:
  template<typename... Args>
  plugin register_copy_output(plugin plugin, Args &&... args) {
    return register_named_output("copy", std::move(plugin), args...);
  }

 protected:
  void process(json &&message) override {
    json copy(message);
    output(std::move(message));
    copy_output(std::move(copy));
  }

  void copy_output(json &&message) {
    output("copy", std::move(message));
  }
};

}


