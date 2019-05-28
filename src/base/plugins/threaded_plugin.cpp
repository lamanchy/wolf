//
// Created by lamanchy on 27.5.19.
//

#include "threaded_plugin.h"

namespace wolf {

void threaded_plugin::run() {
  setup();
  while (is_running()) {
    loop();
  }
}
void threaded_plugin::stop() {
  end_loop();
  thread.join();
}
void threaded_plugin::start() {
  running = true;
  thread = std::thread(&threaded_plugin::run, this);
}
void threaded_plugin::end_loop() {
  running = false;
}
bool threaded_plugin::is_running() {
  return running;
}
}