#include <base/plugin.h>
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

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, false);
  Logger &logger = p.logger;
  logger.info("Parsing command line arguments");
  cxxopts::Options opts(argv[0], " - example command line options");

  p.register_plugin(
      create<cin>()->register_output(
          create<string_to_json>()->register_output(
              create<stream_sort>(
                  [](const json &lhs, const json &rhs) -> bool {
                    return lhs.find("@timestamp")->get_string() > rhs.find("@timestamp")->get_string();
                  },
                  stream_sort::ready_after(std::chrono::seconds(0))
              )->register_output(
                  create<elapsed>()->register_output(
                      create<json_to_string>()->register_output(
                          create<cout>()
                      )
                  )
              )
          )
      )
  );

  logger.info("Starting");
  p.run();
  logger.info("Stopped");

  return 0;
}
