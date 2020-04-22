

#include <mutex>
#include <plugins/deserializers/line.h>
void wolf::from::line::process(json &&message) {
  std::string string(std::move(message.get_string()));
  unsigned p = 0;
  auto partition = message.metadata.find("partition");
  if (partition != nullptr) {
    p = partition->get_unsigned();
  }
  std::string _previous(this->get_previous(p));
  bool previous_empty = _previous.empty();

  auto begin = string.begin();
  auto mark = string.begin();
  auto end = string.end();
  while (mark != end) {
    if (*mark == '\n') {
      if (!previous_empty) {
        output(json(_previous + std::string(begin, mark)).copy_metadata(message));
        _previous.clear();
        previous_empty = true;
      } else if (begin != mark) {
        output(json(std::string(begin, mark)).copy_metadata(message));
      }
      begin = ++mark;
    } else {
      ++mark;
    }
  }
  if (begin != end) {
    if (previous_empty) {
      _previous = std::string(begin, end);
      previous_empty = false;
    } else {
      _previous += std::string(begin, end);
    }
  }
  if (!previous_empty) {
    this->put_previous(p, std::move(_previous));
  }
}
