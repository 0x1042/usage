#include <string>

#include "absl/log/log.h"
#include "gtest/gtest.h"
#include "http.h"

TEST(get, cpr) {
    std::string url = "https://tool.lu";
    auto&& rsp = get_with_cpr(url);
    LOG(INFO) << "cpr rsp length is " << rsp.length();
}

TEST(get, lib) {
    std::string url = "https://tool.lu";
    auto&& rsp = get_with_httplib(url);
    LOG(INFO) << "httplib rsp length is " << rsp.length();
}
