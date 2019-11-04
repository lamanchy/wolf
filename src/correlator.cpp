#include <wolf.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  options o(argc, argv);

  auto max_seconds_to_keep = o.add<command<int>>("max_seconds_to_keep", "How long to wait for end event", "1800");
  auto broker_list = o.add<command<std::string>>("broker_list", "List of kafka brokers", "localhost:9092");
  auto stream_sort_seconds = o.add<command<int>>("stream_sort_seconds", "Seconds to wait with each event", "10");

  pipeline p(o);

  plugin common_processing = p.chain_plugins(
      make<lambda>(
          [](json &message) {
            message.assign_object(
                {
                    {"message", message.get_string()},
                    {"type", "metrics"}
                });
          }
      ),
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

  p.register_plugin(
      make<kafka::input>(
          "^correlation_data-.*",
          kafka::config(
              {
                  {"metadata.broker.list", broker_list->value()},
                  {"group.id", "wolf_correlator5"},
                  {"client.id", "wolf_correlator5"},
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
      make<lambda>(
          [](json &message) {
            message.metadata["output"] = "metrics-" + message["group"].get_string();
          }
      ),
      // todo make few plugins in a 'mutex' plugin, so order is not changed
      // or just use one thread...
      make<time_sort>(stream_sort_seconds),
      make<elapsed>(max_seconds_to_keep)->register_expired_output(
          make<to::influx>(
              "elapsed",
              std::vector<std::string>({"elapsedId", "status", "start_host", "group"}),
              std::vector<std::string>({"uniqueId",}),
              "start_time"
          ),
          common_processing
      ),
      make<to::influx>(
          "elapsed",
          std::vector<std::string>({"elapsedId", "status", "start_host", "end_host", "group"}),
          std::vector<std::string>({"uniqueId", "duration"}),
          "start_time"
      ),
      common_processing
  );

  p.run();

  return 0;
}
