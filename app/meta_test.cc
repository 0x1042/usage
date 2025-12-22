#include "app/meta.h"

#include "absl/cleanup/cleanup.h"
#include "absl/log/log.h"
#include "gtest/gtest.h"

TEST(meta, cpu) {
    CpuInfo info;
    LOG(INFO) << "info is " << std::format("cpu_info is {}", info);
}

TEST(meta, defer) {
    const absl::Cleanup done = std::function<void()>([]() -> void { LOG(INFO) << "run in cleanup"; });
    LOG(INFO) << "run " << __PRETTY_FUNCTION__;
}