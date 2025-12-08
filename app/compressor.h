
#pragma once

#include <cstdint>
#include <memory>
#include <string>

struct Compressor {
    enum class Type : uint8_t {
        SNAPPY,
        LZ4,
        ZSTD,
    };

    Compressor() = default;
    Compressor(const Compressor&) = default;
    Compressor(Compressor&&) = default;
    auto operator=(const Compressor&) -> Compressor& = default;
    auto operator=(Compressor&&) -> Compressor& = default;
    virtual ~Compressor() = default;

    [[nodiscard]] virtual auto compress(const std::string& data) const -> std::string = 0;
    [[nodiscard]] virtual auto decompress(const std::string& data) const -> std::string = 0;

    static auto create(Type type) -> std::unique_ptr<Compressor>;
};
