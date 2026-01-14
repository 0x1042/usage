#pragma once

#include <chrono>
#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

enum class LogLevel : uint8_t {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
};

inline void log_impl(
    LogLevel level,
    std::chrono::time_point<std::chrono::system_clock> now,
    const std::source_location& loc,
    std::string_view message) {
    // 简单的颜色代码
    const char* color_code = "";
    const char* level_str = "";

    switch (level) {
        case LogLevel::TRACE:
            level_str = "TRACE";
            color_code = "\033[32m";
            break;
        case LogLevel::DEBUG:
            level_str = "DEBUG";
            color_code = "\033[32m";
            break;
        case LogLevel::INFO:
            level_str = "INFO";
            color_code = "\033[32m";
            break;
        case LogLevel::WARN:
            level_str = "WARN";
            color_code = "\033[33m";
            break;
        case LogLevel::ERROR:
            level_str = "ERROR";
            color_code = "\033[31m";
            break;
    }
    const char* reset_code = "\033[0m";

    // 格式化最终日志条目
    // C++20 chrono 也可以直接被 format
    // {:%F %T} 是 YYYY-MM-DD HH:MM:SS 格式
    std::string final_log = std::format(
        "[{:%F %T}] [{}] [{}:{}] {}\n",
        std::chrono::floor<std::chrono::seconds>(now), // 时间
        level_str, // 等级文本
        loc.file_name(), // 文件名
        loc.line(), // 行号
        message // 用户格式化好的消息
    );
    std::clog << color_code << final_log << reset_code;
}

template <typename... Args>
void log(
    LogLevel level,
    const std::source_location& loc,
    std::format_string<Args...> fmt,
    Args&&... args) {
    std::string user_msg = std::format(fmt, std::forward<Args>(args)...);
    log_impl(level, std::chrono::system_clock::now(), loc, user_msg);
}

#define TRACE(fmt, ...) log(LogLevel::TRACE, std::source_location::current(), fmt, __VA_ARGS__)
#define DEBUG(fmt, ...) log(LogLevel::DEBUG, std::source_location::current(), fmt, __VA_ARGS__)
#define INFO(fmt, ...) log(LogLevel::INFO, std::source_location::current(), fmt, __VA_ARGS__)
#define WARN(fmt, ...) log(LogLevel::WARN, std::source_location::current(), fmt, __VA_ARGS__)
#define ERROR(fmt, ...) log(LogLevel::ERROR, std::source_location::current(), fmt, __VA_ARGS__)
