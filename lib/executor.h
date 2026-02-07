#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "curl/curl.h"
#include "curl/multi.h"

class CurlExecutor {
public:
    CurlExecutor();

    CurlExecutor(CurlExecutor&&) = delete;
    auto operator=(CurlExecutor&&) -> CurlExecutor& = delete;
    CurlExecutor(const CurlExecutor&) = delete;
    auto operator=(const CurlExecutor&) -> CurlExecutor& = delete;

    void run();

    void stop();

    void add(CURL* task);

    void schedule(std::function<void()> task);

    ~CurlExecutor();

private:
    std::atomic<bool> stop_;
    CURLM* multi_handle_ = nullptr;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread worker_;
};
