//
// Created by lamanchy on 7.2.19.
//

#ifndef WOLF_OPTION_H
#define WOLF_OPTION_H

#include <cxxopts.hpp>
#include <extras/logger.h>
#include "base/json.h"

namespace wolf {

class base_option {
 public:
  virtual void add_options(cxxopts::OptionAdder &&opts) {}
  virtual void validate_options(const cxxopts::ParseResult &opts) {}
  virtual void print_info() {};
};

class options {
 public:
  options(int argc, char *argv[]);

  void print_help();

  template<typename T, typename... Args>
  std::shared_ptr<T> add(Args &&... args) {
    return add_named<T>("", std::forward<Args>(args)...);
  }

  template<typename T, typename... Args>
  std::shared_ptr<T> add_named(const std::string & group, Args &&... args) {
    if (parsed)
      logger.fatal("Cannot add options after pipeline creation");

    auto opt = std::shared_ptr<T>(new T(std::forward<Args>(args)...));

    opt->add_options(g_opts.add_options(group));
    all_options.push_back(opt);
    return opt;
  }

  void parse_options();
  void print_options();

  static std::string general_config_group_name;

 private:
  int argc;
  char **argv;
  cxxopts::Options g_opts;
  std::vector<std::shared_ptr<base_option>> all_options;
  Logger &logger = Logger::getLogger();
  bool parsed = false;
};

template<typename T>
class option_type : public base_option {
 public:
  virtual T get_value(const json &event) {
    return get_value();
  };

  virtual T get_value() = 0;

 protected:
  option_type() = default;

 private:
  friend class options;
};

template<typename T>
using option = std::shared_ptr<option_type<T>>;

template<typename T>
class not_event_option_type : public option_type<T> {};

template<typename T>
using not_event_option = std::shared_ptr<not_event_option_type<T>>;

}

#endif //WOLF_OPTION_H
