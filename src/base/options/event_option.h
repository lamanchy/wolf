#ifndef WOLF_OPTION_H
#define WOLF_OPTION_H

#include <cxxopts.hpp>
#include <extras/logger.h>
#include <base/plugins/base_plugin.h>
#include <base/json.h>
#include <base/make.h>

namespace wolf {

class base_option {
 public:
  virtual void add_options(cxxopts::OptionAdder &&opts) {}
  virtual void validate_options(const cxxopts::ParseResult &opts) {}
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

 private:
  friend class options;
};

template<typename T>
class constant;

template<typename T>
class event_option : public std::shared_ptr<option_type<T>> {
  using constructor = std::shared_ptr<option_type<T>>;
  using constructor::constructor;

 public:
  event_option(const T &value) : event_option(make<constant<T>>(value)) {}
  event_option(const char *value) : event_option(make<constant<T>>(std::string(value))) {}
};

template<typename T>
class not_event_option_type : public option_type<T> {};

template<typename T>
class option : public std::shared_ptr<not_event_option_type<T>> {
  using constructor = std::shared_ptr<not_event_option_type<T>>;
  using constructor::constructor;

 public:
  option(const T &value) : option(make<constant<T>>(value)) {}
  option(const char *value) : option(make<constant<T>>(std::string(value))) {}
};

template<typename T>
class constant : public not_event_option_type<T> {
 public:
  explicit constant(const T &value) : _value(value) {}

  T value() {
    return _value;
  }

 private:
  T _value;
};

}

#endif //WOLF_OPTION_H
