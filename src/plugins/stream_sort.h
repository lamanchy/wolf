#include <utility>

#include <utility>

//
// Created by lamanchy on 17.12.18.
//

#ifndef WOLF_STREAM_SORT_H
#define WOLF_STREAM_SORT_H

#include <base/mutexed_threaded_plugin.h>
namespace wolf {

class stream_sort : public mutexed_threaded_plugin {
 public:

  template<typename T>
  static std::function<bool(json &)> ready_after(std::chrono::duration<T> duration) {
    return [duration](json & top) -> bool {
      return (std::chrono::system_clock::now() - top.metadata["stream_sort_inserted_time"].as<std::chrono::time_point<std::chrono::system_clock>>()) > duration;
    };
  }
 protected:

  using priority_queue_t = std::priority_queue<
      json,
      std::deque<json>,
      std::function<bool(const json &lhs, const json &rhs)>
  >;

  stream_sort(
      const std::function<bool(const json &lhs, const json &rhs)> &cmp,
      std::function<bool(const json &)> is_ready
  ) : priority_queue(priority_queue_t(cmp)), is_ready(std::move(is_ready)) { }

  void run() override {
    mark_as_processor();
    while (running) {
      lock.lock();
      while (!priority_queue.empty() && is_ready(priority_queue.top())) {
        // TODO can it be std::moved outside? (can, but with custom implementation)
        json j = priority_queue.top();
        priority_queue.pop();
        lock.unlock();
        output(std::move(j));
        lock.lock();
      }
      lock.unlock();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  void process(json &&message) override {
    message.metadata["stream_sort_inserted_time"] = std::chrono::system_clock::now();
    priority_queue.push(std::move(message));
  }

 private:
  priority_queue_t priority_queue;
  std::function<bool(const json &lhs)> is_ready;
};

}

#endif //WOLF_STREAM_SORT_H
