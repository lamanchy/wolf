//
// Created by lamanchy on 8/19/18.
//

#ifndef WOLF_NORMALIZE_LOG4J2_LOGS_H
#define WOLF_NORMALIZE_LOG4J2_LOGS_H

#include <base/plugins/plugin.h>
#include <extras/convert_time.h>

namespace wolf {

class normalize_log4j2_logs : public plugin {
 protected:
  void process(json &&message) override {
    if (message.find("timeMillis") != nullptr) {
      message["@timestamp"] = extras::convert_time(message["timeMillis"].get_unsigned());
      message.erase("timeMillis");
    } else if (message.find("instant") != nullptr) {
      message["@timestamp"] = extras::convert_time(message["instant"]["epochSecond"].get_unsigned(),
                                                   message["instant"]["nanoOfSecond"].get_unsigned());
      message.erase("instant");
    } else {
      throw std::runtime_error("Cannot find time");
    }
    output(std::move(message));
  }
};

}

#endif //WOLF_NORMALIZE_LOG4J2_LOGS_H
