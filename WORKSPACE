workspace(name = "coinbase")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Boost C++
_RULES_BOOST_COMMIT = "88ce413e61b1b549e44e3dc889251f14d4702abd"

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "391192d7b5231ac8f01792e87a14056b967d467952396cab22e96546f8358236",
    strip_prefix = "rules_boost-%s" % _RULES_BOOST_COMMIT,
    urls = ["https://github.com/nelhage/rules_boost/archive/%s.tar.gz" % _RULES_BOOST_COMMIT],
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

# RapidJSON
_RAPIDJSON_COMMIT = "06d58b9e848c650114556a23294d0b6440078c61"

http_archive(
    name = "com_github_tencent_rapidjson",
    build_file = "//:third_party/rapidjson.BUILD.bazel",
    sha256 = "30d28bbe0bfff9d8dc5d3cf62799b6ee550499cc1520e44bdece81e002480d19",
    strip_prefix = "rapidjson-%s" % _RAPIDJSON_COMMIT,
    urls = ["https://github.com/Tencent/rapidjson/archive/%s.tar.gz" % _RAPIDJSON_COMMIT],
)

# GFlags
_GLFAGS_VERSION = "2.2.2"

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-%s" % _GLFAGS_VERSION,
    urls = ["https://github.com/gflags/gflags/archive/v%s.tar.gz" % _GLFAGS_VERSION],
)

# GLog
_GLOG_VERSION = "0.6.0"

http_archive(
    name = "com_github_google_glog",
    sha256 = "122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022",
    strip_prefix = "glog-%s" % _GLOG_VERSION,
    urls = ["https://github.com/google/glog/archive/v%s.zip" % _GLOG_VERSION],
)
