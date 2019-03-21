#include <base/plugins/plugin.h>
#include <base/pipeline.h>
#include <plugins/generator.h>
#include <plugins/tcp_in.h>
#include <serializers/line.h>
#include <plugins/string_to_json.h>
#include <plugins/kafka_out.h>
#include <plugins/ysoft/add_local_info.h>
#include <plugins/json_to_string.h>
#include <plugins/ysoft/normalize_nlog_logs.h>
#include <plugins/ysoft/normalize_serilog_logs.h>
#include <plugins/ysoft/normalize_log4j2_logs.h>
#include <plugins/tcp_out.h>
#include <plugins/lambda.h>
#include <whereami/whereami.h>
#include <plugins/cin.h>
#include <plugins/cout.h>
#include <re2/set.h>
#include <plugins/regex.h>
#include <plugins/ysoft/get_elapsed_preevents.h>
#include <plugins/ysoft/count_logs.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, true);

  p.register_plugin(
      create<cin>(),
      create<string_to_json>(),
      create<regex>(std::vector<std::pair<std::string, std::string>>(regex::parse_file("/home/lamanchy/CLionProjects/wolf/cmake-build-debug/parsers"))),
      create<get_elapsed_preevents>(std::vector<get_elapsed_preevents::elapsed_config>({{"start_stuff", "stop_stuff", "myId", "stuff_elapsed"}}))->register_metrics_output(
          create<json_to_string>()->register_output(
              create<cout>()
              )
          ),
      create<count_logs>(std::vector<std::string>({"logId", "host", "group"}))->register_stats_output(
          create<json_to_string>()->register_output(
              create<cout>()
          )
      ),
      create<json_to_string>(),
      create<cout>()
  );

  p.run();

  return 0;
}
