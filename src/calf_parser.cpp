#include <base/pipeline.h>
#include <base/plugins/plugin.h>
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
#include <serializers/plain.h>
#include <plugins/cout.h>
#include <serializers/compressed.h>
#include <plugins/collate.h>
#include <extras/extras.h>

int main(int argc, char *argv[]) {
  using namespace wolf;
  using std::string;

  options opts = options(argc, argv);
  auto output = opts.add
      <command<string>>("output", "Type of output, kafka/logstash", "logstash",
                        [](const string &value) { return extras::is_in<string>(value, {"kafka", "logstash"}); });
  auto output_ip = opts.add<command<string>>("output_ip", "Ip address of output", "localhost");
  auto output_port = opts.add<command<string>>("output_port", "Port of output", "9070");
  auto group = opts.add<command<string>>("group", "Define the group name", "default");
  auto max_loglevel = opts.add<command<string>>(
      "max_loglevel", "Define max loglevel, one of OFF, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, ALL", "INFO");

  pipeline p(opts);

  std::function<plugin::pointer(string)> out;
  plugin::pointer tcp =
      make<collate<line>>(60, 1000)->register_output(
          make<tcp_out<compressed>>(output_ip, output_port)
      );

  if (output->get_value() == "kafka") {
    out = [&](const string& type) {
      return make<kafka_out>(
          make<constant<string>>(type + "-" + group->get_value()),
          12,
          output_ip->get_value() + ":9092");
    };
  } else if (output->get_value() == "logstash") {
    out = [&](const string& type) { return tcp; };
  }

  plugin::pointer common_processing =
      make<add_local_info>(group->get_value(), max_loglevel->get_value())->register_output(
          make<json_to_string>()->register_output(
              out("unified_logs")
          )
      );

  p.register_plugin(
      make<tcp_in<line>>(make<constant<unsigned short>>(9556)),
      make<string_to_json>(),
      make<normalize_nlog_logs>(),
      common_processing
  );

  p.register_plugin(
      make<tcp_in<line>>(make<constant<unsigned short>>(9555)),
      make<string_to_json>(),
      make<normalize_log4j2_logs>(),
      common_processing
  );

  p.register_plugin(
      make<tcp_in<line>>(make<constant<unsigned short>>(9559)),
      make<string_to_json>(),
      make<normalize_serilog_logs>(),
      common_processing
  );

  p.register_plugin(
      make<tcp_in<line>>(make<constant<unsigned short>>(9557)),
      make<lambda>(
          [group](json &message) {
            message.assign_object(
                {
                    {"message", message},
                    {"group", group->get_value()},
                    {"type", "metrics"}
                });
          }),
      make<json_to_string>(),
      out("metrics")
  );
  p.run();

  return 0;
}
