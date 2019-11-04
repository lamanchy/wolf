#pragma once

#include <asio.hpp>
#include <base/plugins/threaded_plugin.h>
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <extras/logger.h>

namespace wolf {
namespace tcp {

class input : public threaded_plugin {
 public:
  explicit input(const option<unsigned short> &port) : port(port->value()) {
    should_never_buffer();
    non_processors_should_block();
  }

 protected:
  void setup() override {
    try {
      tcp_server server(this, io_context, port);
      io_context.run();
    }
    catch (std::exception &e) {
      logger.error("Tcp in run failed:" + std::string(e.what()));
    }
  }

  void stop() override {
    io_context.stop();
    threaded_plugin::stop();
  }

 private:
  unsigned short port{};
  asio::io_context io_context;

  class tcp_connection
      : public boost::enable_shared_from_this<tcp_connection> {
   public:
    using pointer = typename boost::shared_ptr<tcp_connection>;

    static pointer create(input *p, asio::io_context &io_context) {
      return pointer(new tcp_connection(p, io_context));
    }

    asio::ip::tcp::socket &socket() {
      return socket_;
    }

    void start() {
      asio::streambuf::mutable_buffers_type mutableBuffer =
          message_.prepare(65536);

      socket_.async_read_some(mutableBuffer, boost::bind(&tcp_connection::handle_write, this->shared_from_this(),
                                                         boost::placeholders::_1,
                                                         boost::placeholders::_2));
    }

   private:
    input *p;

    tcp_connection(input *p, asio::io_context &io_context)
        : p(p), socket_(io_context) {}

    void handle_write(const asio::error_code &error,
                      size_t bytes_transferred) {
      message_.commit(bytes_transferred);

      if (message_.size() > 0) {
        asio::streambuf::const_buffers_type bufs = message_.data();
        json message(std::string(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + message_.size()));
        message.metadata = {
            {"source", "tcp"},
            {"port", p->port}
        };
        p->output(std::move(message));
      }
      message_.consume(bytes_transferred);

      if (not error) {
        start();
      }
    }

    asio::ip::tcp::socket socket_;
    asio::streambuf message_;
  };

  class tcp_server {
   public:
    tcp_server(input *p, asio::io_context &io_context, unsigned short port)
        : p(p), acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
      start_accept();
    }

   private:
    input *p;
    using pointer = typename tcp_connection::pointer;

    void start_accept() {
      pointer new_connection =
          tcp_connection::create(p, acceptor_.get_executor().context());

      acceptor_.async_accept(new_connection->socket(),
                             boost::bind(&tcp_server::handle_accept, this, new_connection,
                                         std::error_code()));
    }

    void handle_accept(pointer new_connection,
                       const std::error_code &error) {
      if (not error) {
        new_connection->start();
      }

      start_accept();
    }

    asio::ip::tcp::acceptor acceptor_;
  };

};

}
}

