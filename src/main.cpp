#include <plugins/tcp_in.h>
#include <base/plugin.h>
#include <base/pipeline.h>
#include <plugins/cout.h>
#include <plugins/generator.h>
#include <serializers/line.h>
#include <plugins/string_to_json.h>

//#include <plugins/kafka_out.h>

#include <plugins/collate.h>
#include <date/tz.h>
#include <plugins/ysoft/add_local_info.h>
#include <plugins/json_to_string.h>
#include <plugins/ysoft/normalize_nlog_logs.h>
#include <plugins/ysoft/normalize_serilog_logs.h>
#include <plugins/ysoft/normalize_log4j2_logs.h>


int main(int argc, char *argv[]) {
  using namespace wolf;

  plugin::pointer common_processing = create<add_local_info>()->register_output(
      create<json_to_string>()->register_output(
          create<cout>()
//          create<kafka_out>("test", 1)
      )
  );


  pipeline p = pipeline(argc, argv).register_plugin(
      create<tcp_in<line>>("nlog")->register_output(
          create<string_to_json>()->register_output(
              create<normalize_nlog_logs>()->register_output(
                  common_processing
              )
          )
      )
  ).register_plugin(
      create<tcp_in<line>>("log4j2")->register_output(
          create<string_to_json>()->register_output(
              create<normalize_log4j2_logs>()->register_output(
                  common_processing
              )
          )
      )
  ).register_plugin(
      create<tcp_in<line>>("serilog")->register_output(
          create<string_to_json>()->register_output(
              create<normalize_serilog_logs>()->register_output(
                  common_processing
              )
          )
      )
  );
  p.run();


  return 0;
}
