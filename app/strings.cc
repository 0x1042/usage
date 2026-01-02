#include "absl/strings/ascii.h"
#include "absl/strings/cord.h"
#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(strings, split) {
    std::string tmp = "apple,banana,cherry,";
    std::vector<absl::string_view> vec1 = absl::StrSplit(tmp, ',');
    EXPECT_THAT(vec1, ::testing::ElementsAre("apple", "banana", "cherry", ""));

    std::vector<absl::string_view> vec2 = absl::StrSplit("apple,banana,cherry,", ',', absl::SkipEmpty());
    EXPECT_THAT(vec2, ::testing::ElementsAre("apple", "banana", "cherry"));
}

TEST(strings, parse) {
    {
        const std::string str = "123";
        int64_t num = 0;
        bool status = absl::SimpleAtoi(str, &num);
        EXPECT_TRUE(status);
    }

    {
        const std::string str = "123e";
        int64_t num = 0;
        bool status = absl::SimpleAtoi(str, &num);
        EXPECT_FALSE(status);
    }
}

TEST(strings, prefix) {
    EXPECT_TRUE(absl::StartsWith("hello world", "hello"));
    EXPECT_FALSE(absl::StartsWith("hello world", "ello"));
}

TEST(strings, suffix) {
    EXPECT_TRUE(absl::EndsWith("hello world", "rld"));
    EXPECT_FALSE(absl::EndsWith("hello world", "hello"));
}

TEST(strings, join) {
    std::vector<std::string> v = {"foo", "bar", "baz"};
    EXPECT_EQ(absl::StrJoin(v, "-"), "foo-bar-baz");
    std::vector<int> nums = {1, 2, 3};
    EXPECT_EQ(absl::StrJoin(nums, ","), "1,2,3");
}

TEST(strings, ascii) {
    {
        std::string tmp = "Hello World!";
        absl::AsciiStrToLower(&tmp);
        EXPECT_EQ(tmp, "hello world!");
    }

    {
        std::string tmp = "Hello World!";
        absl::AsciiStrToUpper(&tmp);
        EXPECT_EQ(tmp, "HELLO WORLD!");
    }
}

TEST(strings, cord) {
    absl::Cord cord("Initial data");
    cord.Append(" - appended content");
    cord.Prepend("Header: ");
    std::string final_str = std::string(cord);
    EXPECT_EQ("Header: Initial data - appended content", final_str);
}