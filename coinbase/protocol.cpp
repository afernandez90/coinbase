#include "coinbase/protocol.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sstream>
#include <unordered_map>

namespace coinbase::protocol {

namespace {

// Keys and values in the JSON response of the /products target.
enum class product_key { id, status };
const std::unordered_map<product_key, std::string_view> PRODUCT_KEYS{
    {product_key::id, "id"}, {product_key::status, "status"}};
constexpr std::string_view ONLINE_STATUS_VALUE = "online";

template <class T> std::string safe_get(const T &entry, product_key key) {
  auto key_string = PRODUCT_KEYS.at(key).data();

  if (!entry.HasMember(key_string)) {
    std::ostringstream error;
    error << "Expected value to have member \"" << key_string
          << "\" but it didn't.";
    throw std::runtime_error(error.str());
  }

  auto &value = entry[key_string];

  if (!value.IsString()) {
    std::ostringstream error;
    error << "Expected value of \"" << key_string << "\" to be of type string.";
    throw std::runtime_error(error.str());
  }

  return value.GetString();
}

} // namespace

std::vector<std::string> extract_online_product_ids(const std::string &data) {
  rapidjson::Document json;
  json.Parse(data.c_str());

  if (!json.IsArray()) {
    throw std::runtime_error("Expected JSON array.");
  }

  auto array = json.GetArray();

  if (array.Empty()) {
    throw std::runtime_error("Expected empty JSON array.");
  }

  std::vector<std::string> result;
  result.reserve(array.Size());

  for (auto &value : array) {
    if (safe_get(value, product_key::status) == ONLINE_STATUS_VALUE) {
      result.emplace_back(safe_get(value, product_key::id));
    }
  }
  return result;
}

std::string
make_subscribe_message(const std::vector<std::string> &product_ids) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  writer.StartObject();
  writer.Key("type");
  writer.String("subscribe");
  writer.Key("product_ids");
  writer.StartArray();
  for (auto &product : product_ids) {
    writer.String(product.data(), product.size());
  }
  writer.EndArray();
  writer.Key("channels");
  writer.StartArray();
  writer.String("heartbeat");
  writer.String("level2");
  writer.String("ticker");
  writer.String("matches");
  writer.EndArray();
  writer.EndObject();
  return std::string(buffer.GetString(), buffer.GetSize());
}

} // namespace coinbase::protocol
