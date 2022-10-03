#include "coinbase/protocol.hpp"

#include <sstream>

namespace coinbase::protocol {

namespace {

template <class T>
void field_check(const T &value, const std::string &field_name) {
  if (!value.HasMember(field_name.c_str())) {
    std::ostringstream error;
    error << "Expected value to have member \"" << field_name
          << "\" but it didn't.";
    throw std::runtime_error(error.str());
  }
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
    field_check(value, "status");
    if (value["status"] == "online") {
      field_check(value, "id");
      result.emplace_back(value["id"].GetString());
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
  writer.String("level2");
  writer.String("auctionfeed");
  writer.EndArray();
  writer.EndObject();
  return std::string(buffer.GetString(), buffer.GetSize());
}

} // namespace coinbase::protocol
