#include <utility>

#include <utility>

//
// Created by lamanchy on 17.12.18.
//

#ifndef WOLF_STREAM_SORT_H
#define WOLF_STREAM_SORT_H

#include <base/plugins/mutexed_threaded_plugin.h>
namespace wolf {

class stream_sort : public mutexed_threaded_plugin {
 public:

  template<typename T>
  static std::function<bool(const json &)> ready_after(std::chrono::duration<T> duration) {
    long real_duration = std::chrono::nanoseconds(duration).count();
    return [real_duration](const json &top) -> bool {
      return (std::chrono::system_clock::now().time_since_epoch().count()
          - top.metadata.find("stream_sort_inserted_time")->get_signed()) > real_duration;
    };
  }

  stream_sort(
      const std::function<bool(const json &lhs, const json &rhs)> &cmp,
      std::function<bool(const json &)> is_ready
  ) : priority_queue(priority_queue_t(cmp)), is_ready(std::move(is_ready)) {}

 protected:
  using priority_queue_t = std::priority_queue<
      json,
      std::deque<json>,
      std::function<bool(const json &lhs, const json &rhs)>
  >;

  void unlocked_loop() override {
    lock.lock();
    while (not priority_queue.empty() and is_ready(priority_queue.top())) {
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

  void process(json &&message) override {
    message.metadata["stream_sort_inserted_time"] = std::chrono::system_clock::now().time_since_epoch().count();
    priority_queue.push(std::move(message));
  }

 private:
  priority_queue_t priority_queue;
  std::function<bool(const json &lhs)> is_ready;
};

}

#endif //WOLF_STREAM_SORT_H
