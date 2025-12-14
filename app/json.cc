#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

#include "absl/log/log.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

template <typename T>
auto get_pointer(const rapidjson::Document& doc, const char* path) -> std::optional<T> {
    const rapidjson::Value* value = rapidjson::Pointer(path).Get(doc);
    if (!value) {
        return std::nullopt;
    }

    if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        return value->IsString() ? std::optional<T>{value->GetString()} : std::nullopt;
    } else if constexpr (std::is_same_v<T, int>) {
        return value->IsInt() ? std::optional<T>{value->GetInt()} : std::nullopt;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return value->IsInt64() ? std::optional<T>{value->GetInt64()} : std::nullopt;
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        return value->IsUint64() ? std::optional<T>{value->GetUint64()} : std::nullopt;
    } else if constexpr (std::is_floating_point_v<T>) {
        return value->IsDouble() ? std::optional<T>{value->GetDouble()} : std::nullopt;
    } else if constexpr (std::is_same_v<T, bool>) {
        return value->IsBool() ? std::optional<T>{value->GetBool()} : std::nullopt;
    } else {
        static_assert(false, "Unsupported type");
    }
}

template <typename T>
void settter(rapidjson::Document& doc, std::string_view key, const T& target) {
    rapidjson::Value kval(key.data(), key.size(), doc.GetAllocator());

    if constexpr (std::is_arithmetic_v<T> or std::is_same_v<T, bool>) {
        rapidjson::Value vval(target);
        doc.AddMember(kval.Move(), vval.Move(), doc.GetAllocator());
    }

    if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>) {
        rapidjson::Value vval(target.c_str(), target.size(), doc.GetAllocator());
        doc.AddMember(kval.Move(), vval.Move(), doc.GetAllocator());
    }

    if constexpr (std::is_convertible_v<T, std::string_view>) {
        rapidjson::Value vval(target.data(), target.size(), doc.GetAllocator());
        doc.AddMember(kval.Move(), vval.Move(), doc.GetAllocator());
    }

    if constexpr (std::is_null_pointer_v<T> or std::is_same_v<T, std::nullptr_t>) {
        rapidjson::Value vval;
        vval.SetNull();
        doc.AddMember(kval.Move(), vval.Move(), doc.GetAllocator());
    }
}

#define SET(key) settter(doc, #key, id)

namespace json {
struct Item {
    int64_t id = 0;
    double score = 0;
    float coef = 1.0;
    int32_t boost = 1;
    bool enable = false;

    [[nodiscard]] auto toStr() const -> std::string {
        rapidjson::Document doc;
        doc.SetObject();

        SET(id);
        SET(score);
        SET(coef);
        SET(boost);
        SET(enable);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        return buffer.GetString();
    }

    Item() = default;

    explicit Item(const std::string& json) {
        rapidjson::Document doc;
        doc.Parse(json.data());

        if (doc.HasParseError()) {
            return;
        }
    }
};
} // namespace json

TEST(tojson, tojson) {
    json::Item item;
    item.id = 1024;
    item.score = 3.14;
    item.coef = 2.1;
    item.boost = 2;
    item.enable = true;
    LOG(INFO) << item.toStr();
}

TEST(tojson, fromjson) {
    json::Item item;

    LOG(INFO) << item.toStr();
}