//
// Created by lamanchy on 17.11.18.
//

#ifndef WOLF_MUTEXED_THREADED_PLUGIN_H
#define WOLF_MUTEXED_THREADED_PLUGIN_H

#include "threaded_plugin.h"
namespace wolf {

class mutexed_threaded_plugin : public threaded_plugin {
 protected:
  void prepare(json &&message) override;

  virtual void locked_loop() {}
  virtual void unlocked_loop() {}

  void run() override;

  std::mutex lock;

 private:
  void loop() override {}
};

}

#endif //WOLF_MUTEXED_THREADED_PLUGIN_H
