#pragma once

#include <base/plugins/threaded_plugin.h>

#include <utility>
namespace wolf {

class stats : public threaded_plugin {
 public:
  stats(std::string description, unsigned interval = 60) :
      description(std::move(description)), interval(interval) {}
  stats() : stats("stats") {}

 protected:
  void loop() override {
    logger.info << description << " speed " << int(i * 60 / interval) << " events per min." << std::endl;
    i = 0;
    get_loop_sleeper().sleep_for(std::chrono::seconds(interval));
  }

  void process(json &&message) override {
    i++;
    output(std::move(message));
  }

 private:
  logging::logger logger{"stats"};
  std::atomic<unsigned long> i{0};
  std::string description;
  unsigned interval = 60;
};

}


