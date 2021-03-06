#pragma once

#include <base/plugins/base_plugin.h>
#include <extras/convert_time.h>

#include <utility>
namespace wolf {
namespace to {

class influx : public base_plugin {
 public:
  influx(std::string measurement,
         std::vector<std::string> tags,
         std::vector<std::string> fields,
         std::string time = "@timestamp",
         bool add_random_nanoseconds = true)
      : measurement(std::move(measurement)),
        tags(std::move(tags)),
        fields(std::move(fields)),
        time(std::move(time)),
        add_random_nanoseconds(add_random_nanoseconds) {}

 protected:
  void process(json &&message) override {
    std::stringstream res;
    res.precision(2);

    res << escape(measurement, ", ");

    for (auto &tag : tags) {
      auto it = message.find(tag);
      if (it != nullptr)
        res << "," << escape(tag, ",= ") << "=" << escape(it->get_string(), ",= ");
    }

    bool first = true;
    for (auto &field : fields) {
      auto it = message.find(field);
      if (it != nullptr) {
        res << (first ? " " : ",");
        first = false;

        res << escape(field, ",= ") << "=";
        if (it->is_string_type()) {
          res << '\"' << escape(it->get_string(), "\"") << '\"';
        } else if (it->is_integer()) {
          res << it->get_signed() << "i";
        } else if (it->is_double()) {
          res << it->get_double();
        } else if (it->is_boolean()) {
          res << (it->get_boolean() ? "t" : "f");
        }
      }
    }

    std::string timestamp = message.find(time)->get_string();
    auto since_epoch = extras::string_to_time(timestamp).time_since_epoch();
    long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(since_epoch).count();
    if (add_random_nanoseconds)
      nanoseconds += std::rand() % 10000000;
    res << " " << nanoseconds;

    auto to_output = json(std::move(res.str()));
    to_output.metadata = message.metadata;
    to_output.size = message.size;
    output(std::move(to_output));
  }

 private:
  std::string measurement, time;
  std::vector<std::string> tags, fields;
  bool add_random_nanoseconds;

  std::string escape(const std::string &src, const std::string &escape_seq) {
    size_t pos = 0, start = 0;
    std::stringstream res;
    while ((pos = src.find_first_of(escape_seq, start)) != std::string::npos) {
      res.write(src.c_str() + start, pos - start);
      res << '\\' << src[pos];
      start = ++pos;
    }
    res.write(src.c_str() + start, src.length() - start);

    return res.str();
  }
};

}
}


