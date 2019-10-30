#include <utility>

//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_KAFKA_OUT_H
#define WOLF_KAFKA_OUT_H

#include <base/plugins/plugin.h>
#include <cppkafka/cppkafka.h>
#include <base/plugins/mutexed_plugin.h>

namespace wolf {

class kafka_out : public base_plugin {
 public:
  kafka_out(event_option<std::string> topic,
            const option<int> &partitions,
            cppkafka::Configuration conf)
      : topic(std::move(topic)),
        partitions(partitions->value()),
        conf(std::move(conf)) {
  }
  using config = cppkafka::Configuration;

 protected:
  using producer = cppkafka::BufferedProducer<std::string>;

  void start() override {
    p = std::unique_ptr<producer>(new producer(conf));
    p->set_max_buffer_size(32000);
    p->set_flush_method(producer::FlushMethod::Sync);

    flusher = std::thread([&]() {
      while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        p->flush();
      }
    });
  }

  void stop() override {
    running = false;
    flusher.join();
    p->flush();
  }

  void process(json &&message) override {
    p->add_message(
        p->make_builder(topic->value(message))
            .partition(get_partition())
            .payload(std::move(message.as<std::string>())));
  }

  bool is_full() override {
    return p->get_pending_acks() > 64000;
  }
 private:

  int get_partition() {
    return current_partition++ % partitions;
  }

  std::unique_ptr<producer> p;
  config conf;
  event_option<std::string> topic;
  std::string broker_list;
  int partitions;
  std::atomic<int> current_partition{0};
  std::atomic<bool> running{true};
  std::thread flusher;
};

}

#endif //WOLF_KAFKA_OUT_H
