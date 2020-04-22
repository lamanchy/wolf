#pragma once

#include <iostream>
#include <base/plugins/base_plugin.h>

namespace wolf {
namespace from {

class line : public base_plugin {
 protected:
  void process(json &&message) override;
 private:
  std::map<unsigned, std::string> previous;

  std::string get_previous(unsigned partition) {
    auto prev = previous.find(partition);
    if (prev == previous.end())
      return std::string();
    std::string res(std::move(prev->second));
    previous.erase(prev);
    return res;
  }

  void put_previous(unsigned partition, std::string&& prev) {
    previous.emplace(partition, std::move(prev));
  }
};

}
}



