#include <cppkafka/cppkafka.h>
#include <iostream>

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

using namespace std;
using namespace cppkafka;

int main() {
  // Create the config
  Configuration config = {
      {"metadata.broker.list", "localhost:9092"},
      {"compression.type", "gzip"}
  };

  // Create the producer
  BufferedProducer<string> producer(config);



//  producer.set_produce_success_callback([](const Message &msg) {
//    cout << "Successfully produced message with payload " << msg.get_payload() << endl;
//  });


  // Produce a message!
//  std::vector<string> strings;
//  producer.set_timeout(chrono::milliseconds(30000));
//  cout << to_string(producer.get_timeout().count()) << endl;

  auto t1 = Clock::now();
  int total = 10000000;
  for (int i = 0; i < total; ++i) {
    string t =
        "dnafeogdfkpanfrjkgojdosdfmtodnofpenfjlkandfoasnangjrgpokp[dewopirdnlvkdnczkds[pkeopjegknd,mc;laskd[pietoprjndlsnalslkdf[pkafepkgwwrg'wef.ejoisefmaekfopkepjigsrngvkjadfiojeiotlakjrglkjafelknzs,fmnarlkj[tasnkfsjdfghkjgkasjfhaujhfakjbfvarkjnfaeknfaekjnfakjgnkjadnfkjnafskjfnakgjnakjfnadfkjnasdkfjnavr[iiojdflkmzv"
            + to_string(i);
//    strings.push_back(t);
    producer.produce(MessageBuilder("test").partition(0).payload(t));
  }

  producer.flush();

  auto t2 = Clock::now();
  std::cout << "Delta t2-t1: "
            << 1000 * total / std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
            << " mili" << std::endl;
}