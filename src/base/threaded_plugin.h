#ifndef WOLF_THREADED_PLUGIN_H
#define WOLF_THREADED_PLUGIN_H


#include <thread>
#include "plugin.h"

namespace wolf {

class threaded_plugin : public plugin {
protected:
  std::atomic<bool> running{false};
  std::thread thread;

  virtual void run() { }

  threaded_plugin() : plugin() { }
  threaded_plugin(std::string name) : plugin(name) { }

public:
  void start() override {
    running = true;
    thread = std::thread(&threaded_plugin::run, this);
  }

  void stop() override {
    running = false;
    thread.join();
  }

  bool is_running() override {
    return running;
  }


};

}

#endif //WOLF_THREADED_PLUGIN_H
