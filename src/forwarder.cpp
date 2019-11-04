#include <wolf.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  options o(argc, argv);
  auto broker_list = o.add<command<std::string>>("broker_list", "List of kafka brokers", "localhost:9092");
  auto es_ip = o.add<command<std::string>>("es_ip", "Ip address of elasticsearch", "localhost");

  pipeline p(o);

  p.register_plugin(
      make<kafka::input>("^parsed_logs-.*", kafka::config(
          {
              {"metadata.broker.list", broker_list->value()},
              {"group.id", "wolf_logs_forwarder5"},
              {"client.id", "wolf_logs_forwarder5"},
              {"auto.offset.reset", "earliest"},
              {"queued.max.messages.kbytes", 64},
              {"fetch.max.bytes", 64 * 1024},
              {"enable.auto.commit", true},
              {"heartbeat.interval.ms", 10000},
              {"session.timeout.ms", 50000},
              {"metadata.max.age.ms", 300000}
          })),
      make<lambda>(
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
                                 {"_index", "logs-" + group + "-" + extras::get_date()},
                                 {"_type", "logs"}
                             }}
                         })
                ) + "\n" + std::string(message.get_string() + "\n")
            );
          }
      ),
      make<stats>(),
      make<collate>(),
      make<http::output>(es_ip, "9200", "/_bulk")
  );

  p.run();
}
