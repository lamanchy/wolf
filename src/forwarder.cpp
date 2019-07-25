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
#include <plugins/copy.h>
#include <plugins/json_to_string.h>
#include <serializers/line.h>
#include <serializers/serialize.h>
#include <serializers/compressed.h>
#include <plugins/file_out.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, false);
  Logger &logger = p.logger;

  std::string bootstrap_servers, es_ip;

  bootstrap_servers = p.option<command<std::string>>("broker_list", "List of kafka brokers")->get_value();
  es_ip = p.option<command<std::string>>("es_ip", "Ip address of elasticsearch, default localhost")->get_value();

  logger.info("Parsed arguments:");
  logger.info("bootstrap_servers:    " + bootstrap_servers);
  logger.info("es_ip: " + es_ip);

  p.register_plugin(
      create<kafka_in>("^parsed_logs-.*", bootstrap_servers, "wolf_logs_forwarder5"),
      create<copy>()->register_copy_output(
          create<collate<line>>(300, 1000),
          create<serialize<compressed>>(),
          create<file_out>("logs_archive")
      ),
      create<lambda>(
          [](json &message) {
            const std::string &topic = message.metadata["topic"].get_string();
            std::string group;

            auto res = topic.find('-');
            if (res == std::string::npos) {
              group = "unknown";
            } else {
              group = topic.substr(res + 1);
            }

            message.assign_string(
                tao::json::to_string(
                    json({
                             {"index", {
                                 {"_index", "logs-" +
                                     group + "-" +
                                     extras::get_date()},
                                 {"_type", "logs"}
                             }}
                         })
                ) + "\n" + std::string(message.get_string() + "\n")
            );
          }
      ),
      create<stats>(),
      create<collate<plain>>(),
      create<http_out>(es_ip, "9200", "/_bulk")
  );

  p.run();
}
