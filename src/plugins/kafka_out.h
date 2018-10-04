#include <utility>

//
// Created by lamanchy on 8/14/18.
//

#ifndef WOLF_KAFKA_OUT_H
#define WOLF_KAFKA_OUT_H


#include <base/plugin.h>
#include <cppkafka/cppkafka.h>
#include <base/mutexed_plugin.h>


namespace wolf {

class kafka_out : public plugin {
public:
  kafka_out(std::string topic, unsigned partitions, std::string broker_list)
      : topic(std::move(topic)), partitions(partitions), broker_list(broker_list) { }

protected:
  using producer = cppkafka::BufferedProducer<std::string>;
  using configuration = cppkafka::Configuration;

  void start() override {
    p = std::unique_ptr<producer>(new producer(config));
    p->set_max_buffer_size(1000);
  }

  void stop() override {
    p->flush();
  }

  void register_options(options &opts) override {
//    opts.add_options("Kafka output")
//        ("x,broker_list", "Broker list", cxxopts::value<std::string>(broker_list));
  }

  void validate_options(parse_result &result) override {
//    if (result.count("broker_list") == 0) {
//      throw std::runtime_error("broker_list option not specified");
//    }

    config = {
        { "metadata.broker.list", broker_list }
        ,
        { "compression.type", "lz4" }
    };
  }

  void process(json && message) override {
    p->produce(
        p->make_builder(topic)
        .partition(get_partition())
        .payload(std::move(message.as<std::string>())));
  }

  bool is_full() override {
    return p->get_pending_acks() > 1000;
  }
private:

  unsigned get_partition() {
    return current_partition++ % partitions;
  }

  std::unique_ptr<producer> p;
  configuration config;
  std::string topic;
  std::string broker_list;
  unsigned partitions;
  std::atomic<unsigned> current_partition{0};

};

}

#endif //WOLF_KAFKA_OUT_H
