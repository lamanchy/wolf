//
// Created by lamanchy on 8/15/18.
//

#ifndef WOLF_COLLATE_H
#define WOLF_COLLATE_H

#include <base/plugins/mutexed_threaded_plugin.h>

namespace wolf {

class collate : public mutexed_threaded_plugin {
 protected:

  void locked_loop() override {
    empty();
  }
  void unlocked_loop() override {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void process(json &&message) override {
    buffer += message.get_string();
    count += 1;
    if (count > 10000) {
      empty();
    }
  }

  bool is_full() override {
    return false;
    std::lock_guard<std::mutex> lg(lock);
    return count > 10000;

    bool res = lock.try_lock();
    if (res) lock.unlock();
    return res;
  }

 private:
  void empty() {
    if (buffer.size() > 0) {
      output(json(std::move(buffer)));
      buffer.clear();
      count = 0;
    }
  }

  std::string buffer;
  unsigned int count = 0;

};

}

#endif //WOLF_COLLATE_H
