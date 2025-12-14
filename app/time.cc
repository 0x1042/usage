#include "absl/time/time.h"

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
