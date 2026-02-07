#include <cstdint>
#include <format>
#include <numbers>
#include <string>
#include <string_view>
#include <vector>

#include "gtest/gtest.h"
#include "lib/log.h"

TEST(fmt, format) {
    std::string_view world = "world";
    EXPECT_EQ(std::format("Hello, {}", world), "Hello, world");
    EXPECT_EQ(std::format("{1}, {0}!", "world", "hello"), "hello, world!");
    EXPECT_EQ(std::format("{:.3f}", std::numbers::pi), "3.142");
}

struct Tag {
    std::string key;
    std::string val;
};

template <>
struct std::formatter<Tag> {
    // 1. parse: 解析格式字符串
    // 上下文 ctx 包含了格式字符串的迭代器
    static constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    // 2. format: 输出内容
    // const Point& p: 要格式化的对象
    // FormatContext& ctx: 格式化上下文，提供输出迭代器
    static auto format(const Tag& tag, std::format_context& ctx) {
        // std::format_to 向 ctx.out() 写入数据
        return std::format_to(ctx.out(), "({} -> {})", tag.key, tag.val);
    }
};

template <typename V>
struct RangeFormatter {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    static auto format(const V& range, std::format_context& ctx) {
        auto out = ctx.out();
        out = std::format_to(out, "[");
        bool first = true;
        for (const auto& tmp : range) {
            if (!first) {
                out = std::format_to(out, ", ");
            }
            first = false;
            // 递归格式化元素：item 会自动寻找它自己的 formatter
            out = std::format_to(out, "{}", tmp);
        }
        return std::format_to(out, "]");
    }
};

template <>
struct std::formatter<std::vector<Tag>> : RangeFormatter<std::vector<Tag>> {};

TEST(fmt, formatTag) {
    Tag tag = {.key = "hello", .val = "world"};
    EXPECT_EQ(std::format("tag is {}", tag), "tag is (hello -> world)");
}

TEST(fmt, formatTags) {
    std::vector<Tag> tags;
    for (int i = 0; i < 10; i++) {
        Tag tag = {.key = std::to_string(i), .val = std::to_string(i + 1)};
        tags.emplace_back(tag);
    }

    EXPECT_EQ(
        "[(0 -> 1), (1 -> 2), (2 -> 3), (3 -> 4), (4 -> 5), (5 -> 6), (6 -> 7), (7 -> 8), (8 -> "
        "9), (9 -> 10)]",
        std::format("{}", tags));
}

struct Item {
    int64_t id = 0;
    std::string brand;
    double coef = 1.0;
    std::vector<Tag> tags;
};

template <>
struct std::formatter<Item> {
    static constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    static auto format(const Item& item, std::format_context& ctx) {
        auto out = ctx.out();
        out = std::format_to(out, "id:{} brand:{} coef:{} ", item.id, item.brand, item.coef);
        return std::format_to(out, "tags:{}", item.tags);
    }
};

TEST(fmt, formatItem) {
    std::vector<Tag> tags;
    for (int i = 0; i < 10; i++) {
        Tag tag = {.key = std::to_string(i), .val = std::to_string(i + 1)};
        tags.emplace_back(tag);
    }

    Item item;
    item.id = 1024;
    item.brand = "KFC";
    item.coef = 1.23;
    item.tags = tags;

    INFO("item info {}", std::format("{}", item));
}

template <typename... Args>
void log_vformat(const std::string& fmt, Args&&... args) { // NOLINT
    auto fmt_args = std::make_format_args(args...);
    std::string out = std::vformat(fmt, fmt_args);
    std::cout << "[LOG] " << out << '\n';
}

TEST(fmt, log) {
    log_vformat("User {} logged in from IP {}", "Alice", "192.168.1.100");
    log_vformat("{} {} {}", "hello", 1.23, "world");
}

TEST(format, logv2) {
    int id2 = 42;
    std::string name = "System";
    INFO("User {} logged in with ID {}", name, id2);
    WARN("Disk space is low: {}%", 15);
}