#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <unordered_map>

#include <unistd.h>

#include <sys/utsname.h>

#include "absl/flags/parse.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "cpuinfo.h"
#include "gtest/gtest.h"

namespace {
__attribute__((constructor(101))) void init() {
    cpuinfo_initialize();
}

__attribute__((destructor(101))) void dinit() {
    cpuinfo_deinitialize();
}

} // namespace

struct EnvInfo {
    std::string sysname;
    std::string release;
    std::string version;
    std::string machine;
    pid_t pid;

    EnvInfo() : pid(getpid()) {
        struct utsname name{};

        if (uname(&name) == 0) {
            sysname = static_cast<const char*>(name.sysname);
            release = static_cast<const char*>(name.release);
            version = static_cast<const char*>(name.version);
            machine = static_cast<const char*>(name.machine);
        }
    }

    [[nodiscard]] auto toStr() const -> std::string {
        std::unordered_map<std::string, std::string> kvs;
        kvs["pid"] = std::to_string(pid);
        kvs["sysname"] = sysname;
        kvs["release"] = release;
        kvs["version"] = version;
        kvs["machine"] = machine;

        int len = 0;
        for (const auto& [k, v] : kvs) { // NOLINT
            len = std::max(static_cast<int>(k.length()), len);
        }

        std::ostringstream oss;
        oss << "\n\n";
        for (const auto& [k, v] : kvs) { // NOLINT
            oss << std::left << std::setw(len) << k << " : " << v << "\n";
        }

        return oss.str();
    }
};

auto main(int argc, char** argv) -> int {
    absl::ParseCommandLine(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    absl::InitializeLog();
    absl::SetMinLogLevel(absl::LogSeverityAtLeast::kInfo);
    absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);

    EnvInfo info;

    LOG(INFO) << info.toStr();

    return RUN_ALL_TESTS();
}
