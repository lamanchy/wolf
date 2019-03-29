//
// Created by lamanchy on 7.2.19.
//

#ifndef WOLF_OPTION_H
#define WOLF_OPTION_H

#include <cxxopts.hpp>
#include <extras/logger.h>
#include "base/json.h"

namespace wolf {

class base_option {};

class options {
 public:
  options(int argc, char *argv[]) :
      g_opts(argv[0], " - example command line options"),
      argc(argc),
      argv(argv) {
    g_opts.add_options()("h,help", "Prints help");
    cxxopts::Options opts("");
    opts.add_options("")("h,help", "Prints help", cxxopts::value<bool>(_should_print_help));
    opts.allow_unrecognised_options();
    auto r = options::parse_opts(opts, argc, argv);
    std::cout << r.count("h,help") << std::endl;
  }

  void print_help() {
    Logger::getLogger().info(g_opts.help(g_opts.groups()));
  }

  bool should_print_help() {
    std::cout << _should_print_help << std::endl;
    return _should_print_help;
  }

  template<typename T, typename... Args>
  std::shared_ptr<T> option(Args &&... args) {
    auto opt = std::shared_ptr<T>(new T(std::forward<Args>(args)...));

    cxxopts::Options opts("");
    opt->add_options(g_opts.add_options());
    opt->add_options(opts.add_options());

    opts.allow_unrecognised_options();
    if (not should_print_help())
      opt->validate_options(options::parse_opts(opts, argc, argv));
    all_options.push_back(opt);
    return opt;
  }

 private:
  int argc;
  char **argv;
  cxxopts::Options g_opts;
  std::vector<std::shared_ptr<base_option>> all_options;
  bool _should_print_help;

  static cxxopts::ParseResult parse_opts(cxxopts::Options &opts, int argc, char **argv) {
    try {
      return opts.parse(argc, argv);
    } catch (const cxxopts::OptionException &e) {
      Logger::getLogger().error("Error parsing options: " + std::string(e.what()));
      exit(1);
    }
  }
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

  virtual void add_options(cxxopts::OptionAdder &&opts) {}
  virtual void validate_options(const cxxopts::ParseResult &opts) {}

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
