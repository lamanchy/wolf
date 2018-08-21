//
// Created by lamanchy on 8/19/18.
//

#ifndef WOLF_NORMALIZE_NLOG_LOGS_H
#define WOLF_NORMALIZE_NLOG_LOGS_H

#include <base/plugin.h>
#include <extras/convert_time.h>

namespace wolf {

class normalize_nlog_logs : public plugin {
protected:
  void process(json &&message) override {
    message["@timestamp"] = extras::convert_time(message["time"].as<std::string>());
    message.erase("time");
    output(std::move(message));
  }
};

}

#endif //WOLF_NORMALIZE_NLOG_LOGS_H
