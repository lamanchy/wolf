#ifndef WOLF_GENERATOR_H
#define WOLF_GENERATOR_H


#include <base/plugins/threaded_plugin.h>

namespace wolf {

class generator : public threaded_plugin {
  unsigned i = 0;
  unsigned count = 10;

public:
  void register_options(options &opts) {
//    opts.add_options("Generator")
//        ("c,count", "Number of generated messages", cxxopts::value<unsigned>(count));
  }

  void run() override {
//    Plugin::is_thread_processor = true;
//    sleep(100);
//    running = false;
    std::cout << i << " " << count << " " << running << std::endl;
    while (running) {
      json message = {
          {"happy", true},
          {"message", std::string(
              "2018-06-21 16:40:00,092 pool-8-thread-1                  INFO icReportGeneratorJob                                               - Checking if there are unprocessed statistic report that must be generated and sent by an email or save to the file.")},
          {"pi",      i++}
      };
      output(json(std::string("2018-06-21 16:40:00,092 pool-8-thread-1                  INFO icReportGeneratorJob                                               - Checking if there are unprocessed statistic report that must be generated and sent by an email or save to the file.")));
      std::cout << i << std::endl;
//      output(json(std::string("cpu_load_short,host=server01,region=us-west value=0.64 " + std::to_string(1434000000000000000l + i*1000000000l))));
//      if (i % 1 == 0) std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//      std::this_thread::yield();
//      if (i % (count/20) == 0) {
//        std::cerr << outputs.at("default")->front_queue.size() << std::endl;
//        std::cerr << outputs.at("default")->back_queue.size() << std::endl;
//        std::cerr << outputs.at("default")->persistent_queue.size() << std::endl;
//      }
      if (i > count) {
        running = false;
      }
//      sleep(1);
//      running = false; ///////////////////////////////////////////////////////////// TEST
    }
  }
};

}

#endif //WOLF_GENERATOR_H
