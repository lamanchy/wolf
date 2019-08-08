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
    file.open(file_name, std::ios_base::binary);
  }

 protected:
  void setup() override {
    size_t buffer_size = 256 * 1024;
    char * buffer = new char[buffer_size];

    while (file) {
      file.read(buffer, buffer_size);
      long long int count = file.gcount();

      if (count == 0) break;

      s.deserialize(std::string(buffer, count), [this](json &&message) {
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