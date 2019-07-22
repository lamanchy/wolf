//
// Created by lamanchy on 17.7.19.
//

#ifndef WOLF_FILE_IN_H
#define WOLF_FILE_IN_H

#include <base/plugins/threaded_plugin.h>
namespace wolf {

template<typename Serializer>
class file_in : public threaded_plugin {
 public:
  explicit file_in(const std::string& file_name) {
    file.open(file_name);
  }

 protected:
  void setup() override {
    for (std::string line; std::getline(file, line);) {
      s.deserialize(std::string(line + "\n"), [this](json &&message) {
            output(std::move(message));
          });
    }
    end_loop();
  }
 private:
  std::ifstream file;
  Serializer s;
};

}

#endif //WOLF_FILE_IN_H
