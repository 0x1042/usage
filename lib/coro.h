#pragma once

#include <coroutine>
#include <exception>
#include <variant>

template <typename T>
class Task {
public:
    struct promise_type {
        std::variant<std::monostate, T, std::exception_ptr> result;

        auto get_return_object() -> Task {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        auto initial_suspend() noexcept -> std::suspend_never { return {}; }

        auto final_suspend() noexcept -> std::suspend_never { return {}; }

        void return_void() {}

        void unhandled_exception() { result = std::current_exception(); }

        template <typename U>
        void return_value(U&& value) {
            result = std::forward<U>(value);
        }

        auto get_result() -> T {
            if (std::holds_alternative<std::exception_ptr>(result)) {
                std::rethrow_exception(std::get<std::exception_ptr>(result));
            }
            return std::get<T>(std::move(result));
        }
    };

    void resume() {
        if (!handle_.done()) {
            handle_.resume();
        }
    }

    [[nodiscard]] auto done() const -> bool { return handle_.done(); }

    auto get_result() -> T { return handle_.promise().get_result(); }

    auto get_handle() -> std::coroutine_handle<promise_type> { return handle_; }

    Task(const Task&) = delete;
    auto operator=(const Task&) -> Task& = delete;

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    auto operator=(Task&& other) noexcept -> Task& {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    Task(std::coroutine_handle<promise_type> handle) : handle_(handle) {}

    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }

private:
    std::coroutine_handle<promise_type> handle_;
};

template <>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr exception;

        auto get_return_object() -> Task {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        static auto initial_suspend() -> std::suspend_never { return {}; }

        static auto final_suspend() noexcept -> std::suspend_never { return {}; }

        void return_void() {}

        void unhandled_exception() { exception = std::current_exception(); }

        auto get_result() const {
            if (exception) {
                std::rethrow_exception(exception);
            }
        }
    };

    void resume() {
        if (!handle_.done()) {
            handle_.resume();
        }
    }

    [[nodiscard]] auto done() const -> bool { return handle_.done(); }

    void get_result() { handle_.promise().get_result(); }

    auto get_handle() -> std::coroutine_handle<promise_type> { return handle_; }

    Task(const Task&) = delete;
    auto operator=(const Task&) -> Task& = delete;

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    auto operator=(Task&& other) noexcept -> Task& {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    Task(std::coroutine_handle<promise_type> handle) : handle_(handle) {}

    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }

private:
    std::coroutine_handle<promise_type> handle_;
};