#include "app/meta.h"

#include "absl/log/log.h"
#include "gtest/gtest.h"

TEST(meta, cpu) {
    CpuInfo info;
    LOG(INFO) << "info is " << std::format("cpu_info is {}", info);
}