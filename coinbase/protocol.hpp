#pragma once

#include <string>
#include <vector>

// Coinbase API protocol implementation.
namespace coinbase::protocol {

// Given a JSON response of the REST API for /products, extracts all online ids.
std::vector<std::string> extract_online_product_ids(const std::string &data);

// Make a "subscribe" message for a set of products.
std::string make_subscribe_message(const std::vector<std::string> &product_ids);

} // namespace coinbase::protocol
