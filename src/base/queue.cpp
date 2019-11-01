//
// Created by lomic on 10/31/2019.
//

#include <thread>
#include <base/plugins/plugin.h>
#include "queue.h"

namespace wolf {
bool queue::persistent = true;
unsigned queue::buffer_size = 128;

void queue::push(json &&message) {
  front_queue_mutex.lock();
  size += message.size;
  front_queue.push(std::move(message));

  if (persistent and front_queue.size() >= queue::buffer_size) {
    empty_front_queue(); // unlocks fqm
    return;
  }
  front_queue_mutex.unlock();
}

void queue::try_pop(const std::function<void(json &&)> &fn) {
  back_queue_mutex.lock();
  if (back_queue.empty()) {
    // bq is empty, but size is not 0, so let's check,
    // where and if there's anything for us
    check_if_all_is_empty(); // unlocks bqm
    return;
  }

  do_pop(fn); // unlocks bqm
}
void queue::check_if_all_is_empty() {
  // let's check fq first, and swap it if necessary
  if (swappable) {
    // premise, that fq could be swappable
    front_queue_mutex.lock();
    if (swappable) {
      // fq is swappable, therefore it contains something to process,
      // or can be empty, if we were too slow
      if (!front_queue.empty()) {
        // fq is not empty, let's swap fq and bq
        front_queue.swap(back_queue);
        front_queue_mutex.unlock();
        back_queue_mutex.unlock();
        // could do do_pop, bq is not empty anymore
        // but try_pop will return true, which will make processor make to try again
        return;
      } else {
        front_queue_mutex.unlock();
        back_queue_mutex.unlock();
        // bq and fq are empty,
        // so return
        return;
      }
    }
    // fq is not swappable, we were too slow
    front_queue_mutex.unlock();
  }
  // is not swappable, therefore there are or were some events in persistent queue
  // let's go to empty it
  load_from_persistent_queue();
}
void queue::load_from_persistent_queue() {
  // swap queues bq and bpq
  back_processing_mutex.lock();
  back_processing_queue.swap(back_queue);
  back_queue_mutex.unlock();

  // prepare next bq queue, load from persistent queue
  persistent_queue_mutex.lock();
  // if pq is empty, we can try to lock the rest of queues and make fq and bq swappable again
  if (persistent_queue->empty()) {
    try_setting_swappable();
    return;
  }

  // persistent queue not empty, let's prepare bpq and empty some of it
  std::string s;
  s.reserve(4);
  while (true) {
    s.push_back(persistent_queue->front());
    persistent_queue->pop();
    if (s.back() == string_serializer_end) break;
  }
  size_t deserialized_size = get_deserialized_size(s);

  tmp_back_buffer1.reserve(std::max(deserialized_size, tmp_back_buffer1.capacity()));
  char *ptr = &tmp_back_buffer1[0];
  char *back_ptr = &tmp_back_buffer1[deserialized_size];
  while (ptr != back_ptr) {
    *ptr++ = persistent_queue->front();
    persistent_queue->pop();
  }
  persistent_queue_mutex.unlock();

  // we load stuff from pq, so now just to decompress and deserialize it
  std::string decompressed(gzip::decompress(tmp_back_buffer1.data(), deserialized_size));
  tmp_back_buffer1.clear();

  ptr = &decompressed[0];
  back_ptr = &decompressed[decompressed.size()];
  while (ptr != back_ptr) {
    s.clear();
    while (true) {
      s.push_back(*ptr++);
      if (s.back() == string_serializer_end) break;
    }
    deserialized_size = get_deserialized_size(s);

    tao::json::events::to_value consumer;
    tao::json::cbor::events::from_string(consumer, tao::basic_string_view<char>(ptr, deserialized_size));
    back_processing_queue.push(std::move(consumer.value));
    ptr += deserialized_size;
  }
  // all is prepared in bpq, if we try again, we find unswappable fq and bq,
  // therefore we'll go next and swap bq nad bpq, then check for pq emptiness and swappability
  back_processing_mutex.unlock();
}
void queue::try_setting_swappable() {
  back_processing_mutex.unlock();
  // trying to set swappable
  if (front_processing_mutex.try_lock()) {
    persistent_queue_mutex.unlock();
    // let's try to lock fq for at least one ms, so we by accident don't just get stuck on incoming event
    if (front_queue_mutex.try_lock_for(std::chrono::milliseconds(1))) {
      // yey, we locked everything needed and we're making fq and bq swappable again
      swappable = true;
      front_queue_mutex.unlock();
    }
    front_processing_mutex.unlock();
  } else {
    persistent_queue_mutex.unlock();
  }
}

namespace serializer = tao::json::cbor;

void queue::empty_front_queue() {
  // front_queue_mutex is locked, must be unlocked
  // to be loading stuff to pq, not swappable anymore
  swappable = false;
  front_processing_mutex.lock();
  front_queue.swap(front_processing_queue);
  front_queue_mutex.unlock();

  // serialize and decompress events to pq
  while (!front_processing_queue.empty()) {
    std::string s(serializer::to_string(front_processing_queue.front()));
    for (char c: get_serialized_size(s.size())) tmp_front_buffer1.push_back(c);
    tmp_front_buffer1.insert(tmp_front_buffer1.end(), s.begin(), s.end());
    front_processing_queue.pop();
  }
  tmp_front_buffer2.append(gzip::compress(tmp_front_buffer1.data(), tmp_front_buffer1.size(), Z_BEST_SPEED));
  tmp_front_buffer1.clear();

  persistent_queue_mutex.lock();
  if (persistent_queue == nullptr)
    persistent_queue = std::unique_ptr<stxxl::queue<char>>(new stxxl::queue<char>());
  for (char c: get_serialized_size(tmp_front_buffer2.size())) persistent_queue->push(c);
  for (char c: tmp_front_buffer2) persistent_queue->push(c);
  persistent_queue_mutex.unlock();
  tmp_front_buffer2.clear();
  front_processing_mutex.unlock();
}

void queue::do_pop(const std::function<void(json &&)> &fn) {
  json message = std::move(back_queue.front());
  back_queue.pop();
  size -= message.size;
  back_queue_mutex.unlock();
  fn(std::move(message));
}

std::string queue::get_serialized_size(size_t size) {
  std::string result;
  result.reserve(4);
  while (size > 0) {
    result.push_back((char) (size % string_serializer_divisor));
    size /= string_serializer_divisor;
  }
  result.push_back(string_serializer_end);
  return result;
}
size_t queue::get_deserialized_size(std::string serialized_form) {
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