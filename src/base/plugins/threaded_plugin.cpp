#include "threaded_plugin.h"

namespace wolf {

void threaded_plugin::run() {
  setup();
  while (is_loop_running()) {
    loop();
  }
  flush();
  mark_as_stopped();
}
bool threaded_plugin::is_loop_running() { return loop_running; }

void threaded_plugin::mark_as_stopped() {
  running = false;
  pipeline_status::notify_sleeper();
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
  loop_sleeper.wake_up();
}

bool threaded_plugin::is_running() {
  return running;
}
}