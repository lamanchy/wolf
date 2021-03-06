#pragma once

#include <base/plugins/threaded_plugin.h>
namespace wolf {

class mutexed_threaded_plugin : public threaded_plugin {
 public:
  mutexed_threaded_plugin() {
    processors_should_prefer_buffering();
  }
 protected:
  void prepare(json &&message) override;

  virtual void locked_loop() {}
  virtual void unlocked_loop() {}

  void run() override;

 private:
  void loop() override {}
  std::mutex lock;
};

}
