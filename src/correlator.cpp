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

int main(int argc, char *argv[]) {
  using namespace wolf;


  pipeline p = pipeline(argc, argv, false);

  not_event_option<std::string> opt = p.option<constant<std::string>>("event");

  std::cout << p.option<command<std::string>>("filter", "ffff")->get_value() << std::endl;
  std::cout << p.option<command<std::string>>("option", "ffff")->get_value() << std::endl;
  std::cout << p.option<event<std::string>>("filter")->get_value(json({{"filter", "event"}})) << std::endl;
  std::cout << opt->get_value(json({{"event", "123"}})) << std::endl;

  p.logger.info("Parsing command line arguments");

//  p.register_plugin(
//      plugin<cin>()->register_output(
//          plugin<string_to_json>()->register_output(
//              plugin<stream_sort>(
//                  [](const json &lhs, const json &rhs) -> bool {
//                    return lhs.find("@timestamp")->get_string() > rhs.find("@timestamp")->get_string();
//                  },
//                      stream_sort::ready_after(std::chrono::seconds(0))
//              )->register_output(
//                  plugin<elapsed>()->register_output(
//                      plugin<json_to_string>()->register_output(
//                          plugin<cout>()
//                      )
//                  )
//              )
//          )
//      )
//  );

  plugin::pointer sort_by_time = create<stream_sort>(
      [](const json &lhs, const json &rhs) -> bool {
        return lhs.find("@timestamp")->get_string() > rhs.find("@timestamp")->get_string();
      },
      stream_sort::ready_after(std::chrono::seconds(1))
  );

  p.register_plugin(
      create<cin>(),
      create<string_to_json>(),
      sort_by_time,
      create<elapsed>(),
      create<json_to_string>(),
      create<cout>()
  );

  p.logger.info("Starting");
  p.run();
  p.logger.info("Stopped");

  return 0;
}
