#include <cppkafka/cppkafka.h>

using namespace std;
using namespace cppkafka;

int main() {
  // Create the config
  Configuration config = {
      {"metadata.broker.list", "10.0.10.18:9092"}
  };

  // Create the producer
  Producer producer(config);

  // Produce a message!
  string message = "hey there!";
  producer.produce(MessageBuilder("test").partition(0).payload(message));
  producer.flush();
}