#include <print>
#include <string>

#include "absl/log/log.h"
#include "app/log.h"
#include "cpr/api.h"
#include "cpr/ssl_options.h"
#include "curl/curl.h"
#include "gtest/gtest.h"
#include "http.h"
#include "workflow/HttpMessage.h"
#include "workflow/HttpUtil.h"
#include "workflow/WFDnsClient.h"
#include "workflow/WFTaskFactory.h"

TEST(HTTP, cpr) {
    std::string url = "https://www.baidu.com";
    auto&& rsp = get_with_cpr(url);
    LOG(INFO) << "cpr rsp length is " << rsp.length();
}

TEST(HTTP, lib) {
    std::string url = "https://www.baidu.com";
    auto&& rsp = get_with_httplib(url);
    LOG(INFO) << "httplib rsp length is " << rsp.length();
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

TEST(HTTP, timeout) {
    cpr::Response resp = cpr::Get(
        cpr::Url{"http://www.httpbin.org/get"},
        cpr::Timeout{1000},
        cpr::Ssl(cpr::ssl::TLSv1_2{}),
        cpr::VerifySsl(false));

    EXPECT_LE(resp.elapsed, 1);
}

TEST(HTTP, post) {
    cpr::Response resp = cpr::Post(
        cpr::Url{"http://www.httpbin.org/post"},
        cpr::Body{"This is raw POST data"},
        cpr::Timeout{5000},
        cpr::Header{{"Content-Type", "text/plain"}});

    LOG(INFO) << "status is " << resp.status_code;
}

TEST(HTTP, asyncget) {
    cpr::AsyncResponse fr = cpr::GetAsync(cpr::Url{"http://www.httpbin.org/get"});
    cpr::Response resp = fr.get();
}

void wget_callback(WFHttpTask* task) {
    protocol::HttpRequest* req = task->get_req();
    protocol::HttpResponse* resp = task->get_resp();
    int state = task->get_state();
    int error = task->get_error();

    INFO("state is {}/{}", state, error);

    switch (state) {
        case WFT_STATE_SYS_ERROR:
            std::println(stderr, "system error: {}", strerror(error));
            break;
        case WFT_STATE_DNS_ERROR:
            std::println(stderr, "DNS error: {}", gai_strerror(error));
            break;
        case WFT_STATE_SSL_ERROR:
            std::println(stderr, "SSL error: {}", error);
            break;
        case WFT_STATE_TASK_ERROR:
            std::println(stderr, "Task error: {}", error);
            break;
        default:
            break;
    }

    if (state != WFT_STATE_SUCCESS) {
        std::println(stderr, "Failed. Press Ctrl-C to exit.");
        return;
    }

    std::string name;
    std::string value;

    /* Print request. */
    std::print(
        stderr, "{} {} {}\r\n", req->get_method(), req->get_http_version(), req->get_request_uri());

    protocol::HttpHeaderCursor req_cursor(req);

    while (req_cursor.next(name, value)) {
        std::print(stderr, "{}: {}\r\n", name, value);
    }
    std::print(stderr, "\r\n");

    /* Print response header. */
    std::print(
        stderr,
        "{} {} {}\r\n",
        resp->get_http_version(),
        resp->get_status_code(),
        resp->get_reason_phrase());

    protocol::HttpHeaderCursor resp_cursor(resp);

    while (resp_cursor.next(name, value)) {
        std::print(stderr, "{}: {}\r\n", name, value);
    }
    std::print(stderr, "\r\n");

    /* Print response body. */
    const void* body = nullptr;
    size_t body_len = 0;

    resp->get_parsed_body(&body, &body_len);
    fwrite(body, 1, body_len, stdout);
    fflush(stdout);

    INFO("success");
}

TEST(HTTP, WGET) {
    std::mutex mutex;
    std::condition_variable cond;
    bool done = false;

    WFHttpTask* task = WFTaskFactory::create_http_task(
        "https://www.baidu.com", 5, 2, [&mutex, &cond, &done](WFHttpTask* task) -> void {
            wget_callback(task);
            mutex.lock();
            done = true;
            mutex.unlock();
            cond.notify_one();
        });

    protocol::HttpRequest* req = task->get_req();
    req->add_header_pair("Accept", "*/*");
    req->add_header_pair("User-Agent", "Wget/1.14 (linux-gnu)");
    req->add_header_pair("Connection", "close");
    task->start();

    std::unique_lock<std::mutex> lock(mutex);
    while (!done) {
        cond.wait(lock);
    }

    lock.unlock();
}
