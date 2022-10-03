#include "coinbase/net.hpp"

#include <boost/beast/core/role.hpp>
#include <boost/beast/websocket/stream_base.hpp>
#include <boost/format.hpp>
#include <glog/logging.h>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ssl = asio::ssl;
namespace websocket = beast::websocket;

namespace coinbase::net {

namespace {

// 10 for HTTP 1.0, 11 for HTTP 1.1.
const int BOOST_HTTP_VERSION = 11;

decltype(auto) lookup_https_service(std::string_view name) {
  boost::asio::io_context io_context;
  boost::asio::ip::tcp::resolver resolver(io_context);
  auto result = resolver.resolve(name, "https");
  LOG(INFO) << "Host name \"" << name
            << "\" resolved as: " << result->endpoint();
  return result;
}

// Make an SSL context ready for certificate validation.
ssl::context make_boost_ssl_context() {
  ssl::context result(ssl::context::tlsv13_client);
  result.set_default_verify_paths();
  result.set_verify_mode(ssl::verify_peer | ssl::verify_fail_if_no_peer_cert);
  return result;
}

// Performs SSL handshake on a given stream for a given host.
decltype(auto) ssl_init(beast::ssl_stream<beast::tcp_stream> &stream,
                        std::string_view host_name) {
  // Set SNI Hostname (many hosts need this to SSL handshake successfully).
  if (!SSL_set_tlsext_host_name(stream.native_handle(), host_name.data())) {
    beast::error_code ec{static_cast<int>(::ERR_get_error()),
                         asio::error::get_ssl_category()};
    throw beast::system_error{ec};
  }

  auto endpoint =
      beast::get_lowest_layer(stream).connect(lookup_https_service(host_name));
  LOG(INFO) << host_name << " -> TCP connection established";

  stream.handshake(ssl::stream_base::client);
  LOG(INFO) << host_name << " -> SSL handshake completed";

  return endpoint;
}

} // namespace

std::string execute(http_get_request request) {
  // Initialise TCP and TLS.
  ssl::context ssl_context = make_boost_ssl_context();
  asio::io_context io_context;
  beast::ssl_stream<beast::tcp_stream> stream(io_context, ssl_context);
  ssl_init(stream, request.host_name);

  // Build http boost request.
  http::request<http::string_body> boost_request{
      http::verb::get, request.target, BOOST_HTTP_VERSION};
  boost_request.set(http::field::host, request.host_name);
  boost_request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  // Send request.
  VLOG(1) << request.host_name << " -> Sending GET Request: " << std::endl
          << boost_request;
  http::write(stream, boost_request);

  // Read response.
  beast::flat_buffer parser_buffer;
  http::response<http::string_body> response;
  http::read(stream, parser_buffer, response);

  VLOG(1) << request.host_name << " -> Response header:" << std::endl
          << response.base();

  if (response.body().empty()) {
    throw std::runtime_error("Unexpected empty response body.");
  }

  VLOG(2) << "Response body (might be truncated): " << std::endl
          << response.body();
  return response.body();
}

websocket_tls_client::websocket_tls_client(std::string_view peer_name)
    : ssl_context_(make_boost_ssl_context()), wss_(io_context_, ssl_context_) {
  // Initialise websocket options.
  wss_.set_option(
      websocket::stream_base::timeout::suggested(beast::role_type::client));

  // The timeouts on a websocket stream are incompatible with the timeouts
  // used in the tcp_stream. When constructing a websocket stream from a tcp
  // stream that has timeouts enabled, the timeout should be disabled first
  // before constructing the websocket stream.
  beast::get_lowest_layer(wss_).expires_never();

  // Initialise TCP and TLS layers.
  auto endpoint = ssl_init(wss_.next_layer(), peer_name);
  // Initialise websocket layer.
  wss_.set_option(beast::websocket::stream_base::decorator(
      [](beast::websocket::request_type &req) {
        req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client");
      }));

  // Add endpoint port to host name for WSS handshake.
  // See https://tools.ietf.org/html/rfc7230#section-5.4
  std::string handshake_peer_addr =
      (boost::format("%s:%d") % peer_name % endpoint.port()).str();
  wss_.handshake(handshake_peer_addr, "/");
  LOG(INFO) << handshake_peer_addr << " -> Websocket handshake completed";
}

void websocket_tls_client::send(const std::string &data) {
  VLOG(1) << "Sending data over websocket: " << data;

  boost::asio::const_buffer boost_buffer(data.data(), data.size());
  wss_.write(boost_buffer);
}

std::string_view websocket_tls_client::receive() {
  read_buffer_.clear();
  size_t bytes_read = wss_.read(read_buffer_);
  std::string_view result = std::string_view(
      reinterpret_cast<const char *>(read_buffer_.cdata().data()), bytes_read);
  VLOG(2) << "Data received over websocket: " << result;
  return result;
}

} // namespace coinbase::net
