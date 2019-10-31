#ifndef WOLF_PIPELINE_H
#define WOLF_PIPELINE_H

#include <csignal>
#include <thread>
#include <base/plugins/plugin.h>
#include <plugins/drop.h>
#include <base/options/event_option.h>
#include <cxxopts.hpp>
#include <base/options/command.h>

namespace wolf {

class pipeline {
 public:
  Logger &logger = Logger::getLogger();

  explicit pipeline(options opts);
  pipeline(int argc, char **argv) : pipeline(options(argc, argv)) {}

  pipeline(pipeline const &) = delete;

  pipeline &operator=(pipeline const &) = delete;

  pipeline(pipeline &&) = default;

  pipeline &operator=(pipeline &&) = delete;

  template<typename... Args>
  void register_plugin(const plugin &plugin) {
    plugins.push_back(plugin);
  }

  template<typename... Args>
  void register_plugin(const plugin &plugin, Args &&... args) {
    plugins.push_back(plugin);
    plugin->register_output(chain_plugins(args...));
  }

  const plugin &chain_plugins(const plugin &plugin) {
    return plugin;
  }

  template<typename... Args>
  const plugin &chain_plugins(const plugin &plugin, Args &&... args) {
    plugin->register_output(chain_plugins(args...));

    return plugin;
  }

  void run();

  std::string get_config_dir() {
    return config_dir;
  }

 private:
  std::vector<plugin> plugins;
  std::vector<std::thread> processors;
  unsigned number_of_processors = std::thread::hardware_concurrency();
  static bool initialized;
  std::string config_dir;
  options opts;

  void evaluate_options();
  void setup_persistency();

  template<typename T>
  std::vector<T> for_each_plugin(const std::function<T(base_plugin &)> &function);

  void for_each_plugin(const std::function<void(base_plugin &)> &function);

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
