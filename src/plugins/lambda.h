#include <utility>

//
// Created by lamanchy on 10/1/18.
//

#ifndef WOLF_LAMBDA_H
#define WOLF_LAMBDA_H

#include <base/plugin.h>

namespace wolf {
class lambda : public plugin {
public:
  lambda(std::function<void(json &)> fn) : fn(std::move(fn)) {}

protected:
  void process(json &&message) override {
    fn(message);
    output(std::move(message));
  }

private:
  std::function<void(json &)> fn;
};
}


#endif //WOLF_LAMBDA_H
