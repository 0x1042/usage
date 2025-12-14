#include "meta.h"

#include <cstdint>

#include "cpuinfo.h"

CpuInfo::CpuInfo()
    : processors(cpuinfo_get_processors_count()),
      cores(cpuinfo_get_cores_count()),
      socket(cpuinfo_get_packages_count()),
      l1(cpuinfo_get_l1d_caches_count()),
      l2(cpuinfo_get_l2_caches_count()),
      l3(cpuinfo_get_l3_caches_count()),
      l4(cpuinfo_get_l4_caches_count()) {
    name = std::string(cpuinfo_get_package(0)->name); //NOLINT

    for (uint32_t i = 0; i < cpuinfo_get_cores_count(); ++i) {
        const struct cpuinfo_core* core = cpuinfo_get_core(i);
        frequency.push_back(core->frequency);
    }
}
