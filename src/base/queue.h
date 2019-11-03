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
  static std::string get_serialized_size(size_t size);
  static size_t get_deserialized_size(std::string serialized_form);

  void do_pop(const std::function<void(json && )> &);
  void empty_front_queue();
  void check_if_all_is_empty();
  void load_from_persistent_queue();
  void try_setting_swappable();

  Logger &logger = Logger::getLogger();
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

  constexpr static unsigned string_serializer_divisor = 126;
  constexpr static char string_serializer_end = 127;
};
}

#endif //WOLF_SRC_BASE_QUEUE_H_
