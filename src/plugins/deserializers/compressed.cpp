

#include <plugins/deserializers/compressed.h>

namespace wolf {
namespace from {
void compressed::process(json &&next) {
  std::lock_guard<std::mutex> lg(m);
  message.append(next.get_string());
  while (true) {
    if (not is_line and not is_gzip) {
      // determine if line or gzip
      if (message.length() >= extras::gzip::get_prefix().length()) {
        if (message.rfind(extras::gzip::get_prefix(), 0) == 0)
          is_gzip = true;
        else
          is_line = true;
      } else {
        if (message.find('\n') != std::string::npos)
          is_line = true;
      }
      if (not is_line and not is_gzip) {
        break;
      }
    }
    // is line or is gzip
    if (is_line) {
      auto pos = message.find('\n');
      if (pos == std::string::npos) break;
      output(json(message.substr(0, pos + 1)).copy_metadata(next));
      message = message.substr(pos + 1);
      is_line = false;
    }

    if (is_gzip) {
      auto pos = message.find(extras::gzip::string_serializer_end);
      if (pos == std::string::npos) break;

      size_t size = extras::gzip::get_deserialized_size(message.substr(extras::gzip::get_prefix().length(),
                                                                       pos - extras::gzip::get_prefix().length() + 1));

      if (message.length() < 1 + pos + size) break;

      auto test1 = gzip::decompress(message.substr(pos + 1, size).data(), size);
      output(json(gzip::decompress(message.substr(pos + 1, size).data(), size)));
      message = message.substr(pos + size + 1);
      is_gzip = false;
    }
  }
}

}
}