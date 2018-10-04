#ifndef WOLF_STDOUT_H
#define WOLF_STDOUT_H


#include <base/plugin.h>

namespace wolf {

class cout : public plugin {
public:
  void print_name() override {
    std::cout << "cout" << std::endl;
  }

  ~cout() {
    std::cout << std::flush;
  }

protected:
  void process(json &&message) override {
//    tao::json::from_string(tao::json::to_string(message))["pi"].get_unsigned();
//    assert(i == 0);
    std::cout << message.get_string() << "\n";
//    assert(message.is_object());
//    assert(message["happy"].is_boolean());
//    assert(message["pi"] == i++);
//    assert(message["happy"] == true);
  }

private:
  unsigned long i = 0;
};

}

#endif //WOLF_STDOUT_H
