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
  command(std::string name, std::string desc, std::string default_value = "",
          std::function<bool(const T &)> validator = [](const T &) { return true; }) :
      name(std::move(name)),
      desc(std::move(desc)),
      validator(std::move(validator)),
      default_value(std::move(default_value)) {}

  T get_value() override {
    if (not validated)
      logger.fatal("Cannot access values before validation - pipeline initialization");
    return value;
  }

  std::string value_to_string() {
    std::stringstream ss;
    ss << value;
    return ss.str();
  }

  void print_info() override {

    std::string nam = name;
    while (nam.size() < 30) nam.push_back(' ');
    logger.info("    " + nam + value_to_string());
  }

  void add_options(cxxopts::OptionAdder &&opts) override {
    try {
      if (default_value == "") {
        opts(name, desc, cxxopts::value<T>(value));
      } else {
        opts(name, desc, cxxopts::value<T>(value)->default_value(default_value));
      }
    } catch (cxxopts::option_exists_error &ex) {
      logger.fatal("Option " + name + " cannot be registered, it already exists");
    }
  }

  void validate_options(const cxxopts::ParseResult &res) override {
    if (res.count(name) == 0 and not std::is_same<T, bool>::value and default_value == "")
      logger.fatal("Missing option " + name);

    if (not validator(value))
      logger.fatal("Custom validation of option '" + name +
          "' failed, inputted value was '" + value_to_string() + "'");

    validated = true;
  }

 private:
  Logger &logger = Logger::getLogger();
  T value;
  std::string default_value;
  std::string name, desc;
  bool validated = false;
  std::function<bool(const T &)> validator;
};

}

#endif //WOLF_COMMAND_H
