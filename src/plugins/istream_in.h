//
// Created by lomic on 11/1/2019.
//

#ifndef WOLF_SRC_PLUGINS_ISTREAM_IN_H_
#define WOLF_SRC_PLUGINS_ISTREAM_IN_H_

#include <base/plugins/threaded_plugin.h>
namespace wolf {

template<typename Serializer>
class istream_in : public threaded_plugin {
 public:
  explicit istream_in(std::istream &istream) :
      istream(istream) {}

 protected:
  void setup() override {
    setup_future();
  }

  void loop() override {
    if (future_string.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      auto line = future_string.get();
      setup_future();

      s.deserialize(line, [this](json &&message) {
        output(std::move(message));
      });
    }

    get_loop_sleeper().sleep_for(std::chrono::seconds(1));
  }

 private:
  void setup_future() {
    future_string = std::async(std::launch::async, [this] { return get_string_from_istream(); });
  }

  std::string get_string_from_istream() {
    std::string line;
    if (not std::getline(istream, line)) {
      end_loop();
      return std::string();
    }
    return line;
  }

  std::istream &istream;
  std::future<std::string> future_string;
  Serializer s;
};

}

#endif //WOLF_SRC_PLUGINS_ISTREAM_IN_H_
