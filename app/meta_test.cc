#include "lib/meta.h"

#include <format>

#include "absl/cleanup/cleanup.h"
#include "gtest/gtest.h"
#include "lib/log.h"

TEST(meta, cpu) {
    CpuInfo info;
    INFO("info is {}", std::format("cpu_info is {}", info));
}

TEST(meta, defer) {
    const absl::Cleanup done = std::function<void()>([]() -> void { INFO("run in cleanup"); });
    INFO("run {}", __PRETTY_FUNCTION__);
}