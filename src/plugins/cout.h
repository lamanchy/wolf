#ifndef WOLF_STDOUT_H
#define WOLF_STDOUT_H


#include <base/plugin.h>

namespace wolf {

class cout : public plugin {
public:
  unsigned long i = 0;

public:
  void process(json &&message) override {
//    tao::json::from_string(tao::json::to_string(message))["pi"].get_unsigned();
//    assert(i == 0);
    std::cout << message.get_string() << "\n";
//    assert(message.is_object());
//    assert(message["happy"].is_boolean());
//    assert(message["pi"] == i++);
//    assert(message["happy"] == true);
  }

  ~cout() {
    std::cout << std::flush;
  }

};

}

#endif //WOLF_STDOUT_H
