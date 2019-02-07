//
// Created by lamanchy on 5.2.19.
//

#ifndef WOLF_ELAPSED_H
#define WOLF_ELAPSED_H

#include <base/mutexed_threaded_plugin.h>
#include <extras/convert_time.h>
namespace wolf {

class elapsed : public mutexed_threaded_plugin {
 protected:
  void run() override {
    while (running) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::lock_guard<std::mutex> lg(lock);

      for (auto it = storage.begin(); it != storage.end();) {

        if (
            (
                std::chrono::system_clock::now().time_since_epoch().count() -
                    it->second.metadata["elapsed_inserted_time"].get_signed()
            ) > std::chrono::nanoseconds(std::chrono::seconds(max_seconds_to_keep)).count()
            ) {
          output({
                     {"start_time", it->second["@timestamp"]},
                     {"type", "metrics"},
                     {"status", "expired"},
                     {"start_host", it->second["host"]},
                     {"group", it->second["group"]},
                     {"elapsedId", it->second["elapsedId"]},
                     {"uniqueId", it->second["uniqueId"]},
                 });
          storage.erase(it++);
        } else {
          ++it;
        }
      }
    }
  }

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

        output(std::move(out));

        storage.erase(start);
      }
    }
  }

  bool is_full() override {
    return false;
  }

 private:
  std::map<std::string, json> storage;
  unsigned int max_seconds_to_keep = 5;

  std::string get_key(json &message) {
    return message["uniqueId"].get_string() + "-" + message["elapsedId"].get_string();
  }
};

}

#endif //WOLF_ELAPSED_H
