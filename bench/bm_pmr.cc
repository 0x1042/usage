#include <array>
#include <cstddef>
#include <memory_resource>
#include <string>
#include <vector>

#include "benchmark/benchmark.h"

constexpr size_t N = 10240;

auto generate_long_string(size_t len = 64) -> std::string {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    result.reserve(len);
    for (size_t i = 0; i < len; ++i) {
        result += charset[rand() % (sizeof(charset) - 1)];
    }
    return result;
}

void base_vec() {
    std::vector<std::string> ids;
    ids.reserve(N);

    auto long_str = generate_long_string(64); // 64 字节,超过 SSO
    for (size_t idx = 0; idx < N; idx++) {
        ids.emplace_back(long_str); // 每次都会堆分配
    }
}

void pmr_vec() {
    alignas(64) std::array<std::byte, 2 * 1024 * 1024> stack_buf; // NOLINT 2MB
    std::pmr::monotonic_buffer_resource arena{stack_buf.data(), stack_buf.size()};

    std::pmr::vector<std::pmr::string> ids(&arena);
    ids.reserve(N);

    auto long_str = generate_long_string(64);
    for (size_t idx = 0; idx < N; idx++) {
        ids.emplace_back(long_str); // 从 arena 分配
    }
}

static void BM_vec_base(benchmark::State& state) {
    for (auto _ : state) {
        base_vec();
    }
}

static void BM_vec_pmr(benchmark::State& state) {
    for (auto _ : state) {
        pmr_vec();
    }
}

BENCHMARK(BM_vec_base);
BENCHMARK(BM_vec_pmr);