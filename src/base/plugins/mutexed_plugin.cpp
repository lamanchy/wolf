//
// Created by lamanchy on 27.5.19.
//

#include "mutexed_plugin.h"

void wolf::mutexed_plugin::prepare(wolf::json &&message) {
  std::lock_guard<std::mutex> lg(lock);
  process(std::move(message));
}
