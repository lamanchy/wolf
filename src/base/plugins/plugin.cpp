#include "plugin.h"

namespace wolf {

thread_local bool plugin::is_thread_processor = false;

bool plugin::persistent = true;

unsigned plugin::buffer_size = 128;

std::atomic<plugin::id_type> plugin::id_counter{0};

namespace serializer = tao::json::cbor;

void plugin::empty_front_queue() {
  // front_queue_mutex is locked, must be unlocked
  swappable = false;
  front_processing_mutex.lock();
  front_queue.swap(front_processing_queue);
  front_queue_mutex.unlock();

  while (!front_processing_queue.empty()) {
    std::string s(serializer::to_string(std::move(front_processing_queue.front())));
    for (char c: get_serialized_size(s.size())) tmp_front_buffer1.push_back(c);
    tmp_front_buffer1.insert(tmp_front_buffer1.end(), s.begin(), s.end());
    front_processing_queue.pop();
  }
  tmp_front_buffer2.append(gzip::compress(tmp_front_buffer1.data(), tmp_front_buffer1.size(), Z_BEST_SPEED));
  tmp_front_buffer1.clear();

  persistent_queue_mutex.lock();
  for (char c: get_serialized_size(tmp_front_buffer2.size())) persistent_queue.push(c);
  for (char c: tmp_front_buffer2) persistent_queue.push(c);
  persistent_queue_mutex.unlock();
  tmp_front_buffer2.clear();
  front_processing_mutex.unlock();
}

bool plugin::process_buffer() {
  if (are_outputs_full()) return false;
  back_queue_mutex.lock();
  if (back_queue.empty()) {
    if (swappable) {
      // premise, that fq could be swappable
      front_queue_mutex.lock();
      if (swappable) {
        // is swappable
        if (!front_queue.empty()) {
//          if (front_queue.front()["pi"] < 2500000 - 10000) {
//            front_queue_mutex.unlock();
//            std::this_thread::sleep_for(std::chrono::milliseconds(11));
//            back_queue_mutex.unlock();
//            return true;
//          }
          front_queue.swap(back_queue);
          front_queue_mutex.unlock();
          process_back_queue_front();
          return true;
        } else {
          front_queue_mutex.unlock();
          back_queue_mutex.unlock();
          return false;
        }
      }
      front_queue_mutex.unlock();
    }
    // is not swappable

    // swap queues
    back_processing_mutex.lock();
    back_processing_queue.swap(back_queue);
    back_queue_mutex.unlock();

    // prepare next queue
    persistent_queue_mutex.lock();
    if (persistent_queue.empty()) {
      back_processing_mutex.unlock();
      // trying to set swappable
      if (front_processing_mutex.try_lock()) {
        persistent_queue_mutex.unlock();
        if (front_queue_mutex.try_lock_for(std::chrono::milliseconds(1))) {
          swappable = true;
          front_queue_mutex.unlock();
        }
        front_processing_mutex.unlock();
      } else {
        persistent_queue_mutex.unlock();
      }
      return true;
    }

    // persistent queue not empty, let's prepare queue
    std::string s;
    s.reserve(4);
    while (true) {
      s.push_back(persistent_queue.front());
      persistent_queue.pop();
      if (s.back() == string_serializer_end) break;
    }
    size_t size = get_deserialized_size(s);

    tmp_back_buffer1.reserve(std::max(size, tmp_back_buffer1.capacity()));
    char *ptr = &tmp_back_buffer1[0];
    char *back_ptr = &tmp_back_buffer1[size];
    while (ptr != back_ptr) {
      *ptr++ = persistent_queue.front();
      persistent_queue.pop();
    }
    persistent_queue_mutex.unlock();

    std::string decompressed(gzip::decompress(tmp_back_buffer1.data(), size));
    tmp_back_buffer1.clear();

    ptr = &decompressed[0];
    back_ptr = &decompressed[decompressed.size()];
    while (ptr != back_ptr) {
      s.clear();
      while (true) {
        s.push_back(*ptr++);
        if (s.back() == string_serializer_end) break;
      }
      size = get_deserialized_size(s);

      tao::json::events::to_value consumer;
      tao::json::cbor::events::from_string(consumer, tao::basic_string_view<char>(ptr, size));
      back_processing_queue.push(std::move(consumer.value));
      ptr += size;
    }
    back_processing_mutex.unlock();
    return true;
  }

  process_back_queue_front();
  return true;

}
void plugin::safe_prepare(json &&message) {
  try {
    prepare(std::move(message));
  } catch (std::exception &ex) {
    logger.error("error in " + std::string(typeid(*this).name()) +
        " when processing message: " + std::string(ex.what()));
  }
}
bool plugin::are_outputs_full() {
  if (plugin::persistent)
    return is_full();

  if (is_full())
    return true;

  for (auto &output : outputs) {
    if (output.second->are_outputs_full()) return true;
  }
  return false;
}

void plugin::process_back_queue_front() {
//      const unsigned batch_size = 64;
//      std::queue<json> q;
//      while (q.size() < batch_size and not back_queue.empty()) {
//        q.push(std::move(back_queue.front()));
//        back_queue.pop();
//      }
//      back_queue_mutex.unlock();
//      while (not q.empty()) {
//        safe_prepare(q.front());
//        q.pop();
//      }

  json message = std::move(back_queue.front());
  back_queue.pop();
  back_queue_mutex.unlock();
  safe_prepare(std::move(message));
}
void plugin::receive(json &&message) {
  if (plugin::is_thread_processor && !is_full()) {
    safe_prepare(std::move(message));
  } else {
    buffer(std::move(message));
  }
}
void plugin::buffer(json &&message) {
  front_queue_mutex.lock();

  if (not plugin::persistent) {
    while (front_queue.size() >= plugin::buffer_size - 2) {
      front_queue_mutex.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (plugin::is_thread_processor && !is_full()) {
        safe_prepare(std::move(message));
        return;
      }
      front_queue_mutex.lock();
    }
  }
  front_queue.push(message);

//    if (front_queue.size() < plugin::buffer_size - 1) front_queue.push(message);
  if (front_queue.size() >= plugin::buffer_size) {
    empty_front_queue(); // unlocks fqm
    return;
  }
  front_queue_mutex.unlock();
}
std::string plugin::get_serialized_size(size_t size) {
  std::string result;
  result.reserve(4);
  while (size > 0) {
    result.push_back((char) (size % string_serializer_divisor));
    size /= string_serializer_divisor;
  }
  result.push_back(string_serializer_end);
  return result;
}
size_t plugin::get_deserialized_size(std::string serialized_form) {
  size_t result = 0;
  size_t base = 1;
  serialized_form.pop_back();
  for (char &c : serialized_form) {
    result += c * base;
    base *= string_serializer_divisor;
  }
  return result;
}

}