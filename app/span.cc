#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include "absl/log/log.h"
#include "gtest/gtest.h"

TEST(span, arr) {
    int arr[] = {1, 2, 3, 4, 5}; // NOLINT
    std::span<int, 5> link{arr};
    EXPECT_EQ(link.size(), 5);
}

TEST(span, vec1) {
    std::vector<int64_t> vec = {1, 2, 3, 4, 5};
    std::span<int64_t> link{vec};
    EXPECT_EQ(link.size(), 5);
    EXPECT_EQ(*link.begin(), 1);
}

TEST(span, vec2) {
    std::vector<int64_t> vec = {1, 2, 3, 4, 5};
    std::span<int64_t> link(vec.begin(), 3);
    EXPECT_EQ(link.size(), 3);
    EXPECT_EQ(*link.begin(), 1);
}

TEST(span, vec3) {
    std::vector<int64_t> vec = {1, 2, 3, 4, 5};
    std::span<int64_t> link(vec.begin(), vec.end());
    EXPECT_EQ(link.size(), vec.size());
    EXPECT_EQ(*link.begin(), 1);
}

TEST(span, sub) {
    std::vector<int64_t> vec = {1, 2, 3, 4, 5, 6, 7};
    std::span<int64_t, 5> link(vec.begin(), vec.end());

    std::span<int64_t> sub = link.subspan(2, 3);

    auto str_fn = [](std::span<int64_t> sp) -> void {
        std::string buf;
        buf.reserve(sp.size() * 2);

        for (auto&& elem : sp) {
            buf.append(std::to_string(elem));
            buf.append(" ");
        }

        LOG(INFO) << "span info. " << buf;
    };

    str_fn(sub);
    str_fn(link.first(3));
    str_fn(link.last(2));
}