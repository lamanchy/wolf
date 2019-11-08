#pragma once

#include <csignal>
#include <thread>
#include <base/plugins/base_plugin.h>
#include <plugins/drop.h>
#include <base/options/base_option.h>
#include <cxxopts.hpp>
#include <base/options/command.h>
#include <base/options/options.h>
#include "sleeper.h"

namespace wolf {

class pipeline {
 public:
  static std::atomic<int> interrupt_received;
  Logger &logger = Logger::getLogger();

  explicit pipeline(options opts);
  pipeline(int argc, char **argv) : pipeline(options(argc, argv)) {}

  pipeline(pipeline const &) = delete;

  pipeline &operator=(pipeline const &) = delete;

  pipeline(pipeline &&) = default;

  pipeline &operator=(pipeline &&) = delete;

  void register_plugin(const plugin &plugin) {
    plugins.push_back(plugin);
  }

  template<typename... Args>
  void register_plugin(const plugin &plugin, Args &&... args) {
    plugins.push_back(plugin);
    plugin->register_output(chain_plugins(args...));
  }

  static const plugin &chain_plugins(const plugin &plugin) {
    return plugin;
  }

  template<typename... Args>
  static const plugin &chain_plugins(const plugin &plugin, Args &&... args) {
    plugin->register_output(chain_plugins(args...));

    return plugin;
  }

  void run();

  std::string get_config_dir() {
    return config_dir;
  }

 private:
  static void catch_signal(int signal);

  void evaluate_options();
  void setup_persistency();

  template<typename T>
  std::vector<T> for_each_plugin(const std::function<T(base_plugin &)> &function);
  void for_each_plugin(const std::function<void(base_plugin &)> &function);

  void process(int i);

  void start();
  void wait();
  bool plugins_running();
  void stop();

  std::vector<plugin> plugins;
  std::vector<std::thread> processors;
  std::unique_ptr<sleeper[]> processors_sleepers;
  unsigned number_of_processors = std::thread::hardware_concurrency();
  std::string config_dir;
  options opts;
};

}


