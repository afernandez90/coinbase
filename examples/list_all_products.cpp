#include <cstdlib>
#include <glog/logging.h>
#include <iostream>

#include "coinbase/client.hpp"

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);

  try {
    coinbase::tls_client client;

    std::vector<std::string> products = client.products();

    for (auto &product : products) {
      std::cout << product << std::endl;
    }
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
