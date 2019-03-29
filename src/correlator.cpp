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

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, false);

  plugin::pointer sort_by_time = create<stream_sort>(
      [](const json &lhs, const json &rhs) -> bool {
        return lhs.find("@timestamp")->get_string() > rhs.find("@timestamp")->get_string();
      },
      stream_sort::ready_after(std::chrono::seconds(60))
  );

  std::string broker_list = p.option<command<std::string>>("broker_list", "List of kafka brokers")->get_value();

  p.register_plugin(
      create<kafka_in>(
          p.option<constant<std::string>>("correlation_data-.*")->get_value(),
          broker_list,
          p.option<constant<std::string>>("correlator")->get_value()
          ),
      create<string_to_json>(),
      sort_by_time,
      create<elapsed>(), // TODO json to influxDB format
      create<json_to_string>(),
      create<cout>()
  );

  p.logger.info("Starting");
  p.run();
  p.logger.info("Stopped");

  return 0;
}
