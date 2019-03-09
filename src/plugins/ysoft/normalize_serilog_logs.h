//
// Created by lamanchy on 8/19/18.
//

#ifndef WOLF_NORMALIZE_SERILOG_LOGS_H
#define WOLF_NORMALIZE_SERILOG_LOGS_H

#include <base/plugins/plugin.h>
#include <extras/convert_time.h>

namespace wolf {

class normalize_serilog_logs : public plugin {
protected:
  void process(json &&message) override {
    message["@timestamp"] = extras::convert_time(message["timestamp"].as<std::string>());
    message.erase("timestamp");
    message["level"] = normalize_level(message["level"].get_string());
    auto logId = message.find("logId");
    if (logId != nullptr) {
      std::string id = logId->get_string();
      id[0] = static_cast<char>(tolower(id[0]));
      logId->assign_string(id);
    }
    output(std::move(message));
  }

private:
  std::string normalize_level(const std::string &level) {
    if (level == "Verbose") return "TRACE";
    if (level == "Debug") return "DEBUG";
    if (level == "Information") return "INFO";
    if (level == "Warning") return "WARN";
    if (level == "Error") return "ERROR";
    if (level == "Fatal") return "TRACE";
    if (level == "Verbose") return "FATAL";
    throw std::runtime_error("Unknown log level " + level);
  }
};

}

#endif //WOLF_NORMALIZE_SERILOG_LOGS_H
