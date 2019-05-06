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

class kafka_out : public plugin {
 public:
  kafka_out(option<std::string> topic, unsigned partitions, std::string broker_list)
      : topic(std::move(topic)), partitions(partitions), broker_list(broker_list) {
    config = {
        {"metadata.broker.list", broker_list},
        {"compression.type", "lz4"}
//        ,
//        { "topic.metadata.refresh.interval.ms", 20000 }
        ,
        {"debug", "broker,topic,msg"}
    };
  }

 protected:
  using producer = cppkafka::BufferedProducer<std::string>;
  using configuration = cppkafka::Configuration;

  void start() override {
    p = std::unique_ptr<producer>(new producer(config));
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
        p->make_builder(topic->get_value(message))
            .partition(get_partition())
            .payload(std::move(message.as<std::string>())));
  }

  bool is_full() override {
    return p->get_pending_acks() > 64000;
  }
 private:

  unsigned get_partition() {
    return current_partition++ % partitions;
  }

  std::unique_ptr<producer> p;
  configuration config;
  option<std::string> topic;
  std::string broker_list;
  unsigned partitions;
  std::atomic<unsigned> current_partition{0};
  std::atomic<bool> running{true};
  std::thread flusher;
};

}

#endif //WOLF_KAFKA_OUT_H
