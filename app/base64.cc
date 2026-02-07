#include "absl/strings/escaping.h"
#include "gtest/gtest.h"
#include "lib/log.h"

TEST(base64, encode) {
    std::string original = "https://abseil.io/fast/99";
    std::string encoded = absl::Base64Escape(original);
    INFO("base64 encode is {}", encoded);

    EXPECT_EQ("aHR0cHM6Ly9hYnNlaWwuaW8vZmFzdC85OQ==", encoded);
}

TEST(base64, decode) {
    std::string encoded = "SGVsbG8sIFN5c3RlbSBEZXYh";
    std::string decoded;
    bool status = absl::Base64Unescape(encoded, &decoded);
    EXPECT_TRUE(status);
    INFO("decode success {}", decoded);

    EXPECT_EQ("Hello, System Dev!", decoded);
}
