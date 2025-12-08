#include "compressor.h"

#include <cstdint>
#include <memory>

#include "lz4.h"
#include "lz4hc.h"
#include "snappy.h"
#include "zstd.h"

struct Lz4Impl final : Compressor {
    Lz4Impl() = default;
    Lz4Impl(const Lz4Impl&) = default;
    Lz4Impl(Lz4Impl&&) = default;
    auto operator=(const Lz4Impl&) -> Lz4Impl& = default;
    auto operator=(Lz4Impl&&) -> Lz4Impl& = default;
    ~Lz4Impl() override = default;

    [[nodiscard]] auto compress(const std::string& data) const -> std::string override {
        if (data.empty()) {
            return "";
        }

        const auto original_size = static_cast<uint64_t>(data.size());
        const int src_size = static_cast<int>(data.size());

        // 计算压缩后所需的最大缓冲区大小
        const int max_dst_size = LZ4_compressBound(src_size);
        if (max_dst_size == 0) {
            throw std::runtime_error("LZ4_compressBound failed, input size too large?");
        }

        // 创建一个足够大的结果字符串，头部留出8字节给原始大小
        std::string result(sizeof(uint64_t) + max_dst_size, '\0');

        // 将原始大小写入结果字符串的头部
        *reinterpret_cast<uint64_t*>(result.data()) = original_size; // NOLINT

        // 压缩数据，写入头部之后的空间
        char* compressed_data_ptr = result.data() + sizeof(uint64_t); // NOLINT
        const int compressed_size = LZ4_compress_default(data.c_str(), compressed_data_ptr, src_size, max_dst_size);

        if (compressed_size <= 0) {
            throw std::runtime_error("LZ4_compress_default failed.");
        }

        // 调整结果字符串的大小为实际大小 (头部 + 压缩数据)
        result.resize(sizeof(uint64_t) + compressed_size);

        result.shrink_to_fit();

        return result;
    }

    [[nodiscard]] auto decompress(const std::string& data) const -> std::string override {
        if (data.empty()) {
            return "";
        }

        // 检查输入数据是否至少包含头部信息
        if (data.size() < sizeof(uint64_t)) {
            throw std::runtime_error("Invalid compressed data: too short to contain size header.");
        }

        // 从数据头部读取原始大小
        const uint64_t original_size = *reinterpret_cast<const uint64_t*>(data.c_str()); // NOLINT

        // 获取压缩数据的指针和大小
        const char* compressed_data_ptr = data.c_str() + sizeof(uint64_t); // NOLINT
        const int compressed_size = static_cast<int>(data.size() - sizeof(uint64_t));

        // 创建用于存放解压后数据的字符串
        std::string result(original_size, '\0');

        // 调用解压函数
        const int decompressed_size
            = LZ4_decompress_safe(compressed_data_ptr, result.data(), compressed_size, static_cast<int>(original_size));

        // 检查解压是否成功
        if (decompressed_size < 0) {
            throw std::runtime_error(
                "LZ4_decompress_safe failed with error code: " + std::to_string(decompressed_size));
        }
        if (static_cast<uint64_t>(decompressed_size) != original_size) {
            throw std::runtime_error("Decompression failed: size mismatch.");
        }

        return result;
    }
};

struct ZstdImpl final : Compressor {
    ZstdImpl() = default;
    ZstdImpl(ZstdImpl&&) = delete;
    auto operator=(const ZstdImpl&) -> ZstdImpl& = default;
    auto operator=(ZstdImpl&&) -> ZstdImpl& = delete;
    ZstdImpl(const ZstdImpl&) = default;
    ~ZstdImpl() override = default;

    [[nodiscard]] auto compress(const std::string& data) const -> std::string override {
        if (data.empty()) {
            return "";
        }

        size_t const max_dst_size = ZSTD_compressBound(data.size());
        std::string result(max_dst_size, '\0');

        size_t const compressed_size = ZSTD_compress(
            result.data(),
            max_dst_size,
            data.c_str(),
            data.size(),
            ZSTD_CLEVEL_DEFAULT // 默认压缩级别
        );

        if (ZSTD_isError(compressed_size)) { // NOLINT
            throw std::runtime_error("Zstd compression failed: " + std::string(ZSTD_getErrorName(compressed_size)));
        }

        result.resize(compressed_size);
        return result;
    }

    [[nodiscard]] auto decompress(const std::string& data) const -> std::string override {
        if (data.empty()) {
            return "";
        }

        // 从压缩帧中获取原始大小
        uint64_t original_size = ZSTD_getFrameContentSize(data.c_str(), data.size());

        if (original_size == ZSTD_CONTENTSIZE_ERROR || original_size == ZSTD_CONTENTSIZE_UNKNOWN) {
            throw std::runtime_error("Zstd: failed to get decompressed size from header.");
        }
        if (original_size == 0) {
            return ""; // 原始数据是空的
        }

        std::string result(original_size, '\0');

        size_t const decompressed_size = ZSTD_decompress(result.data(), original_size, data.c_str(), data.size());

        if (ZSTD_isError(decompressed_size)) { // NOLINT
            throw std::runtime_error("Zstd decompression failed: " + std::string(ZSTD_getErrorName(decompressed_size)));
        }
        if (decompressed_size != original_size) {
            throw std::runtime_error("Zstd decompression size mismatch.");
        }

        return result;
    }
};

struct SnappyImpl final : Compressor {
    SnappyImpl() = default;

    SnappyImpl(const SnappyImpl&) = default;
    SnappyImpl(SnappyImpl&&) = delete;
    auto operator=(const SnappyImpl&) -> SnappyImpl& = default;
    auto operator=(SnappyImpl&&) -> SnappyImpl& = delete;
    ~SnappyImpl() override = default;

    [[nodiscard]] auto compress(const std::string& data) const -> std::string override {
        std::string compressed_data;
        snappy::Compress(data.data(), data.size(), &compressed_data);
        return compressed_data;
    }

    [[nodiscard]] auto decompress(const std::string& data) const -> std::string override {
        std::string uncompressed_data;
        if (!snappy::Uncompress(data.data(), data.size(), &uncompressed_data)) {
            throw std::runtime_error("Snappy decompression failed.");
        }
        return uncompressed_data;
    }
};

auto Compressor::create(Type type) -> std::unique_ptr<Compressor> {
    switch (type) {
        case Type::LZ4:
            return std::make_unique<Lz4Impl>();
        case Type::ZSTD:
            return std::make_unique<ZstdImpl>();
        case Type::SNAPPY:
            return std::make_unique<SnappyImpl>();
        default:
            return std::make_unique<Lz4Impl>();
    }
}