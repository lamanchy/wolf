#include <utility>

#include <utility>

//
// Created by lamanchy on 11.3.19.
//

#ifndef WOLF_GET_ELAPSED_PREEVENTS_H
#define WOLF_GET_ELAPSED_PREEVENTS_H

#include <base/plugins/plugin.h>
namespace wolf {

class get_elapsed_preevents : public plugin {
 public:
  struct elapsed_config {
    std::string start_logId, end_logId, uniqueId, name;
  };

  explicit get_elapsed_preevents(std::vector<elapsed_config> configs) : configs(std::move(configs)) { }

 protected:
  void process(json &&message) override {
    auto logId = message.find("logId");
    if (logId != nullptr) {
      for (elapsed_config &c : configs) {
        if (logId->get_string() == c.start_logId) {
          output_preevent("start", c, message);
        }
        if (logId->get_string() == c.end_logId) {
          output_preevent("end", c, message);
        }
      }
    }
    output(std::move(message));
  }
 private:
  std::vector<elapsed_config> configs;

  void output_preevent(const std::string &position, elapsed_config &config, json &message) {
    metrics_output({
               {"algorithm", "elapsed"},
               {"position", position},
               {"elapsedId", config.name},
               {"uniqueId", message.find(config.uniqueId)->get_string()},
               {"@timestamp", message["@timestamp"].get_string()},
               {"type", "correlation_data"},
               {"host", message["host"].get_string()},
               {"group", message["group"].get_string()}
           });
  }


  void metrics_output(json &&message) {
    output("metrics", std::move(message));
  }

 public:
  pointer register_metrics_output(pointer plugin) {
    return register_named_output("metrics", std::move(plugin));
  }

};
}

#endif //WOLF_GET_ELAPSED_PREEVENTS_H
