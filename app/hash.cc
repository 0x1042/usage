#include "absl/hash/hash.h"

#include <cstdint>
#include <string>
#include <vector>

#include "absl/hash/internal/city.h"
#include "app/log.h"
#include "gtest/gtest.h"

TEST(Hash, city) {
    std::string original = "https://abseil.io/fast/99";
    auto hashcode = absl::hash_internal::CityHash64(original.c_str(), original.size());
    INFO("hash code {}", hashcode);
}

struct User {
    int64_t id;
    std::string name;
    std::vector<std::string> tags;
    // C++20 自动生成 == 和 !=
    auto operator==(const User&) const -> bool = default;

    template <typename H>
    friend auto AbslHashValue(H h, const User& u) -> H {
        return H::combine(std::move(h), u.id, u.name, u.tags);
    }
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    // 这是一个经典的魔数，用于分布均匀
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
template <>
struct hash<User> {
    auto operator()(const User& u) const noexcept -> size_t {
        size_t seed = 0;
        hash_combine(seed, u.id);
        hash_combine(seed, u.name);

        for (const auto& tag : u.tags) {
            hash_combine(seed, tag);
        }
        return seed;
    }
};
} // namespace std

TEST(Hash, std) {
    User u{.id = 1, .name = "Alice", .tags = {"hello", "world"}};
    INFO("hash with std is {}", std::hash<User>{}(u));
}

TEST(Hash, absl) {
    User u{.id = 1, .name = "Alice", .tags = {"hello", "world"}};
    INFO("hash with absl is {}", absl::Hash<User>{}(u));
}
