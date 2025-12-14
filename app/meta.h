#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <vector>

struct CpuInfo {
    std::string name;
    uint16_t processors = 0;
    uint16_t cores = 0;
    uint16_t socket = 0;

    uint32_t l1 = 0;
    uint32_t l2 = 0;
    uint32_t l3 = 0;
    uint32_t l4 = 0;

    std::vector<uint32_t> frequency;

    CpuInfo();
};

template <>
struct std::formatter<CpuInfo> {
    static constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    static auto format(const CpuInfo& cpu, std::format_context& ctx) {
        auto out = ctx.out();
        out = std::format_to(out, "\nname:{} ", cpu.name);
        out = std::format_to(out, "\nprocessors:{} ", cpu.processors);
        out = std::format_to(out, "\ncores:{} ", cpu.cores);
        out = std::format_to(out, "\nsocket:{} ", cpu.socket);

        out = std::format_to(out, "\nl1:{} ", cpu.l1);
        out = std::format_to(out, "\nl2:{} ", cpu.l2);
        out = std::format_to(out, "\nl3:{} ", cpu.l3);
        out = std::format_to(out, "\nl4:{} ", cpu.l4);
        out = std::format_to(out, "\nfrequency: [");
        bool first = true;

        for (const auto& tmp : cpu.frequency) {
            if (!first) {
                out = std::format_to(out, ", ");
            }
            first = false;
            out = std::format_to(out, "{}", tmp);
        }
        return std::format_to(out, "]\n");
    }
};
