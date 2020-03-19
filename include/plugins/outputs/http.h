#pragma once

#include <base/plugins/threaded_plugin.h>

#include <asio/connect.hpp>
#include <asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace wolf {
namespace http {
class output : public threaded_plugin {
 public:
  output(const option<std::string> &host,
         const option<std::string> &port,
         const option<std::string> &url) :
      host(host->value()),
      port(port->value()),
      url(url->value()) {};

 protected:
  void loop() override {
    if (ioc.run_one() == 0) {
      i = 0;
      get_loop_sleeper().increasing_sleep();
      ioc.restart();
    } else {
      get_loop_sleeper().decrease_sleep_time();
    }
  }

 protected:
  void process(json &&message) override {
    std::make_shared<session>(ioc, url, message.get_string())->run(host, port);

    ++i;
//    ioc.run();
  }

  bool is_full() override {
    return i >= 10;
  }

 private:
  asio::io_context ioc;

  std::string host, port, url, method;
  std::atomic<unsigned long> i{0};

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
        logger.warn("[http_out] http out resolve error: " + ec.message());
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
        logger.warn("[http_out] http out connect error: " + ec.message());
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
        logger.warn("[http_out] http out write error: " + ec.message());
        retry();
        return;
      }


      // Receive the HTTP response
      asio::async_read_until(socket_, result, "\r\n\r\n", std::bind(
          &session::on_read,
          shared_from_this(),
          std::placeholders::_1,
          std::placeholders::_2)
      );
    }

    void on_read(
        asio::error_code ec,
        std::size_t bytes_transferred) {
      result.commit(bytes_transferred);

      if (ec) {
        logger.warn("[http_out] http out read error: " + ec.message());
        retry();
        return;
      }

      std::istream response_stream(&result);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        logger.error("[http_out] Invalid http response");
      } else if (status_code != 200 and status_code != 201 and status_code != 202 and status_code != 203
          and status_code != 204) {
        asio::streambuf::const_buffers_type bufs = result.data();
        auto str = std::string(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + bytes_transferred);
        std::cout << "size " << result.size() << ", transfered: " << bytes_transferred << ", string size: "
                  << str.size() << std::endl;

        logger.error("[http_out] Http response not in 200-204");
        logger.error(str);
      }

      // Gracefully close the socket
      socket_.shutdown(tcp::socket::shutdown_both, ec);

      // not_connected happens sometimes so don't bother reporting it.
      if (ec && ec != asio::error::not_connected) {
        logger.warn("[http_out] http out shutdown error, not retrying: " + ec.message());
        return;
      }
      // If we get here then the connection is closed gracefully

    }

  };

};

}
}


