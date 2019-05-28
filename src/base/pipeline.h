#ifndef WOLF_PIPELINE_H
#define WOLF_PIPELINE_H

#include <csignal>
#include <thread>
#include "base/plugins/plugin.h"
#include "base/options/option.h"
#include <cxxopts.hpp>
#include <base/options/constant.h>
#include <base/options/command.h>

namespace wolf {

class pipeline {
 private:
  options opts;

 public:
  using pointer = plugin::pointer;
  Logger &logger;

  pipeline(int argc, char *argv[], bool persistent = true);

  pipeline(pipeline const &) = delete;

  pipeline &operator=(pipeline const &) = delete;

  pipeline(pipeline &&) = default;

  pipeline &operator=(pipeline &&) = delete;

  template<typename T, typename... Args>
  std::shared_ptr<T> option(Args &&... args) {
    return opts.option<T>(std::forward<Args>(args)...);
  }

  template<typename... Args>
  void register_plugin(const pointer &plugin) {
    plugins.push_back(plugin);
  }

  template<typename... Args>
  void register_plugin(const pointer &plugin, Args &&... args) {
    plugins.push_back(plugin);
    plugin->register_output(chain_register_output(args...));
  }

  const pointer &chain_register_output(const pointer &plugin) {
    return plugin;
  }

  template<typename... Args>
  const pointer &chain_register_output(const pointer &plugin, Args &&... args) {
    plugin->register_output(chain_register_output(args...));

    return plugin;
  }

  void run();

  bool will_print_help() {
    return opts.should_print_help();
  }

  std::string get_config_dir() {
    return config_dir;
  }

 private:
  std::vector<pointer> plugins;
  std::vector<std::thread> processors;
  unsigned number_of_processors = std::thread::hardware_concurrency();
  static bool initialized;
  static std::string config_dir;

  void initialize();

  void evaluate_options() {
    if (opts.should_print_help()) {
      opts.print_help();
      exit(0);
    }
  }

  template<typename T>
  std::vector<T> for_each_plugin(const std::function<T(plugin &)> &function);

  void for_each_plugin(const std::function<void(plugin &)> &function);

  void process();

  static void catch_signal(int signal);

  static std::atomic<int> interrupt_received;

  void start();

  bool plugins_running();

  void wait();

  void stop();

};

}

#endif //WOLF_PIPELINE_H
