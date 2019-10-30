#ifndef WOLF_THREADED_PLUGIN_H
#define WOLF_THREADED_PLUGIN_H

#include <thread>
#include "plugin.h"

namespace wolf {

class threaded_plugin : public base_plugin {
 protected:
  std::atomic<bool> running{false};
  std::atomic<bool> loop_running{false};
  std::thread thread;

  virtual void setup() {}

  virtual void loop() {}

  void start() override;

  virtual void end_loop();

  void stop() override;

  bool is_running() override;

  virtual void run();

};

}

#endif //WOLF_THREADED_PLUGIN_H
