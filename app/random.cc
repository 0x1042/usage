#include "absl/random/random.h"

#include "absl/log/log.h"
#include "gtest/gtest.h"

TEST(random, uniform) {
    absl::BitGen gen;
    for (int idx : std::views::iota(0, 10)) {
        LOG(INFO) << "random " << idx << " int : " << absl::Uniform(gen, 1, 101);
        LOG(INFO) << "random " << idx << " float :" << absl::Uniform(gen, 0.0, 1.0);
        LOG(INFO) << "random " << idx << " Bernoulli :" << absl::Bernoulli(gen,0.5);
    }
}
