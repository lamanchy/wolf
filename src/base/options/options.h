//
// Created by lomic on 11/2/2019.
//

#ifndef WOLF_SRC_BASE_OPTIONS_OPTIONS_H_
#define WOLF_SRC_BASE_OPTIONS_OPTIONS_H_

#include <string>
#include <memory>
#include <cxxopts.hpp>
#include <vector>
#include "event_option.h"

namespace wolf {

class options {
 public:
  options(int argc, char *argv[]);

  void print_help();

  template<typename T, typename... Args>
  std::shared_ptr<T> add(Args &&... args) {
    return add_grouped<T>("", std::forward<Args>(args)...);
  }

  template<typename T, typename... Args>
  std::shared_ptr<T> add_grouped(const std::string &group, Args &&... args) {
    if (parsed)
      logger.fatal("Cannot add options after pipeline creation");

    auto opt = std::shared_ptr<T>(new T(std::forward<Args>(args)...));

    opt->add_options(g_opts.add_options(group));
    all_options.push_back(opt);
    return opt;
  }

  void parse_options();
  void print_options();

  static const std::string general_config_group_name;

 private:
  int argc;
  char **argv;
  cxxopts::Options g_opts;
  std::vector<std::shared_ptr<base_option>> all_options;
  Logger &logger = Logger::getLogger();
  bool parsed = false;
};

}

#endif //WOLF_SRC_BASE_OPTIONS_OPTIONS_H_
