//
// Created by lamanchy on 9/29/18.
//

#ifndef WOLF_TCP_OUT_H
#define WOLF_TCP_OUT_H


#include <base/plugin.h>
#include <asio.hpp>

namespace wolf {

class tcp_out : public plugin {
public:
  tcp_out(std::string host, std::string port)
      : plugin(), io_context_(), socket_(io_context_), host(host), port(port) {
    std::cout << "tcp plugin created" << std::endl;
    socket_ = asio::ip::tcp::socket(io_context_);
    std::cout << "tcp socket created" << std::endl;
  }

protected:
  void process(json &&message) override {

    lock.lock();
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(std::move(message.get_string()));
    check_if_full();
    lock.unlock();

    if (!write_in_progress) {
      do_write();
    }
  }

  void start() override {
    do_connect();
  }

  void stop() override {
    std::cout << "stop" << std::endl;
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
        std::cout << "WARNING sending of tcp message failed: " << e.what() << std::endl;
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
    while (true) {
      try {
        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, port);
        asio::connect(socket_, endpoints);
      } catch (std::exception &e) {
        std::cout << "tcp connect failed: " << e.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }
      break;
    }
  }

  void check_if_full() {
    is_full_ = write_msgs_.size() > plugin::buffer_size;
  }

  asio::io_context io_context_;
  asio::ip::tcp::socket socket_;
  std::deque<std::string> write_msgs_;
  std::mutex lock;
  std::atomic<bool> is_full_{false};

  std::string host;
  std::string port;
};

}


#endif //WOLF_TCP_OUT_H
