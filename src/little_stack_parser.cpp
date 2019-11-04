#include <wolf.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  options o(argc, argv);

  auto input_port = o.add<command<unsigned short>>("input_port", "Input port", "9070");
  auto stream_sort_seconds = o.add<command<int>>("stream_sort_seconds", "Seconds to wait with each event", "60");

  pipeline p(o);

  plugin metrics_output =
      make<lambda>(
          [](json &message) {
            message.assign_string(std::string(message.get_string() + "\n"));
          }
      )->register_output(
          make<stats>(),
          make<collate>(),
          make<http::output>("localhost", "8086", "/write?db=metric_db")
      );

  plugin sort_by_time = make<stream_sort>(
      [](const json &lhs, const json &rhs) -> bool {
        return lhs.find("@timestamp")->get_string() > rhs.find("@timestamp")->get_string();
      },
      stream_sort::ready_after(std::chrono::seconds(stream_sort_seconds->value()))
  );

  p.register_plugin(
      make<tcp::input>(input_port),
      make<from::compressed>(),
      make<from::line>(),
      make<from::string>(),

      make<filter>(
          [](const json &message) {
            auto type = message.find("type");
            return type != nullptr && type->get_string() == "metrics";
          }
      )->filtered(
          make<lambda>(
              [](json &message) {
                message.assign_string(std::string(message["message"].get_string()));
              }
          ),
          metrics_output
      ),

      make<regex>(regex::parse_file(p.get_config_dir() + "parsers")),

      make<get_elapsed_preevents>(
          get_elapsed_preevents::parse_file(p.get_config_dir() + "elapsed")
      )->register_preevents_output(
          sort_by_time,
          make<elapsed>(1800)->register_expired_output(
              make<to::influx>(
                  "elapsed",
                  std::vector<std::string>({"elapsedId", "status", "start_host", "group"}),
                  std::vector<std::string>({"uniqueId",}),
                  "start_time"
              ),
              metrics_output
          ),
          make<to::influx>(
              "elapsed",
              std::vector<std::string>({"elapsedId", "status", "start_host", "end_host", "group"}),
              std::vector<std::string>({"uniqueId", "duration"}),
              "start_time"
          ),
          metrics_output
      ),

      make<count_logs>(
          std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"})
      )->register_stats_output(
          make<to::influx>(
              "logs_count",
              std::vector<std::string>({"logId", "host", "group", "level", "component", "spocGuid"}),
              std::vector<std::string>({"count"}),
              "@timestamp"
          ),
          metrics_output
      ),
      // send logs to influx as well
      make<lambda>(
          [](json &message) {
            json copy(message);
            copy.erase("message");
            copy.erase("@timestamp");
            copy.erase("logId");
            copy.erase("host");
            copy.erase("group");
            copy.erase("level");
            copy.erase("component");
            std::string rest;

            for (const auto &key : copy.get_object()) {
              if (rest.length() > 0)
                rest += ", ";

              if (key.second.is_null())
                continue;

              rest += key.first + ": " + tao::json::to_string(key.second);
            }
            message["rest"] = rest;
          }
      ),
      make<to::influx>(
          "logs",
          std::vector<std::string>({"logId", "host", "group", "level", "component"}),
          std::vector<std::string>({"message", "rest"}),
          "@timestamp"
      ),
      make<lambda>(
          [](json &message) {
            message.assign_string(std::string(message.get_string() + "\n"));
          }
      ),
      make<collate>(1, 1000),
      make<http::output>("localhost", "8086", "/write?db=log_db")
  );

  p.run();

  return 0;
}
