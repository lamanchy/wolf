#include <utility>

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

  command(std::string name, std::string desc, std::string help = "", std::string default_value = "") :
      name(std::move(name)),
      desc(std::move(desc)),
      help(std::move(help)),
      default_value(std::move(default_value)) {}

  T get_value() override {
    return value;
  }

  void add_options(cxxopts::OptionAdder &&opts) override {
    if (default_value == "") {
      opts(name, desc, cxxopts::value<T>(value), help);
    } else {
      opts(name, desc, cxxopts::value<T>(value)->default_value(default_value), help);
    }
  }

  void validate_options(const cxxopts::ParseResult &res) override {
    if (res.count(name) == 0 and not std::is_same<T, bool>::value and default_value == "") {
      throw std::runtime_error("Missing option " + name);
    }
  }

 private:
  T value;
  std::string default_value;
  std::string name, desc, help;
};

}

#endif //WOLF_COMMAND_H
