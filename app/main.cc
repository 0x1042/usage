#include "absl/flags/parse.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "gtest/gtest.h"

auto main(int argc, char** argv) -> int {
    absl::ParseCommandLine(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    absl::InitializeLog();
    absl::SetMinLogLevel(absl::LogSeverityAtLeast::kInfo);
    absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);
    return RUN_ALL_TESTS();
}