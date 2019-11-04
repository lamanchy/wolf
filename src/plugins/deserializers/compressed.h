//
// Created by lamanchy on 9.5.19.
//

#ifndef WOLF_SRC_DESERIALIZERS_COMPRESSED_H_
#define WOLF_SRC_DESERIALIZERS_COMPRESSED_H_
#include <mutex>
#include <base/plugins/base_plugin.h>
#include <iso646.h>
#include <extras/gzip.h>

namespace wolf {
namespace from {

class compressed : public base_plugin {
 protected:
  void process(json &&message) override;

 private:
  bool is_gzip = false;
  bool is_line = false;
  std::string message;
  std::mutex m;
};

}
}
#endif //WOLF_SRC_DESERIALIZERS_COMPRESSED_H_
