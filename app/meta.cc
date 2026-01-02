#include "meta.h"

#include <cstdint>
#include <filesystem>
#include <format>

#include "cpuinfo.h"

CpuInfo::CpuInfo()
    : processors(cpuinfo_get_processors_count()),
      cores(cpuinfo_get_cores_count()),
      socket(cpuinfo_get_packages_count()),
      l1(cpuinfo_get_l1d_caches_count()),
      l2(cpuinfo_get_l2_caches_count()),
      l3(cpuinfo_get_l3_caches_count()),
      l4(cpuinfo_get_l4_caches_count()) {
    name = std::string(cpuinfo_get_package(0)->name); // NOLINT

    for (uint32_t i = 0; i < cpuinfo_get_cores_count(); ++i) {
        const struct cpuinfo_core* core = cpuinfo_get_core(i);
        frequency.push_back(core->frequency);
    }
}

[[nodiscard]] EnvInfo::EnvInfo() : pid(getpid()) {
    struct utsname name {};

    if (uname(&name) == 0) {
        sysname = static_cast<const char*>(name.sysname);
        release = static_cast<const char*>(name.release);
        version = static_cast<const char*>(name.version);
        machine = static_cast<const char*>(name.machine);
    }

    std::filesystem::path p = "/";
    const auto& space_info = std::filesystem::space(p);

    disk_info.capacity = space_info.capacity / 1024 / 1024;
    disk_info.free = space_info.free / 1024 / 1024;
    disk_info.available = space_info.available / 1024 / 1024;
}

auto EnvInfo::toStr() const -> std::string {
    std::unordered_map<std::string, std::string> kvs;
    kvs["pid"] = std::to_string(pid);
    kvs["sysname"] = sysname;
    kvs["release"] = release;
    kvs["version"] = version;
    kvs["machine"] = machine;

    kvs["disk.capacity"] = std::format("{} MB", disk_info.capacity);
    kvs["disk.free"] = std::format("{} MB", disk_info.free);
    kvs["disk.available"] = std::format("{} MB", disk_info.available);

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
