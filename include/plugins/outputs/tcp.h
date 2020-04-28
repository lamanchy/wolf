#pragma once

#include <base/plugins/base_plugin.h>
#include <asio.hpp>
#include <base/pipeline_status.h>

namespace wolf {
namespace tcp {

class output : public base_plugin {
 public:
  output(const static_option<std::string> &host,
         const static_option<std::string> &port)
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
    _size += message.get_string().size();
    write_msgs_.push_back(std::move(message));
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
    socket_.close();
  }

  bool is_full() override {
    return _size > 65536;
  }

 private:
  void do_write() {
    while (true) {
      auto &front = write_msgs_.front().get_string();
      try {
        asio::write(socket_, asio::buffer(front.data(),
                                          front.length()));
      } catch (std::exception &e) {
        logger.warn("[tcp_out] sending of tcp message failed: " + std::string(e.what()));
        do_connect();
        continue;
      }

      _size -= front.size();
      std::lock_guard<std::mutex> lg(lock);
      write_msgs_.pop_front();
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
      logger.info("[tcp_out] Successfully connected to " + host + ":" + port);
    } catch (std::exception &e) {
      logger.warn("[tcp_out] tcp connect failed: " + std::string(e.what()));
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  asio::io_context io_context_;
  asio::ip::tcp::socket socket_;
  std::deque<json> write_msgs_;
  std::mutex lock;
  std::atomic<unsigned int> _size{0};

  std::string host;
  std::string port;
};

}
}


