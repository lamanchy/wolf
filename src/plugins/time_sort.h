//
// Created by lamanchy on 21.6.19.
//

#ifndef WOLF_TIME_SORT_H
#define WOLF_TIME_SORT_H

#include <base/plugins/mutexed_threaded_plugin.h>
#include <extras/convert_time.h>
namespace wolf {

class time_sort : public mutexed_threaded_plugin {
 public:
  explicit time_sort(
      const option<int> &seconds_to_wait
  ) : seconds_to_wait(seconds_to_wait->value()) {}

 protected:
  using priority_queue_t = std::priority_queue<
      json,
      std::deque<json>,
      std::function<bool(const json &lhs, const json &rhs)>
  >;

  void unlocked_loop() override {
    lock.lock();
    std::sort(events.begin(), events.end(), [this](const json &lhs, const json &rhs) -> bool {
      return lhs.find("@timestamp")->get_string() < rhs.find("@timestamp")->get_string();
    });
    bool is_first = true;
    std::chrono::time_point<std::chrono::system_clock> prev_time;
    long real_duration = std::chrono::nanoseconds(std::chrono::seconds(seconds_to_wait)).count();
    std::vector<json> to_keep;
    std::vector<json> to_output;
    for (auto event : events) {
      if (
          (
              std::chrono::system_clock::now().time_since_epoch().count()
                  - event.metadata.find("time_sort_inserted_time")->get_signed()
          ) > real_duration
              and (
                  is_first
                      or
                          std::chrono::duration_cast<std::chrono::seconds>(
                              extras::string_to_time(event.find("@timestamp")->get_string())
                                  - prev_time).count() > seconds_to_wait
              )
          ) {
        // old enough AND (first or has good distance)
        to_output.push_back(std::move(event));
      } else {
        // too young
        is_first = false;
        prev_time = extras::string_to_time(event.find("@timestamp")->get_string());
        to_keep.push_back(std::move(event));
      }
    }
    events = std::move(to_keep);
    lock.unlock();
    for (auto event: to_output)
      output(std::move(event));

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void process(json &&message) override {
    message.metadata["time_sort_inserted_time"] = std::chrono::system_clock::now().time_since_epoch().count();
    events.push_back(std::move(message));
  }

 private:
  int seconds_to_wait;
  std::vector<json> events;
};

}

#endif //WOLF_TIME_SORT_H
