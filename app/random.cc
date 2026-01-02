#include "absl/random/random.h"

#include "absl/log/log.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(random, uniform) {
    using namespace ::testing;
    absl::BitGen gen;
    int iterations = 10;

    for ([[maybe_unused]] int idx : std::views::iota(0, iterations)) {
        EXPECT_THAT(absl::Uniform(gen, 1, 101), AllOf(Ge(1), Lt(101)));
        EXPECT_THAT(absl::Uniform(gen, 0.0, 1.0), AllOf(Ge(0.0), Lt(1.0)));
    }

    LOG(INFO) << "Bernoulli." << absl::Bernoulli(gen, 0.5);
}
