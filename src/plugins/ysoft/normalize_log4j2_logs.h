//
// Created by lamanchy on 8/19/18.
//

#ifndef WOLF_NORMALIZE_LOG4J2_LOGS_H
#define WOLF_NORMALIZE_LOG4J2_LOGS_H

#include <base/plugin.h>
#include <extras/convert_time.h>

namespace wolf {

class normalize_log4j2_logs : public plugin {
protected:
  void process(json &&message) override {
    message["@timestamp"] = extras::convert_time(message["timeMillis"].get_unsigned());
    message.erase("timeMillis");
    output(std::move(message));
  }
};

}

#endif //WOLF_NORMALIZE_LOG4J2_LOGS_H
