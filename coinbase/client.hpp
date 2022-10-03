#include "coinbase/net.hpp"

#include <string_view>

// C++ client for Coinbase.
namespace coinbase {

namespace {

constexpr std::string_view REST_API = "api.exchange.coinbase.com";
constexpr std::string_view REST_API_PRODUCTS_TARGET = "/products";
constexpr std::string_view WEBSOCKET_API = "ws-feed.exchange.coinbase.com";

} // namespace

// Any function that can execute GET requests.
using rest_api_executor = std::function<std::string(net::http_get_request)>;
// Any function that makes Websocket clients.
using ws_client_factory =
    std::function<std::unique_ptr<net::websocket_client>()>;

// Represents a live feed stream.
class feed {
public:
  virtual ~feed() = default;
  virtual std::string_view next() = 0;
};

// Interface for Coinbase clients.
class client {
public:
  virtual ~client() = default;

  // List all online products (available in feed).
  virtual std::vector<std::string> products() = 0;

  // Make a new feed stream for a set of products.
  virtual std::unique_ptr<feed>
  make_feed(const std::vector<std::string> &products) = 0;
};

// Feed implementation over a Websocket.
class wss_feed : public feed {
public:
  explicit wss_feed(std::unique_ptr<net::websocket_client> ws_client);
  std::string_view next() override;

private:
  std::unique_ptr<net::websocket_client> ws_client_;
};

// Implementation of Coinbase clients with HTTP and WebSockets over TLS.
struct tls_client_config {
  rest_api_executor api_executor = net::execute;
  ws_client_factory ws_factory = []() {
    return std::make_unique<net::websocket_tls_client>(WEBSOCKET_API);
  };
};
class tls_client : public client {
public:
  explicit tls_client(tls_client_config &&config = {});

  std::vector<std::string> products() override;
  std::unique_ptr<feed>
  make_feed(const std::vector<std::string> &products) override;

private:
  tls_client_config config_;
};

} // namespace coinbase
