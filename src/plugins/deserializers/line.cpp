

#include <mutex>
#include <plugins/deserializers/line.h>
void wolf::from::line::process(json &&message) {
  std::string string = message.get_string();
  std::string _previous = std::string(previous);
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
  previous = _previous;
}
