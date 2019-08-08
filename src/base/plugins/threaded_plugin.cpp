//
// Created by lamanchy on 27.5.19.
//

#include "threaded_plugin.h"

namespace wolf {

void threaded_plugin::run() {
  setup();
  while (loop_running) {
    loop();
  }
  running = false;
}
void threaded_plugin::stop() {
  end_loop();
  thread.join();
}
void threaded_plugin::start() {
  running = true;
  loop_running = true;
  thread = std::thread(&threaded_plugin::run, this);
}
void threaded_plugin::end_loop() {
  loop_running = false;
}
bool threaded_plugin::is_running() {
  return running;
}
}