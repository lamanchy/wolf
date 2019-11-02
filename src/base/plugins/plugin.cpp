#include <base/sleeper.h>
#include <base/pipeline_status.h>
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
  if (pipeline_status::is_persistent())
    return is_full();

  if (is_full())
    return true;

  for (auto &output : outputs) {
    if (output.second->q.is_full()) return true;
    if (output.second->are_outputs_full()) return true;
  }
  return false;
}

void base_plugin::receive(json &&message, bool non_blocking, bool always_buffer) {
  if (is_processor()) {
    if (always_buffer or is_full()) buffer(std::move(message));
    else safe_prepare(std::move(message));
  } else {
    if (not non_blocking and q.is_full()) {
      sleeper sleeper;
      while (q.is_full())
        sleeper.increasing_sleep();
    }
    buffer(std::move(message));
  }
}
void base_plugin::buffer(json &&message) {
  q.push(std::move(message));
}

void base_plugin::do_stop() {
  logger.trace("stopping " + std::string(typeid(*this).name()));
  if (num_of_parents > 0)
    return;

  logger.trace("no parents");
  sleeper sleeper;
  while (not q.is_empty())
    sleeper.increasing_sleep();

  stop();

  for (const auto& output : outputs){
    output.second->num_of_parents -= 1;
    output.second->do_stop();
  }

}

std::vector<base_plugin::id_type> base_plugin::get_all_outputs_ids() {
  auto res = std::vector<id_type>({id});
  for (const auto& output : outputs)
    for (auto _id : output.second->get_all_outputs_ids())
      res.push_back(_id);

  return res;
}

plugin base_plugin::register_named_output(const std::string &output_name, const plugin &plugin) {
  auto it = outputs.find(output_name);
  if (it != outputs.end())
    logger.fatal("plugin already registered output named: " + output_name);

  auto ids = plugin->get_all_outputs_ids();
  auto it2 = std::find(ids.begin(), ids.end(), id);
  if (it2 != ids.end())
    logger.fatal("Cannot register plugins into a loop.");

  plugin->num_of_parents += 1;
  outputs.emplace(std::make_pair(output_name, plugin));

  return shared_from_this();
}

}