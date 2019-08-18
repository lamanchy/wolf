

#include <base/pipeline.h>
#include <plugins/file_in.h>
#include <serializers/deserialize.h>
#include <serializers/compressed.h>
#include <serializers/serialize.h>
#include <plugins/json_to_string.h>
#include <plugins/cout.h>
#include <plugins/string_to_json.h>
#include <serializers/line.h>
#include <plugins/lambda.h>
#include <serializers/plain.h>
#include <plugins/collate.h>
#include <plugins/http_out.h>
#include "json_to_influx.h"

int main(int argc, char ** argv) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, false);

  std::string file_name = p.option<command<std::string>>("file_name", "File to load")->get_value();

  p.register_plugin(
      create<file_in<compressed>>(file_name),
      create<deserialize<line>>(),
      create<string_to_json>(),
      create<lambda>(
          [](json &message) {
//            std::string m = message["message"].get_string();
//            if (m.length() > 100) {
//              std::string title;
//              for (auto c : m) {
//                if (c == '<' or c == '>') continue;
//                if (c == '"') c = '\'';
//              }
//              std::string title = str.erase(std::remove(str.begin(), str.end(), 'a'), str.end());
//            }
            json copy(message);
            copy.erase("message");
            copy.erase("@timestamp");
            copy.erase("logId");
            copy.erase("host");
            copy.erase("group");
            copy.erase("level");
            copy.erase("component");
            message["message"].assign_string(std::string(message["message"].get_string()) + " " + tao::json::to_string(copy));
          }
      ),
      create<json_to_influx>(
          "logs",
          std::vector<std::string>({"logId", "host", "group", "level", "component"}),
          std::vector<std::string>({"message"}),
          "@timestamp"
          ),
      create<lambda>(
          [](json &message) {
            message.assign_string(std::string(message.get_string() + "\n"));
          }
      ),
      create<collate<plain>>(1, 1000),
//      create<cout>()
      create<http_out>("localhost", "8086", "/write?db=log_db")
  );

  p.run();
  return 0;
}