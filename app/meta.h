#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <vector>

template <typename F>
class DeferredAction {
public:
    DeferredAction(F&& fn) noexcept : fn_(std::move(fn)) {}
    DeferredAction(DeferredAction&& other) noexcept : fn_(std::move(other.fn_)), cancelled_(other.cancelled_) {
        other.cancelled_ = true;
    }

    auto operator=(DeferredAction&& o) noexcept -> DeferredAction& {
        if (this != &o) {
            this->~DeferredAction();
            new (this) DeferredAction(std::move(o));
        }
        return *this;
    }

    auto operator=(const DeferredAction&) -> DeferredAction& = delete;
    DeferredAction(const DeferredAction&) = delete;

    ~DeferredAction() {
        if (!cancelled_) {
            fn_();
        };
    }

    void cancel() { cancelled_ = true; }

private:
    F fn_;
    bool cancelled_ = false;
};

template <typename F>
inline auto defer(F&& fn) -> DeferredAction<F> {
    return DeferredAction<F>(std::forward<F>(fn));
}

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
