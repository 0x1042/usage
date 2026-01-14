#include <cstring>

#include <unistd.h>

#include <sys/utsname.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "cpuinfo.h"
#include "gflags/gflags.h"
#include "gtest/gtest.h"
#include "meta.h"

namespace {
__attribute__((constructor(101))) void init() {
    cpuinfo_initialize();
}

__attribute__((destructor(101))) void dinit() {
    cpuinfo_deinitialize();
}

void log_env(char** envp) {
    LOG(INFO) << "-----------------------------";
    for (char** env = envp; *env != nullptr; ++env) { // NOLINT
        LOG(INFO) << "env " << *env;
    }
    LOG(INFO) << "-----------------------------";
}

} // namespace

auto main(int argc, char** argv, char** envp) -> int {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    absl::InitializeLog();
    absl::SetMinLogLevel(absl::LogSeverityAtLeast::kInfo);
    absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);
    log_env(envp);

    EnvInfo info;

    LOG(INFO) << info.toStr();

    return RUN_ALL_TESTS();
}
