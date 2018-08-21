#include "plugin.h"


namespace wolf {

thread_local bool plugin::is_thread_processor = false;

unsigned plugin::buffer_size = 1024;

std::atomic<plugin::id_type> plugin::id_counter{0};

namespace serializer = tao::json::cbor;

void plugin::empty_front_queue() {
  // front_queue_mutex is locked, must be unlocked
  swappable = false;
  front_processing_mutex.lock();
  front_queue.swap(front_processing_queue);
  front_queue_mutex.unlock();

  while (not front_processing_queue.empty()) {
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
  if (is_full()) return false;
  back_queue_mutex.lock();
  if (back_queue.empty()) {
    if (swappable) {
      // premise, that fq could be swappable
      front_queue_mutex.lock();
      if (swappable) {
        // is swappable
        if (not front_queue.empty()) {
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

}