#include <base/plugin.h>
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

std::string get_dir_path() {

  char *path = NULL;
  int length, dirname_length;
  std::string dir_path;

  length = wai_getExecutablePath(NULL, 0, &dirname_length);
  if (length > 0) {
    path = (char *) malloc(length + 1);
    if (!path)
      abort();
    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';

//    printf("executable path: %s\n", path);
    char separator = path[dirname_length];
    path[dirname_length] = '\0';
//    printf("  dirname: %s\n", path);
//    printf("  basename: %s\n", path + dirname_length + 1);
    dir_path = std::string(path) + separator;
    free(path);
  } else {
    throw std::runtime_error("coulndt get path");
  }
  return dir_path;
}

int main(int argc, char *argv[]) {
  using namespace wolf;

  Logger::setupLogger(get_dir_path());
  Logger &logger = Logger::getLogger();
  logger.info("Configuring STXXL");
  stxxl::config *cfg = stxxl::config::get_instance();
  // create a disk_config structure.
  stxxl::disk_config
      disk(get_dir_path() + "queue.tmp", 0, get_dir_path()[get_dir_path().size() - 1] == '/' ? "syscall" : "wincall");
  disk.autogrow = true;
  disk.unlink_on_open = true;
  disk.delete_on_exit = true;
  disk.direct = stxxl::disk_config::DIRECT_TRY;
  cfg->add_disk(disk);

  logger.info("Parsing command line arguments");
  cxxopts::Options opts(argv[0], " - example command line options");

  std::string bootstrap_servers, influx_ip, es_ip;
  opts.add_options()
      ("bootstrap_servers",
       "Servers of kafka, default 10.0.11.162:9092",
       cxxopts::value<std::string>(bootstrap_servers)->default_value("10.0.11.162:9092"))
      ("influx_ip",
       "Ip address of influx, default localhost",
       cxxopts::value<std::string>(influx_ip)->default_value("localhost"))
      ("es_ip",
       "Ip address of influx, default localhost",
       cxxopts::value<std::string>(es_ip)->default_value("localhost"));
  opts.parse(argc, argv);

  logger.info("Parsed arguments:");
  logger.info("bootstrap_servers:    " + bootstrap_servers);
  logger.info("influx_ip: " + influx_ip);
  logger.info("es_ip: " + es_ip);

  pipeline p = pipeline(argc, argv, false);

  p.register_plugin(
      create<kafka_in>("^metrics-.*", bootstrap_servers, "metrics_forwarder_test")->register_output(
          create<string_to_json>()->register_output(
              create<lambda>(
                  [](json &message) {
                    message.assign_string(std::string(message["message"].get_string() + "\n"));
                  }
              )->register_output(
                  create<stats>()->register_output(
                      create<collate>()
                          ->register_output(
//                              create<cout>()
                              create<http_out>(influx_ip, "8086", "/write?db=metric_db")
                          )
                  )
              )
          )
      )
  );

  p.register_plugin(
      create<kafka_in>("^parsed_logs-.*", bootstrap_servers, "logs_forwarder_test1")->register_output(
//      create<generator>()->register_output(
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
                                         extras::get_time()},
                                     {"_type", "logs"}
                                 }}
                             })
                    ) + "\n" + std::string(message.get_string() + "\n")
                );
              }
          )->register_output(
              create<collate>()->register_output(
                  create<http_out>(es_ip, "9200", "/_bulk")
//                  create<cout>()
              )
          )
      )
  );

  p.run();
  return 0;

}
