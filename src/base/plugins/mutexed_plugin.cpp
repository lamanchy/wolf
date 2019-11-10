#include <base/plugins/mutexed_plugin.h>

void wolf::mutexed_plugin::prepare(wolf::json &&message) {
  std::lock_guard<std::mutex> lg(lock);
  process(std::move(message));
}
