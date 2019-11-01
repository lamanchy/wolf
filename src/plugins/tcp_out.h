//
// Created by lamanchy on 9/29/18.
//

#ifndef WOLF_TCP_OUT_H
#define WOLF_TCP_OUT_H

#include <base/plugins/plugin.h>
#include <asio.hpp>

namespace wolf {

template<typename Serializer>
class tcp_out : public base_plugin {
 public:
  tcp_out(const option<std::string> &host,
          const option<std::string> &port)
      : base_plugin(),
        io_context_(),
        socket_(io_context_),
        host(host->value()),
        port(port->value()) {
    socket_ = asio::ip::tcp::socket(io_context_);
  }

 protected:
  void process(json &&message) override {
    lock.lock();
    bool write_in_progress = not write_msgs_.empty();
    write_msgs_.push_back(s.serialize(std::move(message)));
    check_if_full();
    lock.unlock();

    if (not write_in_progress) {
      do_write();
    }
  }

  void stop() override {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      std::lock_guard<std::mutex> lg(lock);
      if (write_msgs_.empty()) {
        break;
      }
    }
    should_connect = false;
    socket_.close();
  }

  bool is_full() override {
    return is_full_;
  }

 private:
  void do_write() {
    while (true) {
      try {
        asio::write(socket_, asio::buffer(write_msgs_.front().data(),
                                          write_msgs_.front().length()));
      } catch (std::exception &e) {
        logger.warn("sending of tcp message failed: " + std::string(e.what()));
        do_connect();
        continue;
      }

      std::lock_guard<std::mutex> lg(lock);
      write_msgs_.pop_front();
      check_if_full();
      if (write_msgs_.empty()) {
        break;
      }
    }
  }

  void do_connect() {
    try {
      asio::ip::tcp::resolver resolver(io_context_);
      auto endpoints = resolver.resolve(host, port);
      asio::connect(socket_, endpoints);
    } catch (std::exception &e) {
      logger.warn("tcp connect failed: " + std::string(e.what()));
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  void check_if_full() {
    is_full_ = write_msgs_.size() > queue::buffer_size;
  }

  asio::io_context io_context_;
  asio::ip::tcp::socket socket_;
  std::deque<std::string> write_msgs_;
  std::mutex lock;
  std::atomic<bool> is_full_{false};
  std::atomic<bool> should_connect{true};

  std::string host;
  std::string port;

  Serializer s;
};

}

#endif //WOLF_TCP_OUT_H
