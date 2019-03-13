#ifndef WOLF_THREADED_PLUGIN_H
#define WOLF_THREADED_PLUGIN_H


#include <thread>
#include "plugin.h"

namespace wolf {

class threaded_plugin : public plugin {
protected:
  std::atomic<bool> running{false};
  std::thread thread;

  virtual void setup() { }

  virtual void loop() { }

  threaded_plugin() : plugin() { }

  void start() override {
    running = true;
    thread = std::thread(&threaded_plugin::run, this);
  }

  void end_loop() {
    running = false;
  }

  void stop() override {
    end_loop();
    thread.join();
  }

  bool is_running() override {
    return running;
  }

  virtual void run() {
    setup();
    while (is_running()) {
      loop();
    }
  }


};

}

#endif //WOLF_THREADED_PLUGIN_H
