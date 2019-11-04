//
// Created by lamanchy on 9.5.19.
//

#ifndef WOLF_SRC_SERIALIZERS_COMPRESSED_H_
#define WOLF_SRC_SERIALIZERS_COMPRESSED_H_
#include <mutex>
#include <base/plugins/base_plugin.h>
#include <plugins/deserializers/compressed.h>

namespace wolf {
namespace to {

class compressed : public base_plugin {
 protected:
  void process(json &&message) override;
 };

}
}
#endif //WOLF_SRC_SERIALIZERS_COMPRESSED_H_
