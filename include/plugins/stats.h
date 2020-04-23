#pragma once

#include <base/plugins/threaded_plugin.h>

#include <utility>
namespace wolf {

class stats : public threaded_plugin {
 public:
  explicit stats(std::string description) : description(std::move(description)) {}
  stats() : stats("stats") {}

 protected:
  void loop() override {
    logger.info(description + " speed " + std::to_string(i) + " events per min.");
    i = 0;
    get_loop_sleeper().sleep_for(std::chrono::seconds(60));
  }

  void process(json &&message) override {
    i++;
    output(std::move(message));
  }

 private:
  std::atomic<unsigned long> i{0};
  std::string description;
};

}


