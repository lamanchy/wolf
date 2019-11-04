#include <wolf.h>

template<typename TimeT = std::chrono::milliseconds>
struct measure {
  template<typename F, typename ...Args>
  static typename TimeT::rep execution(F &&func, Args &&... args) {
    auto start = std::chrono::steady_clock::now();
    std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
    auto duration = std::chrono::duration_cast<TimeT>
        (std::chrono::steady_clock::now() - start);
    return duration.count();
  }
};

int main(int argc, char *argv[]) {
  using namespace wolf;
  using std::string;

  options opts = options(argc, argv);
  auto output = opts.add
      <command<string>>("output", "Type of output, kafka/logstash", "logstash",
                        [](const string &value) { return extras::is_in<string>(value, {"kafka", "logstash"}); });
  auto output_ip = opts.add<command<string>>("output_ip", "Ip address of output", "localhost");
  auto group = opts.add<command<string>>("group", "Define the group name", "default");
  auto max_loglevel = opts.add<command<string>>(
      "max_loglevel", "Define max loglevel, one of OFF, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, ALL", "INFO");


  // todo this should not be possible
  plugin b = make<from::string>();
  pipeline p(opts);
  // todo this should not be possible
  auto max_loglevel2 = opts.add<command<string>>(
      "max_loglevel2", "Define max loglevel, one of OFF, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, ALL", "INFO");

  std::function<plugin(string)> out;

  if (output->value() == "kafka") {
    out = [&](const string &type) {
      return make<kafka::output>(
          type + "-" + group->value(),
          12,
          kafka::config({
                                {"metadata.broker.list", output_ip->value() + ":9092"},
                                {"compression.type", "lz4"},
//        { "topic.metadata.refresh.interval.ms", 20000 },
//        {"debug", "broker,topic,msg"},
                                {"linger.ms", "1000"}
                            })
      );
    };
  } else {
    out = [&](const string &type) {
      return pipeline::chain_plugins(
          make<to::line>(),
          make<collate>(60, 1000),
          make<to::compressed>(),
          make<tcp::output>(output_ip, "9070")
      );
    };
  }

  plugin common_processing = pipeline::chain_plugins(
      make<add_local_info>(group, max_loglevel),
      make<from::string>(),
      out("unified_logs")
  );

  p.register_plugin(
      make<tcp::input>(9556),
      make<from::line>(),
      make<stats>(),
      make<from::string>(),
      make<normalize_nlog_logs>(),
      common_processing
  );

  p.register_plugin(
      make<tcp::input>(9555),
      make<from::line>(),
      make<from::string>(),
      make<normalize_log4j2_logs>(),
      common_processing
  );

  p.register_plugin(
      make<tcp::input>(9559),
      make<from::line>(),
      make<from::string>(),
      make<normalize_serilog_logs>(),
      common_processing
  );

  p.register_plugin(
      make<tcp::input>(9557),
      make<from::line>(),
      make<lambda>(
          [group](json &message) {
            message.assign_object(
                {
                    {"message", message},
                    {"group", group->value()},
                    {"type", "metrics"}
                });
          }),
      make<from::string>(),
      out("metrics")
  );
  p.run();

  // todo this should not be possible
  auto a = make<from::string>();

  return 0;
}
