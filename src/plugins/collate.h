//
// Created by lamanchy on 8/15/18.
//

#ifndef WOLF_COLLATE_H
#define WOLF_COLLATE_H

#include <base/plugins/mutexed_threaded_plugin.h>

namespace wolf {

template<typename Serializer>
class collate : public mutexed_threaded_plugin {
 public:
  collate(unsigned int secondsToWait = 1, unsigned int maxEvents = 10000) :
      max_events(maxEvents), seconds_to_wait(secondsToWait) {

  }
 protected:

  void locked_loop() override {
    empty();
  }
  void unlocked_loop() override {
    std::this_thread::sleep_for(std::chrono::seconds(seconds_to_wait));
  }

  void process(json &&message) override {
    buffer += s.serialize(std::move(message));
    count += 1;
    if (count > max_events) {
      empty();
    }
  }

  bool is_full() override {
    return false;
  }

 private:
  void empty() {
    if (!buffer.empty()) {
      output(json(std::move(buffer)));
      buffer.clear();
      count = 0;
    }
  }

  unsigned int max_events;
  unsigned int seconds_to_wait;
  std::string buffer;
  unsigned int count = 0;
  Serializer s;

};

}

#endif //WOLF_COLLATE_H
