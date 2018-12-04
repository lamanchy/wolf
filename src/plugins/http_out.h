//
// Created by lamanchy on 16.11.18.
//

#ifndef WOLF_HTTP_OUT_H
#define WOLF_HTTP_OUT_H

#include <base/threaded_plugin.h>

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
  void run() override {
    while (running) {
      if (ioc.run_one() == 0) {
        i = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ioc.restart();
      }
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
      request += "Host: " + host + ":" + port + "\n";
      request += "User-Agent: wolf/1.0\n";
      request += "Accept: */*\n";
      request += "Content-Length: " + std::to_string(content.length()) + "\n";
      request += "Content-Type: application/x-www-form-urlencoded\n\n";

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
        tcp::resolver::results_type results) {
      resolve_results = results;

      if (ec) {
        std::cerr << "resolve" << ": " << ec.message() << "\n";
        return;
      }

      // Make the connection on the IP address we get from a lookup
      asio::async_connect(
          socket_,
          results.begin(),
          results.end(),
          std::bind(
              &session::on_connect,
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
        std::cerr << "connect" << ": " << ec.message() << "\n";
        timer.expires_after(std::chrono::seconds(1));
        timer.async_wait(std::bind(
            &session::reconnect,
            shared_from_this(),
            std::placeholders::_1));

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
        std::cerr << "write" << ": " << ec.message() << "\n";
        return;
      }


      // Receive the HTTP response
      asio::async_read(socket_, asio::streambuf::mutable_buffers_type(result.prepare(64)),
                       std::bind(
                           &session::on_read,
                           shared_from_this(),
                           std::placeholders::_1,
                           std::placeholders::_2));
    }

    void on_read(
        asio::error_code ec,
        std::size_t bytes_transferred) {
//      boost::ignore_unused(bytes_transferred);

      if (ec) {
        std::cerr << "read" << ": " << ec.message() << "\n";
        return;
      }

      // Write the message to standard out

      // Gracefully close the socket
      socket_.shutdown(tcp::socket::shutdown_both, ec);

      // not_connected happens sometimes so don't bother reporting it.
      if (ec && ec != asio::error::not_connected) {
        std::cerr << "shutdown" << ": " << ec.message() << "\n";
        return;
      }
      // If we get here then the connection is closed gracefully

    }

  };


//    std::make_shared<session>(ioc)->run(host, port, target, version);
//    ioc.run();

};

}

#endif //WOLF_HTTP_OUT_H
