//
// Created by lamanchy on 17.11.18.
//

#ifndef WOLF_STATS_H
#define WOLF_STATS_H

#include <base/plugins/threaded_plugin.h>
namespace wolf {

class stats : public threaded_plugin {
 protected:
  void loop() override {
    logger.info("Speed " + std::to_string(i) + " events per sec.");
    i = 0;
    get_loop_sleeper().sleep_for(std::chrono::seconds(1));
  }

  void process(json &&message) override {
    i++;
    output(std::move(message));
  }

 private:
  std::atomic<unsigned long> i{0};
};

}

#endif //WOLF_STATS_H
