#include "absl/random/random.h"

#include <chrono>
#include <random>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lib/log.h"

TEST(random, uniform) {
    using namespace ::testing;
    absl::BitGen gen;
    int iterations = 10;

    for ([[maybe_unused]] int idx : std::views::iota(0, iterations)) {
        EXPECT_THAT(absl::Uniform(gen, 1, 101), AllOf(Ge(1), Lt(101)));
        EXPECT_THAT(absl::Uniform(gen, 0.0, 1.0), AllOf(Ge(0.0), Lt(1.0)));
    }

    INFO("Bernoulli. {}", absl::Bernoulli(gen, 0.5));
}

TEST(random, std) {
    using namespace ::testing;

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 engine(seed);
    std::uniform_int_distribution<int> dist(1, 100);

    int iterations = 10;

    for ([[maybe_unused]] int idx : std::views::iota(0, iterations)) {
        int num = dist(engine);
        EXPECT_THAT(num, AllOf(Ge(1), Le(100)));
    }
}