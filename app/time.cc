#include "absl/time/time.h"

#include <chrono>

#include "absl/time/clock.h"
#include "gtest/gtest.h"
#include "lib/log.h"

TEST(Time, now) {
    INFO("now is {}", absl::FormatTime(absl::Now()));
    INFO("UnixEpoch is {}", absl::FormatTime(absl::UnixEpoch()));
    INFO("UniversalEpoch is {}", absl::FormatTime(absl::UniversalEpoch()));
}

TEST(Time, ts) {
    INFO("now to_unix_seconds {}", absl::ToUnixSeconds(absl::Now()));
    INFO("now to_unix_millis {}", absl::ToUnixMillis(absl::Now()));
    INFO("now to_unix_micros {}", absl::ToUnixMicros(absl::Now()));
}

TEST(Time, chrono) {
    using namespace std::literals;
    std::chrono::year_month_day ymd{2026y / std::chrono::January / 2};
    INFO("ymd is {}", ymd);
    auto now = std::chrono::system_clock::now();
    INFO("Full date and time: {0:%Y-%m-%d %H:%M:%S}", now);
    INFO("Date only: {0:%F}", now);
    INFO("Time only: {0:%T}", now);
    INFO("Day of the week: {0:%A}", now);
    INFO("Month name: {0:%B}", now);
    INFO("12-hour clock with AM/PM: {0:%I:%M:%S %p}", now);
    INFO("ISO 8601 format: {0:%FT%T%z}", now);
}
