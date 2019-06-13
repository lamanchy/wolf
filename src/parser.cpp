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
#include <plugins/kafka_in.h>
#include <base/options/event.h>
#include <json_to_influx.h>
#include <plugins/collate.h>
#include <plugins/http_out.h>
#include <plugins/elapsed.h>
#include <plugins/stream_sort.h>
#include <plugins/stats.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, false);

  std::string brokers =
      p.option<command<std::string>>("broker_list", "List of kafkas brokers")->get_value();

  std::function<plugin::pointer(std::string)> out = [&](const std::string &topic_name) {
    return create<lambda>(
        [&, topic_name](json &message) {
          message.metadata["output"] = topic_name + "-" + message["group"].get_string();
        }
    )->register_output(
        create<json_to_string>(),
        create<stats>(),
        create<kafka_out>(
            p.option<event<std::string>>("output", true), 12, brokers
        )
    );
  };

  p.register_plugin(
      create<kafka_in>(
          "^unified_logs-.*",
          brokers,
          "wolf_parser4"
      ),
      create<string_to_json>(),
      create<regex>(regex::parse_file(p.get_config_dir() + "parsers")),
      create<get_elapsed_preevents>(
          get_elapsed_preevents::parse_file(p.get_config_dir() + "elapsed")
      )->register_preevents_output(
          out("correlation_data")
      ),
      create<count_logs>(
          std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"})
      )->register_stats_output(
          create<lambda>(
              [&](json &message) {
                message.metadata["group"] = message["group"].get_string();
              }
          ),
          create<json_to_influx>(
              "logs_count",
              std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"}),
              std::vector<std::string>({"count"}),
              "@timestamp"
          ),
          create<lambda>(
              [&](json &message) {
                message.assign_object({
                                          {"message", message.get_string()},
                                          {"type", "metrics"},
                                          {"group", message.metadata["group"].get_string()},
                                      });
              }
          ),
          out("metrics")
      ),
      out("parsed_logs")
  );

  p.run();

  return 0;
}
