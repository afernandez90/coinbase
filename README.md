# Coinbase C++ Library
A C++ client to interact with Coinbase via its REST and WebSocket APIs. The build is self-contained via `bazel`, it just requires that the system's version of GCC supports at least C++ 17.

Examples:
* `bazel run //examples:list_all_products`: Lists all available product IDs.
* `bazel run //examples:read_feed`: Consumes the L2 and auction channels for all products indefinitely.

Implementation structure:
* `//coinbase:client`: Client APIs for users.
* `//coinbase:protocol`: Encoding and decoding of JSON requests and responses.
* `//coinbase:net`: Network IO with with DNS, TLS v1.3 and WebSockets.
