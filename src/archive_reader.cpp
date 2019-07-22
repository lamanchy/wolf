

#include <base/pipeline.h>
#include <plugins/file_in.h>
#include <serializers/deserialize.h>
#include <serializers/compressed.h>
#include <serializers/serialize.h>
#include <plugins/json_to_string.h>
#include <plugins/cout.h>
#include <plugins/string_to_json.h>
#include <serializers/line.h>
int main(int argc, char ** argv) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, false);

  std::string file_name = p.option<command<std::string>>("file_name", "File to load")->get_value();

  p.register_plugin(
      create<file_in<compressed>>(file_name),
      create<deserialize<line>>(),
      create<string_to_json>(),
      create<json_to_string>(true),
      create<cout>()
  );

  p.run();
  return 0;
}