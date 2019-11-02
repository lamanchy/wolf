//
// Created by lomic on 11/1/2019.
//

#ifndef WOLF_SRC_BASE_PIPELINE_STATUS_H_
#define WOLF_SRC_BASE_PIPELINE_STATUS_H_

#include <atomic>
#include "sleeper.h"

namespace wolf {

class pipeline_status {
 public:
  static bool is_initialized() { return initialized; }
  static bool is_running() { return running; }
  static bool is_persistent() { return persistent; }
  static unsigned get_buffer_size() { return buffer_size; }
  static void notify_sleeper() {
    pipeline_sleeper.wake_up();
  }

 private:
  static bool initialized;
  static std::atomic<bool> running;
  static sleeper pipeline_sleeper;
  static bool persistent;
  static unsigned buffer_size;

  friend class pipeline;
  pipeline_status() = default;
};

}

#endif //WOLF_SRC_BASE_PIPELINE_STATUS_H_
