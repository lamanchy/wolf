#include <base/plugins/plugin.h>
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
#include <serializers/plain.h>

int main(int argc, char *argv[]) {
  using namespace wolf;

  pipeline p = pipeline(argc, argv, true);
  Logger &logger = p.logger;

  logger.info("Parsing command line arguments");
  cxxopts::Options opts(argv[0], " - example command line options");

  std::string output, output_ip, group;
  opts.add_options()
      ("output", "Type of output, kafka/logstash", cxxopts::value<std::string>(output)->default_value("kafka"))
      ("output_ip", "Ip address of output", cxxopts::value<std::string>(output_ip)->default_value("10.0.11.162"))
      ("group", "Define the group name", cxxopts::value<std::string>(group)->default_value("default"));
  opts.parse(argc, argv);

  logger.info("Parsed arguments:");
  logger.info("output:    " + output);
  logger.info("output_ip: " + output_ip);
  logger.info("group:     " + group);

  std::function<plugin::pointer(std::string)> out;
  plugin::pointer tcp = create<tcp_out<line>>(
      p.option<constant<std::string>>(output_ip),
      p.option<constant<std::string>>("9070")
    );

  if (output == "kafka") {
    out = [&](std::string type) { return create<kafka_out>(type + "-" + group, 1, output_ip + ":9092"); };
  } else if (output == "logstash") {
    out = [&](std::string type) { return tcp; };
  } else {
    throw std::runtime_error("output is not kafka nor logstash but " + output);
  }
//  out = [&](std::string type) { return create<cout>(); };

  plugin::pointer common_processing =
      create<add_local_info>(group)->register_output(
          create<json_to_string>()->register_output(
              out("unified_logs")
          )
      );

  p.register_plugin(
      create<tcp_in<line>>(p.option<constant<unsigned short>>(9556)),
      create<string_to_json>(),
      create<normalize_nlog_logs>(),
      common_processing
  );

  p.register_plugin(
      create<tcp_in<line>>(p.option<constant<unsigned short>>(9555)),
      create<string_to_json>(),
      create<normalize_log4j2_logs>(),
      common_processing
  );

  p.register_plugin(
      create<tcp_in<line>>(p.option<constant<unsigned short>>(9559)),
      create<string_to_json>(),
      create<normalize_serilog_logs>(),
      common_processing
  );

  p.register_plugin(
      create<tcp_in<line>>(p.option<constant<unsigned short>>(9557)),
      create<lambda>(
          [group](json &message) {
            message.assign_object(
                {
                    {"message", message},
                    {"group", group},
                    {"type", "metrics"}
                });
          }),
      create<json_to_string>(),
      out("metrics")
  );
  logger.info("Starting");
  p.run();
  logger.info("Stopped");

  return 0;
}
