#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "absl/log/log.h"
#include "absl/synchronization/notification.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class BgTaskStd {
public:
    BgTaskStd() {
        t_ = std::thread([this]() -> void {
            std::unique_lock<std::mutex> lock(m_);

            while (!cv_.wait_for(lock, std::chrono::seconds(1), [this]() -> bool { return stop_; })) {
                run();
            }
        });
    }

    virtual ~BgTaskStd() {
        {
            std::lock_guard<std::mutex> lock(m_);
            stop_ = true;
        }
        cv_.notify_one();

        if (t_.joinable()) {
            t_.join();
        }
        LOG(INFO) << __PRETTY_FUNCTION__ << " task exit";
    }

    BgTaskStd(const BgTaskStd&) = delete;
    BgTaskStd(BgTaskStd&&) = delete;
    auto operator=(const BgTaskStd&) -> BgTaskStd& = delete;
    auto operator=(BgTaskStd&&) -> BgTaskStd& = delete;

protected:
    virtual void run() {}

private:
    std::mutex m_;
    std::condition_variable cv_;
    bool stop_ = false;
    std::thread t_;
};

class BgWithAbsl {
public:
    BgWithAbsl(const BgWithAbsl&) = delete;
    BgWithAbsl(BgWithAbsl&&) = delete;
    auto operator=(const BgWithAbsl&) -> BgWithAbsl& = delete;
    auto operator=(BgWithAbsl&&) -> BgWithAbsl& = delete;

    BgWithAbsl(std::string_view name, absl::Duration interval = absl::Seconds(1)) : name_(name), interval_(interval) {}

    // 严禁在基类构造函数启动线程
    virtual ~BgWithAbsl() { stop(); }

    // 显式启动：在子类对象完全构造好之后调用
    void start() {
        if (t_.joinable()) {
            return;
        }

        t_ = std::thread([this]() -> void {
            LOG(INFO) << name_ << " task thread started";
            while (!notify_.WaitForNotificationWithTimeout(interval_)) {
                run();
            }
            LOG(INFO) << name_ << " task thread exiting";
        });
    }

    // 显式停止：建议在子类析构函数的第一行调用 Stop()
    void stop() {
        if (!notify_.HasBeenNotified()) {
            notify_.Notify();
        }

        if (t_.joinable()) {
            t_.join();
        }
    }

protected:
    virtual void run() = 0;
    absl::Notification notify_;
    std::string name_;

private:
    std::thread t_;
    absl::Duration interval_;
};

class MockBgTask : public BgWithAbsl {
public:
    MockBgTask() : BgWithAbsl("mock_task", absl::Seconds(1)) { start(); }

    MockBgTask(const MockBgTask&) = delete;
    MockBgTask(MockBgTask&&) = delete;
    auto operator=(const MockBgTask&) -> MockBgTask& = delete;
    auto operator=(MockBgTask&&) -> MockBgTask& = delete;
    ~MockBgTask() override { stop(); }

    void run() override { LOG(INFO) << __PRETTY_FUNCTION__ << " MockBgTask  task run "; }
};

class MockBgTask2 : public BgTaskStd {
public:
    void run() override { LOG(INFO) << __PRETTY_FUNCTION__ << " MockBgTask2  task run "; }
};

TEST(BgWithAbslTest, CallsRunPeriodically) {
    MockBgTask task;
    absl::SleepFor(absl::Seconds(2.5));
}

TEST(BgWithAbslTest, StopsCleanly) {
    {
        MockBgTask task;
        absl::SleepFor(absl::Milliseconds(100));
    }
    SUCCEED();
}

TEST(BgTaskStdTest, CallsRunPeriodically) {
    MockBgTask2 task;
    absl::SleepFor(absl::Seconds(2.5));
}

TEST(BgTaskStdTest, StopsCleanly) {
    {
        MockBgTask2 task;
        absl::SleepFor(absl::Milliseconds(100));
    }
    SUCCEED();
}