#include <cstdint>

#include "absl/log/log.h"
#include "gtest/gtest.h"
#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_all.hpp"

enum class Color : int16_t {
    NONE = -999,
    RED = -10,
    BLUE = 0,
    GREEN = 10,
};

TEST(enum, tostr) {
    Color c1 = Color::RED;
    LOG(INFO) << "c1 name is " << magic_enum::enum_name(c1);

    magic_enum::enum_for_each<Color>([](auto val) -> void {
        constexpr Color color = val;
        LOG(INFO) << "color name is " << magic_enum::enum_name(color);
    });
}

TEST(enum, fromstr) {
    std::string color_name{"GREEN"};
    auto color = magic_enum::enum_cast<Color>(color_name, magic_enum::case_insensitive)
                     .value_or(Color::NONE);

    LOG(INFO) << "success parsed." << magic_enum::enum_name(color);
}

TEST(enum, tointeger) {
    Color c1 = Color::GREEN;

    LOG(INFO) << "color_integer is " << magic_enum::enum_integer(c1);

    magic_enum::enum_for_each<Color>([](auto val) -> void {
        constexpr Color color = val;
        LOG(INFO) << "color_integer is " << magic_enum::enum_integer(color);
    });
}

TEST(enum, frominteger) {
    int16_t value = 2;
    auto color = magic_enum::enum_cast<Color>(value).value_or(Color::NONE);
    LOG(INFO) << "success parsed." << magic_enum::enum_name(color);
}