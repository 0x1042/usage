#include <string>

#include "app/log.h"
#include "cpr/api.h"
#include "cpr/ssl_options.h"
#include "curl/curl.h"
#include "gtest/gtest.h"
#include "http.h"

TEST(HTTP, cpr) {
    std::string url = "https://www.baidu.com";
    auto&& rsp = get_with_cpr(url);
    INFO("cpr rsp length is {}", rsp.length());
}

TEST(HTTP, lib) {
    std::string url = "https://www.baidu.com";
    auto&& rsp = get_with_httplib(url);
    INFO("httplib rsp length is {}", rsp.length());
}

static auto WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) -> size_t {
    userp->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

TEST(HTTP, curl) {
    CURL* curl = curl_easy_init();

    if (curl == nullptr) {
        return;
    }

    INFO("curl version {}", curl_version());

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        long response_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        INFO("curl rsp code {}, len {}", response_code, response.size());
    } else {
        INFO("curl get error {}", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
}

TEST(HTTP, timeout) {
    cpr::Response resp = cpr::Get(
        cpr::Url{"http://www.httpbin.org/get"},
        cpr::Timeout{1000},
        cpr::Ssl(cpr::ssl::TLSv1_2{}),
        cpr::VerifySsl(false));

    EXPECT_LE(resp.elapsed, 1.01);
}

TEST(HTTP, post) {
    cpr::Response resp = cpr::Post(
        cpr::Url{"http://www.httpbin.org/post"},
        cpr::Body{"This is raw POST data"},
        cpr::Timeout{5000},
        cpr::Header{{"Content-Type", "text/plain"}});

    INFO("post status is {}", resp.status_code);
}

TEST(HTTP, put) {
    cpr::Response put = cpr::Put(
        cpr::Url{"https://httpbin.org/put"},
        cpr::Body{R"({"item": "book"})"},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Timeout{3000});

    INFO("put status is {}", put.status_code);
}