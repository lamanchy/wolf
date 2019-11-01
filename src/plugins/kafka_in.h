//
// Created by lamanchy on 14.11.18.
//

#ifndef WOLF_KAFKA_IN_H
#define WOLF_KAFKA_IN_H

#include <cppkafka/cppkafka.h>
#include <base/plugins/mutexed_plugin.h>
#include <base/plugins/threaded_plugin.h>
#include <stdio.h>
#include <utility>

namespace wolf {

class kafka_in : public threaded_plugin {
 public:
  using config = cppkafka::Configuration;

  kafka_in(const option<std::string> &topic,
           config conf)
      : topic(topic->value()),
        consumer(std::move(conf)) {}

 protected:

  void setup() override {
    using namespace cppkafka;
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

    logger.info("Consuming messages from topic " + topic);
  }

  void loop() override {
    using namespace cppkafka;
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

 private:
  std::string topic;
  cppkafka::Consumer consumer;
};

}

#endif //WOLF_KAFKA_IN_H
