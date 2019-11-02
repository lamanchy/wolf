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
  collate(const option<unsigned int> &secondsToWait = 1,
          const option<unsigned int> &maxEvents = 10000) :
      max_events(maxEvents->value()),
      seconds_to_wait(secondsToWait->value()) {

  }
 protected:

  void locked_loop() override {
    empty();
  }
  void unlocked_loop() override {
    get_loop_sleeper().sleep_for(std::chrono::seconds(seconds_to_wait));
  }

  void flush() override {
    empty();
  }

  void process(json &&message) override {
    buffer += s.serialize(std::move(message));
    count += 1;
    if (count > max_events) {
      empty();
    }
  }

 private:
  void empty() {
    if (!buffer.empty()) {
      json j(std::move(buffer));
      j.size = count;
      output(std::move(j));
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
