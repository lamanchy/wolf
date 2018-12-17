#ifndef WOLF_STDOUT_H
#define WOLF_STDOUT_H


#include <base/plugin.h>

namespace wolf {

class cout : public plugin {
public:
  ~cout() {
    std::cout << std::flush;
  }

protected:
  void process(json &&message) override {
    std::cout << message.get_string() << std::endl;
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
};

}

#endif //WOLF_STDOUT_H
