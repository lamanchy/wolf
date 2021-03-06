#pragma once

#include <atomic>
#include <base/sleeper.h>

namespace wolf {

class pipeline_status {
 public:
  static bool is_initialized() { return initialized; }
  static bool was_started() { return started; }
  static bool is_running() { return running; }
  static bool is_persistent() { return persistent; }
  static unsigned get_buffer_size() { return buffer_size; }
  static void notify_sleeper() {
    pipeline_sleeper.wake_up();
  }

 private:
  static bool initialized;
  static bool started;
  static std::atomic<bool> running;
  static sleeper pipeline_sleeper;
  static bool persistent;
  static unsigned buffer_size;

  friend class pipeline;
  pipeline_status() = default;
};

}


