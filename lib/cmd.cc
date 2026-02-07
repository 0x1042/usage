#include "lib/cmd.h"

auto exec_cmd(const std::vector<std::string>& args) -> std::string {
    reproc::process process;

    if (const auto& ec = process.start(args); ec) {
        return ec.message();
    }

    std::string output;

    if (const auto& ec = reproc::drain(process, reproc::sink::string(output), reproc::sink::null);
        ec) {
        return ec.message();
    }

    auto [status, error] = process.wait(reproc::infinite);

    if (error) {
        return error.message();
    }
    return output;
}
