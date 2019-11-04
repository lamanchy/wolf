#include <wolf.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  options o(argc, argv);
  auto broker_list = o.add<command<std::string>>("broker_list", "List of kafka brokers", "localhost:9092");

  pipeline p(o);

  std::function<plugin(std::string)> out = [&](const std::string &topic_name) {
    return p.chain_plugins(
        make<lambda>([&, topic_name](json &message) {
          message.metadata["output"] = topic_name + "-" + message["group"].get_string();
        }),
        make<to::string>(),
        make<stats>(),
        make<kafka::output>(
            make<event<std::string>>("output", true), 12,
            kafka::config({
                                  {"metadata.broker.list", broker_list->value()},
                                  {"compression.type", "lz4"},
//        { "topic.metadata.refresh.interval.ms", 20000 },
//        {"debug", "broker,topic,msg"},
                                  {"linger.ms", "1000"}
                              })
        )
    );
  };

  p.register_plugin(
      make<kafka::input>(
          "^unified_logs-.*",
          kafka::config(
              {
                  {"metadata.broker.list", broker_list->value()},
                  {"group.id", "wolf_parser5"},
                  {"client.id", "wolf_parser5"},
                  {"auto.offset.reset", "earliest"},
                  {"queued.max.messages.kbytes", 64},
                  {"fetch.max.bytes", 64 * 1024},
                  {"enable.auto.commit", true},
                  {"heartbeat.interval.ms", 10000},
                  {"session.timeout.ms", 50000},
                  {"metadata.max.age.ms", 300000}
              })
      ),
      make<from::string>(),
      make<regex>(regex::parse_file(p.get_config_dir() + "parsers")),
      make<get_elapsed_preevents>(
          get_elapsed_preevents::parse_file(p.get_config_dir() + "elapsed")
      )->register_preevents_output(
          out("correlation_data")
      ),
      make<count_logs>(
          std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"})
      )->register_stats_output(
          make<lambda>(
              [&](json &message) {
                message.metadata["group"] = message["group"].get_string();
              }
          ),
          make<to::influx>(
              "logs_count",
              std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"}),
              std::vector<std::string>({"count"}),
              "@timestamp"
          ),
          make<lambda>(
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
