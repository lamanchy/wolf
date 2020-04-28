#pragma once

#include <iostream>
#include <base/plugins/base_plugin.h>

namespace wolf {
namespace from {

class line : public base_plugin {
 protected:
  void process(json &&message) override;
 private:
  std::map<std::pair<std::thread::id, unsigned int>, std::string> previous;
  std::mutex m;

  std::string &get_previous(unsigned partition) {
    std::lock_guard<std::mutex> lg(m);
    return previous.emplace(
        std::make_pair(std::this_thread::get_id(), partition),
        std::string()
    ).first->second;
  }

  void clear_previous(unsigned partition) {
    std::lock_guard<std::mutex> lg(m);
    previous.erase(std::make_pair(std::this_thread::get_id(), partition));
  }
};

}
}



