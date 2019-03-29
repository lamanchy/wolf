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
public:
  using pointer = plugin::pointer;
  Logger & logger = Logger::getLogger();

  pipeline(int argc, char *argv[], bool persistent = true) : opts(argc, argv) {
    plugin::persistent = persistent;

    if (!initialized) {
      initialize();
    }
  }

  pipeline(pipeline const &) = delete;

  pipeline &operator=(pipeline const &) = delete;

  pipeline(pipeline &&) = default;

  pipeline &operator=(pipeline &&) = default;

  template<typename T, typename... Args>
  std::shared_ptr<T> option(Args &&... args) {
    return opts.option<T>(std::forward<Args>(args)...);
  }

  void register_plugin(const pointer &plugin) {
    plugins.push_back(plugin);
  }

  template <typename... Args>
  void register_plugin(const pointer &plugin, Args &&... args) {
    plugins.push_back(plugin);
    plugin->register_output(chain_register_output(args...));
  }

  const pointer &chain_register_output(const pointer &plugin) {
    return plugin;
  }

  template <typename... Args>
  const pointer &chain_register_output(const pointer &plugin, Args &&... args) {
    plugin->register_output(chain_register_output(args...));
    return plugin;
  }

  void run() {
    std::signal(SIGINT, catch_signal);
    evaluate_options();
    start();
    wait();
    stop();
  }


private:
  std::vector<pointer> plugins;
  std::vector<std::thread> processors;
  options opts;
  unsigned number_of_processors = std::thread::hardware_concurrency();
  static bool initialized;


  void initialize() {
    initialized = true;
    std::string path = extras::get_executable_path();

    logger.info("Configuring STXXL");
    stxxl::config *cfg = stxxl::config::get_instance();
    // create a disk_config structure.
    stxxl::disk_config disk(path + "queue.tmp", 0, path[path.size() - 1] == '/' ? "syscall" : "wincall");
    disk.autogrow = true;
    disk.unlink_on_open = true;
    disk.delete_on_exit = true;
    disk.direct = stxxl::disk_config::DIRECT_TRY;
    cfg->add_disk(disk);
  }

  void evaluate_options() {
    auto opt = opts.option<command<bool>>("help", "Prints help");

    if (opt->get_value()) {
      opts.print_help();
      exit(0);
    }
  }

  template<typename T>
  std::vector<T> for_each_plugin(const std::function<T(plugin &)>& function) {
    // TODO this must be done better, yuck
    std::set<plugin::id_type> visited;
    std::queue<pointer> to_process;
    std::queue<pointer> to_do;
    std::for_each(plugins.begin(), plugins.end(), [&to_process](pointer &ptr) { to_process.push(ptr); });
    std::vector<T> results;

    while (!to_process.empty()) {
      pointer &ptr = to_process.front();
      auto it = visited.find(ptr->id);
      if (it == visited.end()) {
        visited.insert(ptr->id);
        to_do.push(ptr);
        for (auto &pair : ptr->outputs) to_process.push(pair.second);
      }
      to_process.pop();
    }
    while (!to_do.empty()) {
      results.push_back(function(*to_do.front()));
      to_do.pop();
    }
    return results;
  }

  void for_each_plugin(const std::function<void(plugin &)>& function) {
    const std::function<int(plugin &)> fn([&function] (plugin & p) {
      function(p);
      return 0;
    });
    for_each_plugin<int>(fn);
  }

  void process() {
    plugin::is_thread_processor = true;
    while (plugins_running()) {
      for_each_plugin([](plugin &p) { while(p.process_buffer()) { }; });
//      if (!std::all_of(res.begin(), res.end(), [](bool r) { return r; }))
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      // TODO build up sleep time
//      std::this_thread::yield();
    }
    std::for_each(plugins.begin(), plugins.end(), [this](pointer &) {
      for_each_plugin([](plugin &p) { while (p.process_buffer()) { }; });
    });

    logger.info("ending processor");
  }

  static void catch_signal(int signal) {
    ++interrupt_received;

    if (interrupt_received >= 2) {
      std::quick_exit(EXIT_FAILURE);
    }
  }

  static std::atomic<int> interrupt_received;

  void start() {
    for_each_plugin([](plugin &p) { p.start(); });

    for (unsigned i = 0; i < number_of_processors; ++i) {
      processors.emplace_back(&pipeline::process, this);
    }
  }

  bool plugins_running() {
    return std::any_of(plugins.begin(), plugins.end(), [](pointer &p) { return p->is_running(); });
  }

  void wait() {
    while (plugins_running()) {
      if (interrupt_received) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }

  void stop() {
    for_each_plugin([](plugin &p) { p.stop(); });
    std::for_each(processors.begin(), processors.end(), [](std::thread &thread) { thread.join(); });
  }

};

std::atomic<int> pipeline::interrupt_received{0};
bool pipeline::initialized{false};

}

#endif //WOLF_PIPELINE_H
