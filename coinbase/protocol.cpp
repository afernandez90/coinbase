#include "coinbase/protocol.hpp"

#include <iostream>

namespace coinbase::protocol {

std::vector<std::string> extract_product_ids(const std::string &data) {
  rapidjson::Document json;
  json.Parse(data.c_str());
  auto array = json.GetArray();
  std::vector<std::string> result;
  result.reserve(array.Size());
  for (auto &value : array) {
    result.emplace_back(value.FindMember("id")->value.GetString());
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
