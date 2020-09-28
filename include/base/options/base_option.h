#pragma once

#include <cxxopts.hpp>
#include <extras/logger.h>
#include <base/plugins/base_plugin.h>
#include <base/json.h>
#include <base/make.h>

namespace wolf {

class base_option {
 public:
  virtual void add_option(cxxopts::OptionAdder &&opts) {}
  virtual void validate_option(const cxxopts::ParseResult &opts) {}
  virtual void print_info() {};
};

template<typename T>
class option_type : public base_option {
 public:
  virtual T value(const json &event) {
    return value();
  };

  virtual T value() = 0;

 protected:
  option_type() = default;
};

template<typename T>
class constant;

template<typename T>
class option : public std::shared_ptr<option_type<T>> {
  using constructor = std::shared_ptr<option_type<T>>;
  using constructor::constructor;

 public:
  option(const T &value) : option(make<constant<T>>(value)) {}
  option(const char *value) : option(make<constant<T>>(std::string(value))) {}
};

template<typename T>
class static_option_type : public option_type<T> {};

template<typename T>
class static_option : public std::shared_ptr<static_option_type<T>> {
  using constructor = std::shared_ptr<static_option_type<T>>;
  using constructor::constructor;

 public:
  static_option(const T &value) : static_option(make<constant<T>>(value)) {}
  static_option(const char *value) : static_option(make<constant<T>>(std::string(value))) {}
};

template<typename T>
class constant : public static_option_type<T> {
 public:
  explicit constant(const T &value) : _value(value) {}

  T value() {
    return _value;
  }

 private:
  T _value;
};

}
