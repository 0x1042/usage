#include <cstdlib>

#include "compressor.h"
#include "gtest/gtest.h"
#include "log.h"

class CompressorTest : public ::testing::Test {
public:
    void SetUp() override {
        const auto& bin_dir = std::getenv("PATH");

        if (bin_dir != nullptr) {
            origin = std::string(bin_dir);
        }
        INFO("origin size is {}", origin.size());
    }

    std::string origin;
};

TEST_F(CompressorTest, lz4) {
    auto lz4 = Compressor::create(Compressor::Type::LZ4);
    auto compressed = lz4->compress(origin);
    INFO("lz4 compressed ratio {}", float(compressed.size()) / float(origin.size()));
    auto decompressed = lz4->decompress(compressed);
    INFO("lz4 decompressed size {}", decompressed.size());
}

TEST_F(CompressorTest, snappy) {
    auto lz4 = Compressor::create(Compressor::Type::SNAPPY);
    auto compressed = lz4->compress(origin);
    INFO("snappy compressed ratio {}", float(compressed.size()) / float(origin.size()));
    auto decompressed = lz4->decompress(compressed);
    INFO("snappy decompressed size {}", decompressed.size());
}

TEST_F(CompressorTest, zstd) {
    auto lz4 = Compressor::create(Compressor::Type::ZSTD);
    auto compressed = lz4->compress(origin);
    INFO("zstd compressed ratio {}", float(compressed.size()) / float(origin.size()));
    auto decompressed = lz4->decompress(compressed);
    INFO("zstd decompressed size {}", decompressed.size());
}