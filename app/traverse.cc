#include <algorithm>
#include <iterator>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
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
    std::vector<int64_t> copyed;
    for (int i = static_cast<int>(vec.size()) - 1; i >= 0; --i) {
        copyed.emplace_back(vec.at(i));
    }
    EXPECT_THAT(copyed, ::testing::ElementsAre(10, 9, 8, 7, 6, 5, 4, 3, 2, 1));
}

TEST(traverse, basicsafe) {
    std::vector<int64_t> copyed;
    for (int i = 0; std::cmp_less(i, vec.size()); i++) {
        copyed.emplace_back(vec.at(i));
    }
    EXPECT_THAT(copyed, ::testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
}

TEST(traverse, iter) {
    std::vector<int64_t> copyed;
    for (auto iter = std::rbegin(vec); iter != std::rend(vec); iter++) {
        copyed.emplace_back(*iter);
    }
    EXPECT_THAT(copyed, ::testing::ElementsAre(10, 9, 8, 7, 6, 5, 4, 3, 2, 1));
}

TEST(traverse, loop) {
    std::unordered_map<int, int64_t> actual;
    using namespace ::testing;

    for (int i = 0; const auto& ele : vec) {
        actual[i++] = ele;
    }
    EXPECT_THAT(
        actual,
        UnorderedElementsAre(
            Pair(0, 1),
            Pair(1, 2),
            Pair(2, 3),
            Pair(3, 4),
            Pair(4, 5),
            Pair(5, 6),
            Pair(6, 7),
            Pair(7, 8),
            Pair(8, 9),
            Pair(9, 10)));
}

TEST(traverse, rangerev) {
    std::unordered_map<int, int64_t> actual;
    using namespace ::testing;

    for (int i = 0; const auto& ele : vec | std::views::reverse) {
        actual[i++] = ele;
    }

    EXPECT_THAT(
        actual,
        UnorderedElementsAre(
            Pair(0, 10),
            Pair(1, 9),
            Pair(2, 8),
            Pair(3, 7),
            Pair(4, 6),
            Pair(5, 5),
            Pair(6, 4),
            Pair(7, 3),
            Pair(8, 2),
            Pair(9, 1)));
}

TEST(traverse, ranges) {
    std::unordered_map<int, int64_t> actual;
    using namespace ::testing;
    std::ranges::for_each(
        std::views::zip(std::ranges::iota_view{0, std::ssize(vec)}, vec) | std::views::reverse,
        [&](const auto& elem) -> void { actual[std::get<0>(elem)] = std::get<1>(elem); });

    EXPECT_THAT(
        actual,
        UnorderedElementsAre(
            Pair(9, 10),
            Pair(8, 9),
            Pair(7, 8),
            Pair(6, 7),
            Pair(5, 6),
            Pair(4, 5),
            Pair(3, 4),
            Pair(2, 3),
            Pair(1, 2),
            Pair(0, 1)));
}