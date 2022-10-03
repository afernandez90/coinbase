#include "coinbase/client.hpp"

#include "coinbase/protocol.hpp"

namespace coinbase {

tls_client::tls_client(tls_client_config &&config)
    : config_(std::move(config)) {}

std::vector<std::string> tls_client::products() {
  net::http_get_request request;
  request.host_name = REST_API;
  request.target = REST_API_PRODUCTS_TARGET;
  return protocol::extract_online_product_ids(
      config_.api_executor(std::move(request)));
}

std::unique_ptr<feed>
tls_client::make_feed(const std::vector<std::string> &products) {
  std::unique_ptr<net::websocket_client> client = config_.ws_factory();
  client->send(protocol::make_subscribe_message(products));
  return std::make_unique<wss_feed>(std::move(client));
}

wss_feed::wss_feed(std::unique_ptr<net::websocket_client> ws_client)
    : ws_client_(std::move(ws_client)) {}

std::string_view wss_feed::next() { return ws_client_->receive(); }

} // namespace coinbase
