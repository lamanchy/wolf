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

namespace wolf {

class plugin : public std::enable_shared_from_this<plugin> {
 public:
  using pointer = std::shared_ptr<plugin>;
  using id_type = unsigned;

  Logger &logger = Logger::getLogger();

  pointer register_output() {
    return shared_from_this();
  }

  template<typename... Args>
  pointer register_output(pointer plugin, Args &&... args) {
    return register_named_output("default", std::move(plugin), args...);
  }

  bool operator==(const plugin &other) const {
    return id == other.id;
  }

  bool operator<(const plugin &other) const {
    return id < other.id;
  }

  plugin(const plugin &) = delete;

  plugin(plugin &&other) {
    *this = std::move(other);
  };

  plugin &operator=(const plugin &) = delete;

  plugin &operator=(plugin &&other) {
    id = other.id;
    return *this;
  };

  void mark_as_processor() {
    plugin::is_thread_processor = true;
  }

 protected:
  plugin() {
    id = id_counter++;
  }

  virtual void process(json &&message) {}

  virtual void safe_prepare(json &&message);

  virtual void prepare(json &&message) {
    process(std::move(message));
  }

  virtual bool is_running() {
    return false;
  }

  virtual void start() {}

  virtual void stop() {}

  virtual bool is_full() {
    return false;
  }

  virtual bool are_outputs_full();

  void output(json &&message) {
    output("default", std::move(message));
  }

  virtual void output(const std::string &output_type, json &&message) {
    outputs.at(output_type)->receive(std::move(message));
  }

  template<typename... Args>
  pointer register_named_output(std::string output_name, plugin::pointer plugin, Args &&... args) {
    auto it = outputs.find(output_name);
    if (it != outputs.end()) throw std::runtime_error("plugin already registered output named: " + output_name);
    outputs.emplace(std::make_pair(output_name, plugin));

    plugin->register_output(args...);

    return shared_from_this();
  }

  static unsigned buffer_size;
 private:
  friend class pipeline;

  static thread_local bool is_thread_processor;
  static bool persistent;

  void process_back_queue_front();

  bool process_buffer();

  void empty_front_queue();

  std::map<std::string, pointer> outputs;
  std::queue<json> front_queue;
  std::queue<json> front_processing_queue;
  std::timed_mutex front_queue_mutex;
  std::mutex front_processing_mutex;
  stxxl::queue<char> persistent_queue;
//  std::queue<char> persistent_queue;
  std::mutex persistent_queue_mutex;
  std::queue<json> back_queue;
  std::queue<json> back_processing_queue;
  std::mutex back_queue_mutex;
  std::mutex back_processing_mutex;

  std::atomic<bool> swappable{true};

  std::vector<char> tmp_front_buffer1;
  std::string tmp_front_buffer2;

  std::vector<char> tmp_back_buffer1;

  static std::atomic<id_type> id_counter;
  id_type id;

//  json get_buffer_stats() {
//    std::lock_guard<std::mutex> bqlg(back_queue_mutex);
//    std::lock_guard<std::timed_mutex> fqlg(front_queue_mutex);
//    std::lock_guard<std::mutex> pqlg(persistent_queue_mutex);
//    return {
//        {"front_queue_size", front_queue.size()},
//        {"back_queue_size", back_queue.size()},
//        {"persistent_queue_size", persistent_queue.size()}
//    };
//  }

  void receive(json &&message);

  void buffer(json &&message);

  constexpr static unsigned string_serializer_divisor = 126;
  constexpr static char string_serializer_end = 127;

  static std::string get_serialized_size(size_t size);

  static size_t get_deserialized_size(std::string serialized_form);

};

template<typename T, typename... Args>
std::shared_ptr<T> make(Args &&... args) {
  return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
}

}

#endif //WOLF_PLUGIN_H
