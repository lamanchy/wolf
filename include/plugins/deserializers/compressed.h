#pragma once

#include <base/plugins/base_plugin.h>
#include <iso646.h>
#include <extras/gzip.h>

namespace wolf {
namespace from {

class compressed : public base_plugin {
 protected:
  void process(json &&message) override;

 private:
  std::map<unsigned, std::string> previous;
  std::mutex m;

  std::string get_previous(unsigned partition) {
    std::lock_guard<std::mutex> lg(m);
    auto prev = previous.find(partition);
    if (prev == previous.end())
      return std::string();
    std::string res = prev->second;
    previous.erase(prev);
    return res;
  }

  void put_previous(unsigned partition, const std::string& prev) {
    std::lock_guard<std::mutex> lg(m);
    previous.emplace(partition, prev);
  }
};

}
}

