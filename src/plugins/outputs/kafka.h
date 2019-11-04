#include <utility>

//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_KAFKA_OUT_H
#define WOLF_KAFKA_OUT_H

#include <base/plugins/base_plugin.h>
#include <cppkafka/cppkafka.h>
#include <base/plugins/mutexed_plugin.h>
#include <plugins/common/kafka.h>

namespace wolf {
namespace kafka {

class output : public threaded_plugin {
 public:
  output(event_option<std::string> topic,
            const option<int> &partitions,
            cppkafka::Configuration conf)
      : topic(std::move(topic)),
        partitions(partitions->value()),
        p(std::move(conf)) {
  }

 protected:
  using producer = cppkafka::BufferedProducer<std::string>;

  void setup() override {
    p.set_max_buffer_size(32000);
    p.set_flush_method(producer::FlushMethod::Sync);
  }

  void loop() override {
    p.flush();
    get_loop_sleeper().sleep_for(std::chrono::seconds(1));
  }

  void flush() override {
    p.flush();
  }

  void process(json &&message) override {
    p.add_message(
        p.make_builder(topic->value(message))
            .partition(get_partition())
            .payload(std::move(message.as<std::string>())));
  }

  bool is_full() override {
    return p.get_pending_acks() > 64000;
  }

 private:
  int get_partition() {
    return current_partition++ % partitions;
  }

  producer p;
  event_option<std::string> topic;
  int partitions;
  std::atomic<int> current_partition{0};
};

}
}

#endif //WOLF_KAFKA_OUT_H
