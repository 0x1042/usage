#include "lib/executor.h"

#include <mutex>
#include <stdexcept>
#include <thread>

#include "curl/multi.h"

CurlExecutor::CurlExecutor() : stop_(false), multi_handle_(curl_multi_init()) {
    if (multi_handle_ == nullptr) {
        throw std::runtime_error("curl_multi_init error");
    }

    worker_ = std::thread([this] -> void { run(); });
}

void CurlExecutor::run() {
    while (!stop_) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (!tasks_.empty()) {
                auto task = std::move(tasks_.front());
                tasks_.pop();
                lock.unlock();
                task();
                lock.lock();
            }
        }

        int still_running = 0;
        CURLMcode mc = curl_multi_perform(multi_handle_, &still_running);
        if (mc != CURLM_OK) {
            continue;
        }

        int msgs_in_queue = 0;
        CURLMsg* msg = nullptr;

        while ((msg = curl_multi_info_read(multi_handle_, &msgs_in_queue)) != nullptr) {
            if (msg->msg == CURLMSG_DONE) {
                CURL* easy_handle = msg->easy_handle;

                // 从 multi handle 中移除
                curl_multi_remove_handle(multi_handle_, easy_handle);

                // 调度完成回调
                void* callback_ptr = nullptr;
                curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &callback_ptr);

                if (callback_ptr != nullptr) {
                    auto* callback = static_cast<std::function<void(CURLcode)>*>(callback_ptr);
                    schedule(
                        [callback, result = msg->data.result] -> void { (*callback)(result); });
                }
            }
        }

        if (still_running > 0) {
            int numfds = 0;
            mc = curl_multi_wait(multi_handle_, nullptr, 0, 100, &numfds);

            if (mc != CURLM_OK) {
                continue;
            }
        } else {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(10), [this] -> bool {
                return stop_ || !tasks_.empty();
            });
        }
    }
}

void CurlExecutor::stop() {
    stop_ = true;
    cv_.notify_one();

    if (worker_.joinable()) {
        worker_.join();
    }

    if (multi_handle_ != nullptr) {
        curl_multi_cleanup(multi_handle_);
        multi_handle_ = nullptr;
    }
}

void CurlExecutor::add(CURL* task) {
    std::unique_lock<std::mutex> lock(mutex_);
    curl_multi_add_handle(multi_handle_, task);
    cv_.notify_one();
}

void CurlExecutor::schedule(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (stop_) {
            return;
        }

        tasks_.emplace(std::move(task));
    }

    cv_.notify_one();
}

CurlExecutor::~CurlExecutor() {
    stop();
}
