#ifndef WOLF_PLUGIN_H
#define WOLF_PLUGIN_H

#include <iso646.h>
#include <utility>
#include <asio.hpp>
#include <tao/json.hpp>
#include <chrono>
#include <gzip/decompress.hpp>
#include <queue>
#include <mutex>
#include <stxxl/bits/containers/queue.h>
#include <atomic>
#include <gzip/compress.hpp>
#include <cxxopts.hpp>
#include <thread>
#include <extras/logger.h>
#include <base/json.h>
#include <base/queue.h>
#include <base/sleeper.h>

namespace wolf {
class base_plugin;
using plugin = std::shared_ptr<base_plugin>;

class base_plugin : public std::enable_shared_from_this<base_plugin> {
 public:
  using id_type = unsigned;

  Logger &logger = Logger::getLogger();

  template<typename... Args>
  plugin register_output(plugin plugin, Args &&... args) {
    return register_named_output("default", std::move(plugin), args...);
  }

  bool operator==(const base_plugin &other) const {
    return id == other.id;
  }

  bool operator<(const base_plugin &other) const {
    return id < other.id;
  }

  base_plugin(const base_plugin &) = delete;

  base_plugin(base_plugin &&other) noexcept {
    *this = std::move(other);
  };

  base_plugin &operator=(const base_plugin &) = delete;

  base_plugin &operator=(base_plugin &&other) noexcept {
    id = other.id;
    return *this;
  };

  static bool is_processor() {
    return is_thread_processor;
  }

  void buffer_output() {
    always_buffered_output = true;
  }

 protected:
  base_plugin() {
    id = id_counter++;
  }

  virtual void process(json &&message) {}

  virtual void start() {}

  virtual void stop() {}

  virtual bool is_full() {
    return false;
  }

  virtual void prepare(json &&message) {
    process(std::move(message));
  }

  void output(json &&message, bool non_blocking = false) {
    output("default", std::move(message), non_blocking);
  }

  void output(const std::string &output_type, json &&message, bool non_blocking = false) {
    outputs.at(output_type)->receive(std::move(message), non_blocking, always_buffered_output);
  }

  template<typename... Args>
  plugin register_named_output(const std::string &output_name, const plugin &plugin, Args &&... args) {
    register_named_output(output_name, plugin);
    plugin->register_output(args...);
    return shared_from_this();
  }

  plugin register_named_output(const std::string &output_name, const plugin &plugin);

  virtual bool is_running() {
    return false;
  }

 private:
  friend class pipeline;
  queue q;

  unsigned num_of_parents = 0;
  bool always_buffered_output = false;

  static thread_local bool is_thread_processor;

  bool process_buffer();

  virtual bool are_outputs_full();

  std::map<std::string, plugin> outputs;

  static std::atomic<id_type> id_counter;
  id_type id{};

  virtual void safe_prepare(json &&message);

  void receive(json &&message, bool non_blocking, bool always_buffer);

  void buffer(json &&message);

  void do_stop();

  std::vector<id_type> get_all_outputs_ids();
};

template<typename T, typename... Args>
std::shared_ptr<T> make(Args &&... args) {
  return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
}

}

#endif //WOLF_PLUGIN_H
