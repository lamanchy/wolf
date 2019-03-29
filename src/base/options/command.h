#include <utility>

//
// Created by lamanchy on 7.2.19.
//

#ifndef WOLF_COMMAND_H
#define WOLF_COMMAND_H

#include "option.h"
namespace wolf {

template<typename T>
class command : public not_event_option_type<T> {
 public:
  Logger &logger = Logger::getLogger();

  command(std::string name, std::string desc, std::string help = "") :
      name(std::move(name)),
      desc(std::move(desc)),
      help(std::move(help)) {}

  T get_value() override {
    return value;
  }

  void add_options(cxxopts::OptionAdder &&opts) override {
    opts(name, desc, cxxopts::value<T>(value), help);
  }

  void validate_options(const cxxopts::ParseResult &res) override {
    if (res.count(name) == 0 and not std::is_same<T, bool>::value) {
      throw std::runtime_error("Missing option " + name);
    }
  }

 private:
  T value;
  std::string name, desc, help;
};

}

#endif //WOLF_COMMAND_H
