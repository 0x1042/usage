#include <cstdlib>
#include <format>
#include <fstream>
#include <ios>
#include <iterator>
#include <stdexcept>

#include "absl/log/log.h"
#include "compressor.h"
#include "gtest/gtest.h"

class CompressorTest : public ::testing::Test {
public:
    void SetUp() override {
        const auto& bin_dir = std::getenv("BUILD_WORKING_DIRECTORY");
        const auto& path = std::format("{}/{}", bin_dir, "MODULE.bazel.lock");
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("invalid path");
        }
        origin = {(std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()};
        LOG(INFO) << "origin size is " << origin.size();
    }

    std::string origin;
};

TEST_F(CompressorTest, lz4) {
    auto lz4 = Compressor::create(Compressor::Type::LZ4);
    auto compressed = lz4->compress(origin);
    LOG(INFO) << "lz4 compressed ratio " << float(compressed.size()) / float(origin.size());
    auto decompressed = lz4->decompress(compressed);
    LOG(INFO) << "lz4 decompressed size is " << decompressed.size();
}

TEST_F(CompressorTest, snappy) {
    auto lz4 = Compressor::create(Compressor::Type::SNAPPY);
    auto compressed = lz4->compress(origin);
    LOG(INFO) << "snappy compressed ratio " << float(compressed.size()) / float(origin.size());
    auto decompressed = lz4->decompress(compressed);
    LOG(INFO) << "snappy decompressed size is " << decompressed.size();
}

TEST_F(CompressorTest, zstd) {
    auto lz4 = Compressor::create(Compressor::Type::ZSTD);
    auto compressed = lz4->compress(origin);
    LOG(INFO) << "zstd compressed ratio " << float(compressed.size()) / float(origin.size());
    auto decompressed = lz4->decompress(compressed);
    LOG(INFO) << "zstd decompressed size is " << decompressed.size();
}