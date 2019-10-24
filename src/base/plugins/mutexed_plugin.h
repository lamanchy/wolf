//
// Created by lamanchy on 8/15/18.
//

#ifndef WOLF_MUTEXED_PLUGIN_H
#define WOLF_MUTEXED_PLUGIN_H

#include <base/plugins/plugin.h>

namespace wolf {

class mutexed_plugin : public plugin {
 protected:
  void prepare(json &&message) override;

  std::mutex lock;
};

}

#endif //WOLF_MUTEXED_PLUGIN_H
