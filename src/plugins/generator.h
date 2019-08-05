#ifndef WOLF_GENERATOR_H
#define WOLF_GENERATOR_H

#include <base/plugins/threaded_plugin.h>

namespace wolf {

class generator : public threaded_plugin {
  unsigned i = 0;
  unsigned count = 10000;

 public:
  void register_options(options &opts) {
//    opts.add_options("Generator")
//        ("c,count", "Number of generated messages", cxxopts::value<unsigned>(count));
  }

 protected:
  void setup() override {
    std::cout << i << " " << count << " " << running << std::endl;
  }

  void loop() override {
    json message = {
        {"happy", true},
        {"message", std::string(
            "2018-06-21 16:40:00,092 pool-8-thread-1                  INFO icReportGeneratorJob                                               - Checking if there are unprocessed statistic report that must be generated and sent by an email or save to the file.")},
        {"pi", i++}
    };
    output(json({
                    {"@timestamp", "2018-02-27T14:11:12.906Z"},
                    {"actionId", "0febc24c-82f3-46ab-bdfd-dc4e7aae2a1c"},
                    {"actionName", "SpoolerSimulator.Controllers.SpoolersController.SendJob (SpoolerSimulator)"},
                    {"activityId", "d8c73212-601f-44d5-b426-0fbb51b4ffd9"},
                    {"component", "SpoolerSimulator"},
                    {"group", "default"},
                    {"host", "lamanchy-Latitude-E7450"},
                    {"jobGuid", "880251d7-fcca-4e19-ba14-a620f4b367a3"},
                    {"level", "INFO"},
                    {"logId", "spoolerJobTicketAccepted"},
                    {"message", "Job ticket with 880251d7-fcca-4e19-ba14-a620f4b367a3 has been successfully sent."},
                    {"requestId", "0HLBTTFQC6TSJ:00000001"},
                    {"requestPath", "/spoolers/76838cee-bed6-4c3e-9e79-a8a800387473/job"},
                    {"sourceContext", "Spooler.Spooler"}
    }));
//    output(json(std::string(
//        "2018-06-21 16:40:00,092 pool-8-thread-1                  INFO icReportGeneratorJob                                               - Checking if there are unprocessed statistic report that must be generated and sent by an email or save to the file.")));
//    std::cout << i << std::endl;
//      output(json(std::string("cpu_load_short,host=server01,region=us-west value=0.64 " + std::to_string(1434000000000000000l + i*1000000000l))));
//      if (i % 1 == 0) std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//      std::this_thread::yield();
//      if (i % (count/20) == 0) {
//        std::cerr << outputs.at("default")->front_queue.size() << std::endl;
//        std::cerr << outputs.at("default")->back_queue.size() << std::endl;
//        std::cerr << outputs.at("default")->persistent_queue.size() << std::endl;
//      }
    if (i > count) {
      end_loop();
    }
//      sleep(1);
//      running = false; ///////////////////////////////////////////////////////////// TEST
  }
};

}

#endif //WOLF_GENERATOR_H
