#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

#include "app/log.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

template <typename T>
struct AlwaysFalse : std::false_type {};

template <typename T>
constexpr bool always_false_v = AlwaysFalse<T>::value;

template <typename T>
auto get_value(const rapidjson::Document& doc, const char* path, T& dst) {
    rapidjson::Pointer pointer(path);
    if (!pointer.IsValid()) {
        ERROR("Invalid JSON pointer syntax: {}", path);
        return;
    }

    const rapidjson::Value* value = pointer.Get(doc);
    if (!value) {
        return;
    }

    if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        if (value->IsString()) {
            dst = value->GetString();
        }
        return;
    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int32_t>) {
        if (value->IsInt()) {
            dst = value->GetInt();
        }
        return;
    } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, long long>) {
        if (value->IsInt64()) {
            dst = value->GetInt64();
        }
        return;
    } else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, uint32_t>) {
        if (value->IsUint64()) {
            dst = value->GetUint64();
        }
        return;
    } else if constexpr (
        std::is_floating_point_v<T> || std::is_same_v<T, double> || std::is_same_v<T, float>) {
        if (value->IsDouble()) {
            dst = value->GetDouble();
        }
        return;
    } else if constexpr (std::is_same_v<T, bool>) {
        if (value->IsBool()) {
            dst = value->GetBool();
        }
        return;
    } else {
        static_assert(always_false_v<T>, "Unsupported type for JSON deserialization");
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

#define SET(key) settter(doc, #key, key)
#define GET(key) get_value(doc, "/" #key, key)

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

    [[nodiscard]] auto toStrV2() const -> std::string {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();

        writer.Key("id");
        writer.Int64(id);

        writer.Key("score");
        writer.Double(score);

        writer.Key("coef");
        writer.Double(coef);

        writer.Key("boost");
        writer.Int(boost);

        writer.Key("enable");
        writer.Bool(enable);

        writer.EndObject();

        return buffer.GetString();
    }

    Item() = default;

    explicit Item(const std::string& json) {
        rapidjson::Document doc;
        doc.Parse(json.data());

        if (doc.HasParseError()) {
            return;
        }
        GET(id);
        GET(score);
        GET(coef);
        GET(boost);
        GET(enable);
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

    INFO("item info {}", item.toStr());
    INFO("item info v2 {}", item.toStrV2());
    EXPECT_EQ(item.toStr(), item.toStrV2());
}

TEST(tojson, fromjson) {
    const std::string tmp
        = R"({"id":1024,"score":3.14,"coef":2.0999999046325684,"boost":2,"enable":true})";

    json::Item item(tmp);

    INFO("item info {}", item.toStr());
}

#undef SET
#undef GET