

#include <mutex>
#include <plugins/deserializers/line.h>
void wolf::from::line::process(json &&message) {
  std::string string = message.get_string();
  unsigned p = 0;
  auto partition = message.metadata.find("partition");
  if (partition != nullptr) {
    p = partition->get_unsigned();
  }
  std::string _previous = this->get_previous(p);

  auto begin = string.begin();
  auto mark = string.begin();
  auto end = string.end();
  while (mark != end) {
    if (*mark == '\n') {
      std::string res;
      if (!_previous.empty()) {
        res += _previous;
        _previous.clear();
      }
      res += std::string(begin, mark);
      if (!res.empty()) {
        output(json(res).copy_metadata(message));
      }
      begin = ++mark;
    } else {
      ++mark;
    }
  }
  if (begin != end) {
    _previous += std::string(begin, end);
  }
  if (!_previous.empty()) {
    this->put_previous(p, _previous);
  }
}
