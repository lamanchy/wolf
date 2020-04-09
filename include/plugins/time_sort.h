#pragma once

#include <base/plugins/mutexed_threaded_plugin.h>
#include <extras/convert_time.h>
namespace wolf {

class time_sort : public mutexed_threaded_plugin {
 public:
  explicit time_sort(
      const static_option<int> &seconds_to_wait
  ) : seconds_to_wait(seconds_to_wait->value()) {}

 protected:
  using priority_queue_t = std::priority_queue<
      json,
      std::deque<json>,
      std::function<bool(const json &lhs, const json &rhs)>
  >;

  void locked_loop() override {
    using namespace std::chrono;
    std::sort(events.begin(), events.end(), [this](const json &lhs, const json &rhs) -> bool {
      return lhs.find("@timestamp")->get_string() < rhs.find("@timestamp")->get_string();
    });

    bool is_first = true;
    time_point<system_clock> prev_time;
    auto real_duration = nanoseconds(seconds(seconds_to_wait)).count();

    auto old_enough = [&](const json &event) {
      return (
          system_clock::now().time_since_epoch().count() - event.metadata.find("time_sort_inserted_time")->get_signed()
      ) > real_duration;
    };
    auto has_good_distance = [&](const json &event) {
      return (
          duration_cast<seconds>(extras::string_to_time(event.find("@timestamp")->get_string()) - prev_time).count()
      ) > seconds_to_wait;
    };

    for (auto event_i = events.begin(); event_i != events.end();) {
      if (old_enough(*event_i) and (is_first or has_good_distance(*event_i))) {
        output(std::move(*event_i));
        event_i = events.erase(event_i);
      } else {
        is_first = false;
        prev_time = extras::string_to_time(event_i->find("@timestamp")->get_string());
        ++event_i;
      }
    }
  }

  void unlocked_loop() override {
    get_loop_sleeper().sleep_for(std::chrono::seconds(1));
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


