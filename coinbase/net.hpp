#pragma once

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

// Network I/O module.
namespace coinbase::net {

struct http_get_request {
  // host_name is of the form www.example.org.
  std::string host_name;
  // In www.example.org/state, "/state is the target".
  std::string target;
};
// Executes an HTTP GET request with TLS and returns the response.
std::string execute(http_get_request request);

// Interface for web sockets.
class websocket_client {
public:
  virtual ~websocket_client() = default;

  virtual void send(const std::string&) = 0;

  // Result is valid as long as the instance is alive.
  virtual std::string_view receive() = 0;
};

// Implementation of websocket over TLS.
class websocket_tls_client : public websocket_client {
public:
  websocket_tls_client(std::string_view peer_name);
  void send(const std::string&) override;
  std::string_view receive() override;

private:
  // IO context for general networking (sockets, etc.).
  boost::asio::io_context io_context_;
  // SSL context (certificates, etc.).
  boost::asio::ssl::context ssl_context_;
  // Top-level stream over the TCP and SSL layers.
  boost::beast::websocket::stream<
      boost::beast::ssl_stream<boost::beast::tcp_stream>, /* deflate */ true>
      wss_;
  // Read buffer, preserved as a member for performance reasons (i.e. avoiding
  // allocations each time receive() is called).
  boost::beast::flat_buffer read_buffer_;
};

} // namespace coinbase::net
