//
// Created by lamanchy on 4.5.19.
//

#ifndef WOLF_DROP_H
#define WOLF_DROP_H

#include <base/plugins/plugin.h>
namespace wolf {

class drop : public plugin {
 protected:
  void process(json &&message) override {}
};

}

#endif //WOLF_DROP_H
