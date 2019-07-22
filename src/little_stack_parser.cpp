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
#include <plugins/filter.h>
#include <plugins/http_out.h>
#include <plugins/collate.h>
#include <plugins/stats.h>
#include <json_to_influx.h>
#include <plugins/stream_sort.h>
#include <plugins/elapsed.h>
#include <plugins/drop.h>
#include <serializers/compressed.h>
#include <serializers/plain.h>
#include <serializers/deserialize.h>
#include <serializers/serialize.h>
#include <plugins/file_out.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, true);

  plugin::pointer metrics_output =
      create<lambda>(
          [](json &message) {
            message.assign_string(std::string(message.get_string() + "\n"));
          }
      )->register_output(
          create<stats>(),
          create<collate<plain>>(),
          create<http_out>("localhost", "8086", "/write?db=metric_db")
      );

  plugin::pointer sort_by_time = create<stream_sort>(
      [](const json &lhs, const json &rhs) -> bool {
        return lhs.find("@timestamp")->get_string() > rhs.find("@timestamp")->get_string();
      },
      stream_sort::ready_after(std::chrono::seconds(
          p.option<command<int>>("stream_sort_seconds", "Seconds to wait with each event")->get_value()
      ))
  );

  p.register_plugin(
      create<tcp_in<compressed>>(
          p.option<command<unsigned short>>("input_port", "Input port")
      ),
      create<deserialize<line>>(),
      create<string_to_json>(),

      create<filter>(
          [](const json &message) {
            auto type = message.find("type");
            return type != nullptr && type->get_string() == "metrics";
          }
      )->filtered(
          create<lambda>(
              [](json &message) {
                message.assign_string(std::string(message["message"].get_string()));
              }
          ),
          metrics_output
      ),

      create<regex>(regex::parse_file(p.get_config_dir() + "parsers")),

      create<get_elapsed_preevents>(
          get_elapsed_preevents::parse_file(p.get_config_dir() + "elapsed")
      )->register_preevents_output(
          sort_by_time,
          create<elapsed>(1800)->register_expired_output(
              create<json_to_influx>(
                  "elapsed",
                  std::vector<std::string>({"elapsedId", "status", "start_host", "group"}),
                  std::vector<std::string>({"uniqueId",}),
                  "start_time"
              ),
              metrics_output
          ),
          create<json_to_influx>(
              "elapsed",
              std::vector<std::string>({"elapsedId", "status", "start_host", "end_host", "group"}),
              std::vector<std::string>({"uniqueId", "duration"}),
              "start_time"
          ),
          metrics_output
      ),

      create<count_logs>(
          std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"})
      )->register_stats_output(
          create<json_to_influx>(
              "logs_count",
              std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"}),
              std::vector<std::string>({"count"}),
              "@timestamp"
          ),
          metrics_output
      ),
      create<json_to_string>(),
      create<collate<line>>(60, 1000),
      create<serialize<compressed>>(),
      create<file_out>("logs_archive")
  );

  p.run();

  return 0;
}
