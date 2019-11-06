#pragma once

#include <utility>
#include "base_option.h"
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

  T value() override {
    if (not validated)
      logger.fatal("Cannot access values before validation - pipeline initialization");
    return _value;
  }

  void print_info() override {
    std::string nam = name;
    while (nam.size() < 30) nam.push_back(' ');
    logger.info("    " + nam + value_to_string());
  }

  void add_options(cxxopts::OptionAdder &&opts) override {
    try {
      if (default_value == "") {
        opts(name, desc, cxxopts::value<T>(_value));
      } else {
        opts(name, desc, cxxopts::value<T>(_value)->default_value(default_value));
      }
    } catch (cxxopts::option_exists_error &) {
      logger.fatal("Option " + name + " cannot be registered, it already exists");
    }
  }

  void validate_options(const cxxopts::ParseResult &res) override {
    validate(res);

    if (not validator(_value))
      logger.fatal("Custom validation of option '" + name +
          "' failed, inputted value was '" + value_to_string() + "'");

    validated = true;
  }

 private:
  std::string value_to_string() {
    return value_to_string_impl(static_cast<T *>(0));
  }
  void validate(const cxxopts::ParseResult &res) {
    validate_impl(res, static_cast<T *>(0));
  }

  template<typename U>
  void validate_impl(const cxxopts::ParseResult &res, U *) {
    if (res.count(name) == 0 and default_value == "")
      logger.fatal("Missing option " + name);
    if (res.count(name) > 1)
      logger.fatal("Duplicate option " + name);
  }
  template<typename U>
  void validate_impl(const cxxopts::ParseResult &res, std::vector<U> *) {}
  void validate_impl(const cxxopts::ParseResult &res, bool *) {}

  template<typename U>
  std::string value_to_string_impl(U *) {
    std::stringstream ss;
    ss << _value;
    return ss.str();
  }
  template<typename U>
  std::string value_to_string_impl(std::vector<U> *) {
    std::stringstream ss;
    for (auto i : _value)
      ss << i << ", ";
    return ss.str();
  }
  std::string value_to_string_impl(bool *) {
    std::stringstream ss;
    ss << (_value ? "true" : "false");
    return ss.str();
  }

  Logger &logger = Logger::getLogger();
  T _value;
  std::string default_value;
  std::string name, desc;
  bool validated = false;
  std::function<bool(const T &)> validator;
};

}
