#include <algorithm>
#include <iterator>
#include <ranges>
#include <utility>
#include <vector>

#include "absl/log/log.h"
#include "gtest/gtest.h"

namespace {
const std::vector<int64_t> vec = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
};

}

TEST(traverse, basic) {
    for (int i = static_cast<int>(vec.size()) - 1; i >= 0; --i) {
        LOG(INFO) << "traverse by basic "
                  << "idx=[" << i << "]=" << vec.at(i);
    }
}

TEST(traverse, basicsafe) {
    for (int i = 0; std::cmp_less(i, vec.size()); i++) {
        LOG(INFO) << "traverse by basicsafe "
                  << "idx=[" << i << "]=" << vec[i];
    }
}

TEST(traverse, iter) {
    for (auto iter = std::rbegin(vec); iter != std::rend(vec); iter++) {
        LOG(INFO) << "traverse by iter " << *iter;
    }
}

TEST(traverse, loop) {
    for (int i = 0; const auto& ele : vec) {
        LOG(INFO) << "traverse by loop "
                  << "idx=[" << i++ << "]=" << ele;
    }
}

TEST(traverse, rangerev) {
    for (int i = 0; const auto& ele : vec | std::views::reverse) {
        LOG(INFO) << "traverse by rangerev "
                  << "idx=[" << i++ << "]=" << ele;
    }
}

TEST(traverse, ranges) {
    std::ranges::for_each(
        std::views::zip(std::ranges::iota_view{0, std::ssize(vec)}, vec) | std::views::reverse,
        [](const auto& elem) -> void {
            LOG(INFO) << "traverse by ranges " << std::get<0>(elem) << "idx =" << std::get<1>(elem);
        });
}