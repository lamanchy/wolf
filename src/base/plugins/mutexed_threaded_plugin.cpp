//
// Created by lamanchy on 27.5.19.
//

#include "mutexed_threaded_plugin.h"

namespace  wolf {

void mutexed_threaded_plugin::prepare(json &&message) {
  std::lock_guard<std::mutex> lg(lock);
  process(std::move(message));
}
void mutexed_threaded_plugin::run() {
  {
    std::lock_guard<std::mutex> lg(lock);
    setup();
  }
  while (is_running()) {
    unlocked_loop();
    std::lock_guard<std::mutex> lg(lock);
    locked_loop();
  }
}
}