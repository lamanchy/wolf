#include <utility>

//
// Created by lamanchy on 8/19/18.
//

#ifndef WOLF_ADD_LOCAL_INFO_H
#define WOLF_ADD_LOCAL_INFO_H

#include <asio/ip/host_name.hpp>
#include <base/plugins/plugin.h>

namespace wolf {

class add_local_info : public plugin {
public:
  add_local_info(std::string group) : group(group) {}

protected:
  void process(json &&message) override {
    message["host"] = asio::ip::host_name();
    message["group"] = group;
    auto component = message.find("component");
    if (component == nullptr) {
      message["component"] = "default";
    }

    std::string level = message["level"].get_string();
    if (level == "DEBUG" or level == "TRACE") {
      return;
    }

    if (message["message"].get_string().size() > 32000) {
      std::string m = message["message"].get_string();
      m.resize(3200);
      m += "... truncated";
      message["message"] = m;
    }

    output(std::move(message));
  }

private:
  std::string group;
};

}

#endif //WOLF_ADD_LOCAL_INFO_H
