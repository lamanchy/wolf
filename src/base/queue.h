//
// Created by lomic on 10/31/2019.
//

#ifndef WOLF_SRC_BASE_QUEUE_H_
#define WOLF_SRC_BASE_QUEUE_H_
#include <stxxl/queue>
#include <mutex>
#include <atomic>
#include "json.h"
#include "pipeline_status.h"
namespace wolf {
class queue {
 public:
  void push(json &&message);

  void try_pop(const std::function<void(json && )> &);

  unsigned long get_size() {
    return size;
  }

  bool is_empty() {
    return get_size() == 0;
  }

  bool is_full() {
    return not pipeline_status::is_persistent() and get_size() >= pipeline_status::get_buffer_size();
  }

 private:
  void do_pop(const std::function<void(json && )> &);
  Logger &logger = Logger::getLogger();

  void empty_front_queue();

  std::queue<json> front_queue;
  std::queue<json> front_processing_queue;
  std::timed_mutex front_queue_mutex;
  std::mutex front_processing_mutex;
  std::unique_ptr<stxxl::queue<char>> persistent_queue = nullptr;
  std::mutex persistent_queue_mutex;
  std::queue<json> back_queue;
  std::queue<json> back_processing_queue;
  std::mutex back_queue_mutex;
  std::mutex back_processing_mutex;
  std::atomic<bool> swappable{true};

  std::vector<char> tmp_front_buffer1;
  std::string tmp_front_buffer2;

  std::vector<char> tmp_back_buffer1;
  std::atomic<unsigned long> size{0};

//  json get_buffer_stats() {
//    std::lock_guard<std::mutex> bqlg(back_queue_mutex);
//    std::lock_guard<std::timed_mutex> fqlg(front_queue_mutex);
//    std::lock_guard<std::mutex> pqlg(persistent_queue_mutex);
//    return {
//        {"front_queue_size", front_queue.size()},
//        {"back_queue_size", back_queue.size()},
//        {"persistent_queue_size", persistent_queue->size()}
//    };
//  }

  constexpr static unsigned string_serializer_divisor = 126;
  constexpr static char string_serializer_end = 127;

  static std::string get_serialized_size(size_t size);

  static size_t get_deserialized_size(std::string serialized_form);

  void check_if_all_is_empty();
  void load_from_persistent_queue();
  void try_setting_swappable();
};
}

#endif //WOLF_SRC_BASE_QUEUE_H_
