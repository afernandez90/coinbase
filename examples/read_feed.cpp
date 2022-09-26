#include <cstdlib>
#include <glog/logging.h>
#include <iostream>

#include "coinbase/client.hpp"

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);

  try {
    coinbase::tls_client client;

    // Make a feed for all products.
    std::unique_ptr<coinbase::feed> feed = client.make_feed(client.products());

    // Consume it indefinitely.
    for (;;) {
      std::cout << feed->next() << std::endl;
    }
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
