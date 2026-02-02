#include <cstring>
#include <memory>
#include <string>

#include <unistd.h>

#include <sys/utsname.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "app/cmd.h"
#include "cpuinfo.h"
#include "gflags/gflags.h"
#include "gtest/gtest.h"
#include "log.h"
#include "meta.h"

namespace {
__attribute__((constructor(101))) void init() {
    cpuinfo_initialize();
}

__attribute__((destructor(101))) void dinit() {
    cpuinfo_deinitialize();
}

void log_env(char** envp) {
    INFO("-----------------------------");
    for (char** env = envp; *env != nullptr; ++env) { // NOLINT
        INFO("env {}", *env);
    }
    INFO("-----------------------------");
}

} // namespace

auto main(int argc, char** argv, char** envp) -> int {
    absl::Time start = absl::Now();

#ifdef __GLIBCXX__
    std::clog << "__GLIBCXX__:" << __GLIBCXX__ << '\n';
#endif

#ifdef _LIBCPP_VERSION
    std::clog << "_LIBCPP_VERSION:" << _LIBCPP_VERSION << '\n';
#endif

    std::shared_ptr<void> on_exit(nullptr, [&](...) -> void {
        absl::Duration elapsed = absl::Now() - start;
        INFO("{} is exit, cost {}", getpid(), absl::FormatDuration(elapsed));
    });

    INFO("{}", exec_cmd({"uname", "-a"}));

#ifdef __LINUX__
    INFO("{}", exec_cmd({"ldd", "-v"}));
    INFO("{}", exec_cmd({"gcc", "--version"}));
    INFO("{}", exec_cmd({"lscpu"}));
    INFO("{}", exec_cmd({"cat", "/proc/version"}));
    INFO("{}", exec_cmd({"cat", "/proc/cpuinfo"}));
    INFO("{}", exec_cmd({"cat", "/proc/meminfo"}));
#endif

    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    absl::InitializeLog();
    absl::SetMinLogLevel(absl::LogSeverityAtLeast::kInfo);
    absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);
    log_env(envp);

    EnvInfo info;

    INFO("env info. {}", info.toStr());

    return RUN_ALL_TESTS();
}
