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
    logger.info("Speed " + std::to_string(i/1000) + "k events per minute.");
    i = 0;
    std::this_thread::sleep_for(std::chrono::seconds(60));
  }

  void process(json &&message)
  override {
    i++;
    output(std::move(message));
  }

 private:
  std::atomic<unsigned long> i{0};

  std::atomic<std::chrono::high_resolution_clock::time_point> time{std::chrono::high_resolution_clock::now()};
};

}

#endif //WOLF_STATS_H
