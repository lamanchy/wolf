#include <utility>

#include <utility>

//
// Created by lamanchy on 3.5.19.
//

#ifndef WOLF_IF_H
#define WOLF_IF_H

#include <base/plugins/base_plugin.h>
namespace wolf {

class filter : public base_plugin {
 public:
  filter(std::function<bool(const json &)> condition) :
      condition(std::move(condition)) {}

  template<typename... Args>
  plugin filtered(plugin plugin, Args &&... args) {
    return register_named_output("filtered", std::move(plugin), args...);
  }

 protected:
  void process(json &&message) override {
    if (condition(message)) {
      output("filtered", std::move(message));
    } else {
      output(std::move(message));
    }
  }

 private:
  const std::function<bool(const json &)> condition;
};

}

#endif //WOLF_IF_H
