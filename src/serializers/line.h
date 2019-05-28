#ifndef WOLF_LINE_H
#define WOLF_LINE_H

#include <base/serializer.h>
#include <iostream>

namespace wolf {

class line : public serializer {
 public:

  std::string serialize(json &&json) override {
    return json.as<std::string>() + "\n";
  }

  void deserialize(std::string &&string, const std::function<void(json &&)> &fn) override;
 private:
  std::string previous;
  std::mutex m;

};

}

#endif //WOLF_LINE_H

