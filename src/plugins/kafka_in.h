//
// Created by lamanchy on 14.11.18.
//

#ifndef WOLF_KAFKA_IN_H
#define WOLF_KAFKA_IN_H

#include <cppkafka/cppkafka.h>
#include <base/plugins/mutexed_plugin.h>
#include <base/plugins/threaded_plugin.h>
#include <stdio.h>

namespace wolf {

class kafka_in : public threaded_plugin {
 public:
  kafka_in(std::string topic, std::string broker_list, std::string group_id)
      : topic(std::move(topic)), broker_list(broker_list), group_id(group_id) {

    config = {
        {"metadata.broker.list", broker_list}
    };
  }

 protected:

  void run() override {
    using namespace cppkafka;

    // Construct the configuration
    Configuration config = {
        {"metadata.broker.list", broker_list},
        {"group.id", group_id},
        {"client.id", group_id},
        {"auto.offset.reset", "earliest"},
        {"queued.max.messages.kbytes", 64},
        {"fetch.max.bytes", 64 * 1024},
        {"enable.auto.commit", true},
        {"heartbeat.interval.ms", 10000},
        {"session.timeout.ms", 50000},
        {"metadata.max.age.ms", 300000}
    };

    // Create the consumer
    Consumer consumer(config);

    // Print the assigned partitions on assignment
    consumer.set_assignment_callback([](const TopicPartitionList &partitions) {
      std::cout << "Got assigned: " << partitions << std::endl;
    });

    // Print the revoked partitions on revocation
    consumer.set_revocation_callback([](const TopicPartitionList &partitions) {
      std::cout << "Got revoked: " << partitions << std::endl;
    });

    // Subscribe to the topic
    consumer.subscribe({topic});

    std::cout << "Consuming messages from topic " << topic << std::endl;

    // Now read lines and write them into kafka
    while (running) {
      // Try to consume a message
      Message msg = consumer.poll();
      if (msg) {
        // If we managed to get a message
        if (msg.get_error()) {
          // Ignore EOF notifications from rdkafka
          if (not msg.is_eof()) {
            logger.warn("Received error notification: " + msg.get_error().to_string());
          }
        } else {
          // Print the key (if any)
//          if (msg.get_key()) {
//            std::cout << msg.get_key() << " -> ";
//          }

          json j = json(std::string(msg.get_payload()));
          j.metadata = {
              {"topic", msg.get_topic()}
          };
          output(std::move(j));

//                        consumer.commit(msg);
        }
      }
    }
  }

  cppkafka::Configuration config;
  std::string topic;
  std::string broker_list, group_id;
};

}

#endif //WOLF_KAFKA_IN_H
