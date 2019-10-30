#include <base/plugins/plugin.h>
#include <base/pipeline.h>
#include <plugins/generator.h>
#include <plugins/kafka_out.h>
#include <plugins/collate.h>
#include <plugins/lambda.h>
#include <whereami/whereami.h>
#include <plugins/kafka_in.h>
#include <plugins/cout.h>
#include <extras/get_time.h>
#include <plugins/http_out.h>
#include <plugins/string_to_json.h>
#include <plugins/stats.h>
#include <serializers/plain.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, false);
  Logger &logger = p.logger;

  logger.info("Parsing command line arguments");

  std::string bootstrap_servers, influx_ip;

  bootstrap_servers = p.option<command<std::string>>("broker_list", "List of kafka brokers")->value();
  influx_ip = p.option<command<std::string>>("influx_ip", "Ip address of influx, default localhost")->value();

  logger.info("Parsed arguments:");
  logger.info("bootstrap_servers:    " + bootstrap_servers);
  logger.info("influx_ip: " + influx_ip);

  p.register_plugin(
      create<kafka_in>("^metrics-.*", bootstrap_servers, "wolf_metrics_forwarder5"),
      create<string_to_json>(),
      create<lambda>(
          [](json &message) {
            message.assign_string(std::string(message["message"].get_string() + "\n"));
          }
      ),
      create<stats>(),
      create<collate<plain>>(),
      create<http_out>(influx_ip, "8086", "/write?db=metric_db")
  );

  p.run();
  return 0;

}
