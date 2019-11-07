#pragma once

#include "base_plugin.h"
#include <thread>

namespace wolf {

class threaded_plugin : public base_plugin {
 protected:
  virtual void setup() {}

  virtual void loop() {}

  virtual void flush() {}

  void start() override;

  bool is_running() override;

  virtual void end_loop();

  void stop() override;

  virtual void run();

  void mark_as_stopped();

  bool is_loop_running();

  sleeper &get_loop_sleeper() {
    return loop_sleeper;
  }

 private:
  std::atomic<bool> loop_running{false};
  std::atomic<bool> running{false};
  std::thread thread;
  sleeper loop_sleeper;
};

}


