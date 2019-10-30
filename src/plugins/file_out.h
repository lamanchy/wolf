#include <utility>

//
// Created by lamanchy on 17.7.19.
//

#ifndef WOLF_FILE_OUT_H
#define WOLF_FILE_OUT_H

#include <base/plugins/mutexed_threaded_plugin.h>
#include <extras/get_time.h>
namespace wolf {

class file_out : public mutexed_threaded_plugin {
 public:
  explicit file_out(const option<std::string>& file_name) :
  file_name(file_name->value()) {
    current_date = extras::get_date();
    file.open(get_file_name(), std::ios_base::app | std::ios_base::binary);
  }

 protected:
  void process(json &&message) override {
    file << message.get_string();
    file.flush();
  }

  void locked_loop() override {
    if (extras::get_date() != current_date) {
      current_date = extras::get_date();
      file.close();
      file.open(get_file_name());
    }
  }

  void unlocked_loop() override {
    std::this_thread::sleep_for(std::chrono::minutes(1));
  }

 private:
  std::string get_file_name() {
    return file_name + "-" + current_date;
  }

  std::string file_name;
  std::ofstream file;
  std::string current_date;
};

}

#endif //WOLF_FILE_OUT_H
