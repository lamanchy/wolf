//
// Created by lamanchy on 16.11.18.
//

#ifndef WOLF_HTTP_OUT_H
#define WOLF_HTTP_OUT_H

#include <base/plugins/threaded_plugin.h>

#include <asio/connect.hpp>
#include <asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace wolf {
class http_out : public threaded_plugin {
 public:
  http_out(std::string host, std::string port, std::string url) :
      host(host), port(port), url(url) {};

 protected:

  void loop() override {
    if (ioc.run_one() == 0) {
      i = 0;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      ioc.restart();
    }
  }

 protected:
  void process(json &&message) override {
    std::make_shared<session>(ioc, url, message.get_string())->run(host, port);

    ++i;
//    ioc.run();
  }

 private:
  asio::io_context ioc;

  std::string host, port, url, method;
  std::atomic<unsigned long> i{0};
 protected:

  bool is_full() override {
    return i >= 10;
  }

 private:
  using tcp = asio::ip::tcp;

  class session : public std::enable_shared_from_this<session> {
    tcp::resolver resolver_;
    tcp::socket socket_;
    std::string request, content, url;
    asio::streambuf result;
    asio::steady_timer timer;
    tcp::resolver::results_type resolve_results;
    Logger &logger = Logger::getLogger();

   public:
    // Resolver and socket require an io_context
    explicit
    session(asio::io_context &ioc, std::string url, std::string content)
        : resolver_(ioc), socket_(ioc), url(url), content(content),
          timer(ioc, std::chrono::steady_clock::now()) {
    }

    // Start the asynchronous operation
    void run(std::string host, std::string port) {
      // Set up an HTTP GET request message
      request = "";
      request += "POST " + url + " HTTP/1.1\n";
      request += "Host: " + std::string("localhost") + ":" + port + "\n";
      request += "User-Agent: wolf/1.0\n";
      request += "Accept: */*\n";
      request += "Content-Length: " + std::to_string(content.length()) + "\n";
      request += "Content-Type: application/x-ndjson\n\n";

      request += content;

      resolver_.resolve(host, port);

      // Look up the domain name
      resolver_.async_resolve(
          host,
          port,
          std::bind(
              &session::on_resolve,
              shared_from_this(),
              std::placeholders::_1,
              std::placeholders::_2));
    }

    void on_resolve(
        asio::error_code ec,
        const tcp::resolver::results_type &results) {
      resolve_results = results;

      if (ec) {
        logger.warn("http out resolve error: " + ec.message());
        retry();
        return;
      }

      // Make the connection on the IP address we get from a lookup
      asio::async_connect(
          socket_,
          resolve_results.begin(),
          resolve_results.end(),
          std::bind(
              &session::on_connect,
              shared_from_this(),
              std::placeholders::_1));
    }

    void retry() {
      timer.expires_after(std::chrono::seconds(1));
      timer.async_wait(std::bind(
          &session::reconnect,
          shared_from_this(),
          std::placeholders::_1));
    }

    void reconnect(asio::error_code ec) {
      asio::async_connect(
          socket_,
          resolve_results.begin(),
          resolve_results.end(),
          std::bind(
              &session::on_connect,
              shared_from_this(),
              std::placeholders::_1));
    }

    void on_connect(asio::error_code ec) {
      if (ec) {
        logger.warn("http out connect error: " + ec.message());
        retry();
        return;
      }


      // Send the HTTP request to the remote host
      asio::async_write(socket_, asio::buffer(request.data(), request.size()),
                        std::bind(
                            &session::on_write,
                            shared_from_this(),
                            std::placeholders::_1,
                            std::placeholders::_2));
    }

    void on_write(
        asio::error_code ec,
        std::size_t bytes_transferred) {
//      boost::ignore_unused(bytes_transferred);

      if (ec) {
        logger.warn("http out write error: " + ec.message());
        retry();
        return;
      }


      // Receive the HTTP response
      socket_.async_read_some(asio::streambuf::mutable_buffers_type(result.prepare(64)),
                              std::bind(
                                  &session::on_read,
                                  shared_from_this(),
                                  std::placeholders::_1,
                                  std::placeholders::_2)
          );
    }

    void on_read(
        asio::error_code ec,
        std::size_t bytes_transferred) {
//      boost::ignore_unused(bytes_transferred);

//      result.commit(bytes_transferred);
//
//      if (bytes_transferred != 0) {
//        on_write(ec, 0);
//        std::cout << "next write" << std::endl;
//
//        asio::streambuf::const_buffers_type bufs = result.data();
//        auto str = std::string(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + result.size());
//        std::cout << "size " << result.size() << ", transfered: " << bytes_transferred << ", string size: " << str.size() << std::endl;
//        std::cout << str << std::endl;
//        return;
//      }

      if (ec) {
        logger.warn("http out read error: " + ec.message());
        retry();
        return;
      }

      // Gracefully close the socket
      socket_.shutdown(tcp::socket::shutdown_both, ec);

      // not_connected happens sometimes so don't bother reporting it.
      if (ec && ec != asio::error::not_connected) {
        logger.warn("http out shutdown error, not retrying: " + ec.message());
        return;
      }
      // If we get here then the connection is closed gracefully

    }

  };

};

}

#endif //WOLF_HTTP_OUT_H
