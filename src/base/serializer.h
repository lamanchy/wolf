//
// Created by lamanchy on 8/13/18.
//

#ifndef WOLF_SERIALIZER_H
#define WOLF_SERIALIZER_H


#include "json.h"

namespace wolf {

class serializer {
public:
  virtual std::string serialize(json &&json) = 0;

  virtual void deserialize(std::string &&string, const std::function<void(json &&)>& fn) = 0;
};

}

#endif //WOLF_SERIALIZER_H
