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
    consumer.set_assignment_callback([this](const TopicPartitionList &partitions) {
      std::ostringstream s;
      s << "Got assigned: " << partitions;
      logger.info(s.str());
    });

    // Print the revoked partitions on revocation
    consumer.set_revocation_callback([this](const TopicPartitionList &partitions) {
      std::ostringstream s;
      s << "Got revoked: " << partitions << std::endl;
      logger.info(s.str());
    });

    // Subscribe to the topic
    consumer.subscribe({topic});

    strategy = std::unique_ptr<cppkafka::RoundRobinPollStrategy>(new cppkafka::RoundRobinPollStrategy(consumer));

    logger.info("Consuming messages from topic " + topic);
  }

  void loop() override {
    using namespace cppkafka;
    // Try to consume a message
    Message msg = strategy->poll(std::chrono::milliseconds(0));
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
      get_loop_sleeper().decrease_sleep_time();
    } else {
      get_loop_sleeper().increasing_sleep();
    }
  }

 private:
  std::string topic;
  cppkafka::Consumer consumer;
  std::unique_ptr<cppkafka::RoundRobinPollStrategy> strategy{nullptr};
};

}
}


