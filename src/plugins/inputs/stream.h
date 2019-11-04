#pragma once

#include <base/plugins/threaded_plugin.h>
namespace wolf {
namespace stream {

class input : public threaded_plugin {
 public:
  explicit input(std::istream &stream) :
      stream(stream) {
    should_never_buffer();
    non_processors_should_block();
  }

 protected:
  void setup() override {
    setup_future();
  }

  void loop() override {
    if (future_string.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      auto line = future_string.get();
      output(json(line));

      setup_future();
    }

    get_loop_sleeper().sleep_for(std::chrono::seconds(1));
  }

 private:
  void setup_future() {
    future_string = std::async(std::launch::async, [this] { return get_string_from_istream(); });
  }

  std::string get_string_from_istream() {
    std::string line;
    if (not std::getline(stream, line)) {
      end_loop();
      return std::string();
    }
    return line;
  }

  std::istream &stream;
  std::future<std::string> future_string;
};

}
}


