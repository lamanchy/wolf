#pragma once

#include <cppkafka/cppkafka.h>
#include <base/plugins/mutexed_plugin.h>
#include <base/plugins/threaded_plugin.h>
#include <stdio.h>
#include <utility>
#include <plugins/common/kafka.h>

namespace wolf {
namespace kafka {

class input : public threaded_plugin {
 public:
  input(const static_option<std::string> &topic,
        config conf)
      : topic(topic->value()),
        consumer(std::move(conf)) {
    non_processors_should_block();
  }

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
    // Try to consume a message
    Message msg = consumer.poll(std::chrono::milliseconds(0));
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
    } else {
      get_loop_sleeper().sleep_for(std::chrono::seconds(1));
    }

  }

 private:
  std::string topic;
  cppkafka::Consumer consumer;
};

}
}


