#include <system_error>

#include "app/log.h"
#include "gtest/gtest.h"
#include "tl/expected.hpp"

auto mock(const std::string& input) -> tl::expected<std::size_t, std::error_code> {
    if (input.empty()) {
        return tl::make_unexpected(std::make_error_code(std::errc::invalid_argument));
    }
    return input.size();
}

TEST(expected, map) {
    auto len = mock("hello").map([](std::size_t size) -> std::size_t { return size * 2; });
    EXPECT_EQ(len.value(), 5 * 2);
}

TEST(expected, value) {
    auto len1 = mock("hello").value_or(20);
    EXPECT_EQ(len1, 5);

    auto len2 = mock("").value_or(20);
    EXPECT_EQ(len2, 20);
}
