//
// Created by lamanchy on 8/19/18.
//

#ifndef WOLF_ADD_LOCAL_INFO_H
#define WOLF_ADD_LOCAL_INFO_H

#include <asio/ip/host_name.hpp>
#include <base/plugin.h>

namespace wolf {

class add_local_info : public plugin {
protected:
  void process(json &&message) override {
    message["host"] = asio::ip::host_name();
    message["group"] = "default";
    message["component"] = "default";
    message["type"] = "logs";
    output(std::move(message));
  }
};

}

#endif //WOLF_ADD_LOCAL_INFO_H
