#include <cstdlib>
#include <future>
#include <glog/logging.h>
#include <iostream>

#include "coinbase/client.hpp"

// Whether the given future is ready.
template <class T>
bool ready(const std::future<T> &f,
           std::chrono::seconds timeout = std::chrono::seconds(0)) {
  return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);

  try {
    coinbase::tls_client client;

    // Make a feed for all products.
    std::unique_ptr<coinbase::feed> feed = client.make_feed(client.products());

    // Consume it until the user stops.
    LOG(INFO) << "Consuming feed until stdin is closed.";
    std::future<void> async_input = std::async([]() { std::cin.get(); });
    while (!ready(async_input)) {
      std::cout << feed->next() << std::endl;
    }
  } catch (std::exception const &e) {
    LOG(ERROR) << "Error: " << e.what();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
