#pragma once

#include <base/plugins/mutexed_threaded_plugin.h>
#include <extras/convert_time.h>
namespace wolf {

class elapsed : public mutexed_threaded_plugin {
 public:
  explicit elapsed(const static_option<int> &max_seconds_to_keep) :
      max_seconds_to_keep(max_seconds_to_keep->value()) {}

 public:

  template<typename... Args>
  plugin register_expired_output(plugin plugin, Args &&... args) {
    return register_named_output("expired", std::move(plugin), args...);
  }

 protected:
  void process(json &&message) override {
    std::string key = get_key(message);

    if (message["position"].get_string() == "start") {
      if (storage.find(key) == storage.end()) {
        message.metadata["elapsed_inserted_time"] = std::chrono::system_clock::now().time_since_epoch().count();
        storage.insert(std::make_pair(key, std::move(message)));
      }
    } else {
      auto start = storage.find(key);
      if (start != storage.end()) {
        json out = {
            {"start_time", start->second["@timestamp"]},
            {"end_time", message["@timestamp"]},
            {"type", "metrics"},
            {"status", "ok"},
            {"start_host", start->second["host"]},
            {"end_host", message["host"]},
            {"group", start->second["group"]},
            {"elapsedId", start->second["elapsedId"]},
            {"uniqueId", start->second["uniqueId"]},
            {"duration", (extras::string_to_time(message["@timestamp"].get_string())
                - extras::string_to_time(start->second["@timestamp"].get_string())).count()},
        };
        out.metadata = start->second.metadata;

        output(std::move(out));

        storage.erase(start);
      }
    }
  }

  void locked_loop() override {
    for (auto it = storage.begin(); it != storage.end();) {
      if ((
          std::chrono::system_clock::now().time_since_epoch().count() -
              it->second.metadata["elapsed_inserted_time"].get_signed()
      ) > std::chrono::nanoseconds(std::chrono::seconds(max_seconds_to_keep)).count()
          ) {
        json out = {
            {"start_time", it->second["@timestamp"]},
            {"type", "metrics"},
            {"status", "expired"},
            {"start_host", it->second["host"]},
            {"group", it->second["group"]},
            {"elapsedId", it->second["elapsedId"]},
            {"uniqueId", it->second["uniqueId"]},
        };
        out.metadata = it->second.metadata;
        expired_output(std::move(out));
        storage.erase(it++);
      } else {
        ++it;
      }
    }
  }

  void unlocked_loop() override {
    get_loop_sleeper().sleep_for(std::chrono::seconds(1));
  }

  void expired_output(json &&message) {
    output("expired", std::move(message));
  }

 private:
  std::map<std::string, json> storage;
  unsigned int max_seconds_to_keep;

  static std::string get_key(json &message) {
    return message["uniqueId"].get_string() + "-" + message["elapsedId"].get_string();
  }
};

}


