#include <asio.hpp>
#include <iostream>
#include <string>

class session : public std::enable_shared_from_this<session> {
 public:
  session(asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

  void start() { do_read(); }

 private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length) {
          if (!ec) {
            std::cout << data_ << std::endl;
            do_write(length);
          }
        });
  }

  void do_write(std::size_t length) {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(data_, length),
                      [this, self](std::error_code ec, std::size_t /*length*/) {
                        if (!ec) {
                          do_read();
                        }
                      });
  }

  asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class server {
 public:
  server(asio::io_context& io_context, std::uint16_t port)
      : acceptor_(io_context,
                  asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    do_accept();
  }

 private:
  void do_accept() {
    acceptor_.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
          if (!ec) {
            std::make_shared<session>(std::move(socket))->start();
          }
          do_accept();
        });
  }

  asio::ip::tcp::acceptor acceptor_;
};

int asio_server() {
  try {
    asio::io_context io_context;
    server s(io_context, 12345);
    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
