#ifndef WOLF_COUT_H
#define WOLF_COUT_H

#include <base/plugins/plugin.h>

namespace wolf {

class cout : public mutexed_plugin {
 public:
  explicit cout(bool print_metadata = false) : print_metadata(print_metadata) { }

  ~cout() {
    std::cout << std::flush;
  }

 protected:
  void process(json &&message) override {
    if (print_metadata) {
      std::cout << message.get_string() << ", metadata: " << tao::json::to_string(message.metadata) << std::endl;
    } else {
      std::cout << message.get_string() << std::endl;
    }
//    tao::json::from_string(tao::json::to_string(message))["pi"].get_unsigned();
//    assert(i == 0);
//    if (i++ % 10000 == 0) {
//      std::cout << i << "\n";
//    }
//    assert(message.is_object());
//    assert(message["happy"].is_boolean());
//    assert(message["pi"] == i++);
//    assert(message["happy"] == true);
  }

 private:
  std::atomic<unsigned long> i{0};
  bool print_metadata;
};

}

#endif //WOLF_COUT_H
