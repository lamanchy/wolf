

#include <mutex>
#include <plugins/deserializers/line.h>
void wolf::from::line::process(json &&message) {
  std::lock_guard<std::mutex> lg(m);
  std::string string = message.get_string();
  auto begin = string.begin();
  auto mark = string.begin();
  auto end = string.end();
  while (mark != end) {
    if (*mark == '\n') {
      std::string res;
      if (!previous.empty()) {
        res += previous;
        previous.clear();
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
    previous += std::string(begin, end);
  }
}
