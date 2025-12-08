#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

#include "absl/log/log.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

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
};
} // namespace json

TEST(tojson, tojson) {
    json::Item item = {.id = 1024, .score = 3.14, .coef = 2.1, .boost = 2,.enable = true};

    LOG(INFO) << item.toStr();
}