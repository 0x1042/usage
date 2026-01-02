#include "absl/time/time.h"

#include <chrono>
#include <print>

#include "absl/log/log.h"
#include "absl/time/civil_time.h"
#include "absl/time/clock.h"
#include "gtest/gtest.h"

TEST(time, now) {
    LOG(INFO) << "now is " << absl::Now();
    LOG(INFO) << "UnixEpoch is " << absl::UnixEpoch();
    LOG(INFO) << "UniversalEpoch is " << absl::UniversalEpoch();
}

TEST(time, ts) {
    LOG(INFO) << "now is " << absl::ToUnixSeconds(absl::Now());
    LOG(INFO) << "now is " << absl::ToUnixMillis(absl::Now());
    LOG(INFO) << "now is " << absl::ToUnixMicros(absl::Now());
}

TEST(time, chrono) {
    using namespace std::literals;
    std::chrono::year_month_day ymd{2026y / std::chrono::January / 2};
    std::print("ymd is {}\n", ymd);
    auto now = std::chrono::system_clock::now();
    std::print("Full date and time: {0:%Y-%m-%d %H:%M:%S}\n", now);
    std::print("Date only: {0:%F}\n", now);
    std::print("Time only: {0:%T}\n", now);
    std::print("Day of the week: {0:%A}\n", now);
    std::print("Month name: {0:%B}\n", now);
    std::print("12-hour clock with AM/PM: {0:%I:%M:%S %p}\n", now);
    std::print("ISO 8601 format: {0:%FT%T%z}\n", now);
}
