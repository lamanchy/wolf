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
#include <plugins/cout.h>
#include <plugins/cin.h>
#include <plugins/stream_sort.h>
#include <plugins/elapsed.h>
#include <base/options/constant.h>
#include <base/options/command.h>
#include <base/options/event.h>
#include <plugins/kafka_in.h>
#include <json_to_influx.h>
#include <plugins/stats.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, false);

  plugin::pointer sort_by_time = create<stream_sort>(
      [](const json &lhs, const json &rhs) -> bool {
        return lhs.find("@timestamp")->get_string() > rhs.find("@timestamp")->get_string();
      },
      stream_sort::ready_after(std::chrono::seconds(
          p.option<command<int>>("stream_sort_seconds", "Seconds to wait with each event", "", "60")->get_value()
      ))
  );

  plugin::pointer in, out, common_processing;

  bool is_test = p.option<command<bool>>("test", "When testing, use stdin/stdout")->get_value();
  int max_seconds_to_keep =
      p.option<command<int>>("max_seconds_to_keep", "How long to wait for end event", "", "1800")->get_value();

  if (is_test) {
    in = create<cin>();
    out = create<cout>();
  } else {
    std::string broker_list = p.option<command<std::string>>("broker_list", "List of kafka brokers")->get_value();

    in = create<kafka_in>(
        p.option<constant<std::string>>("^correlation_data-.*")->get_value(),
        broker_list,
        p.option<constant<std::string>>("wolf_correlator5")->get_value()
    );
    out = create<kafka_out>(
        p.option<event<std::string>>("output", true), 12, broker_list
    );
  }

  common_processing = create<lambda>(
      [](json &message) {
        message.assign_object({
                                  {"message", message.get_string()},
                                  {"type", "metrics"}
                              });
      }
  )->register_output(
      create<json_to_string>(),
      create<stats>(),
      out
  );

  p.register_plugin(
      in,
      create<string_to_json>(),
      create<lambda>(
          [](json &message) {
            message.metadata["output"] = "metrics-" + message["group"].get_string();
          }
      ),
      sort_by_time,
      create<elapsed>(max_seconds_to_keep)->register_expired_output(
          create<json_to_influx>(
              "elapsed",
              std::vector<std::string>({"elapsedId", "status", "start_host", "group"}),
              std::vector<std::string>({"uniqueId",}),
              "start_time"
          ),
          common_processing
      ),
      create<json_to_influx>(
          "elapsed",
          std::vector<std::string>({"elapsedId", "status", "start_host", "end_host", "group"}),
          std::vector<std::string>({"uniqueId", "duration"}),
          "start_time",
          !is_test  // add random nanoseconds if not testing
      ),
      common_processing
  );

  p.run();

  return 0;
}
