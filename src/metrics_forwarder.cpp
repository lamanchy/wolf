#include <wolf.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  options o(argc, argv);
  auto broker_list = o.add<command<std::string>>("broker_list", "List of kafka brokers", "localhost:9092");
  auto influx_ip = o.add<command<std::string>>("influx_ip", "Ip address of influx", "localhost");

  pipeline p(o);

  p.register_plugin(
      make<kafka::input>("^metrics-.*", kafka::config(
          {
              {"metadata.broker.list", broker_list->value()},
              {"group.id", "wolf_metrics_forwarder5"},
              {"client.id", "wolf_metrics_forwarder5"},
              {"auto.offset.reset", "earliest"},
              {"queued.max.messages.kbytes", 64},
              {"fetch.max.bytes", 64 * 1024},
              {"enable.auto.commit", true},
              {"heartbeat.interval.ms", 10000},
              {"session.timeout.ms", 50000},
              {"metadata.max.age.ms", 300000}
          })),
      make<from::string>(),
      make<lambda>(
          [](json &message) {
            message.assign_string(std::string(message["message"].get_string() + "\n"));
          }
      ),
      make<stats>(),
      make<collate>(),
      make<http::output>(influx_ip, "8086", "/write?db=metric_db")
  );

  p.run();
  return 0;

}
