#include <base/sleeper.h>
#include "plugin.h"

namespace wolf {

thread_local bool base_plugin::is_thread_processor = false;

std::atomic<base_plugin::id_type> base_plugin::id_counter{0};

bool base_plugin::process_buffer() {
  if (q.is_empty()) return false;
  if (are_outputs_full()) return false;
  q.try_pop([this](json &&message) { safe_prepare(std::move(message)); });
  return true;
}

void base_plugin::safe_prepare(json &&message) {
  try {
    prepare(std::move(message));
  } catch (std::exception &ex) {
    logger.error("error in " + std::string(typeid(*this).name()) +
        " when processing message: " + std::string(ex.what()));
  }
}

bool base_plugin::are_outputs_full() {
  if (queue::persistent)
    return is_full();

  if (is_full())
    return true;

  for (auto &output : outputs) {
    if (output.second->q.is_full()) return true;
    if (output.second->are_outputs_full()) return true;
  }
  return false;
}

void base_plugin::receive(json &&message, bool non_blocking) {
  if (is_processor()) {
    if (!is_full()) safe_prepare(std::move(message));
    else buffer(std::move(message));
  } else {
    if (not non_blocking and q.is_full()) {
      sleeper sleeper;
      while (q.is_full())
        sleeper.sleep();
    }
    buffer(std::move(message));
  }
}
void base_plugin::buffer(json &&message) {
  q.push(std::move(message));
}

}