#include <string>

#include "absl/log/log.h"
#include "curl/curl.h"
#include "gtest/gtest.h"
#include "http.h"

TEST(get, cpr) {
    std::string url = "https://www.baidu.com";
    auto&& rsp = get_with_cpr(url);
    LOG(INFO) << "cpr rsp length is " << rsp.length();
}

TEST(get, lib) {
    std::string url = "https://www.baidu.com";
    auto&& rsp = get_with_httplib(url);
    LOG(INFO) << "httplib rsp length is " << rsp.length();
}

static auto WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) -> size_t {
    userp->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

TEST(get, curl) {
    CURL* curl = curl_easy_init();

    if (curl == nullptr) {
        return;
    }

    LOG(INFO) << curl_version();

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        long response_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        LOG(INFO) << "curl rsp code " << response_code << " len " << response.size();
    } else {
        LOG(INFO) << "curl get error " << curl_easy_strerror(res);
    }
    curl_easy_cleanup(curl);
}
