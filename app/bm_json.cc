#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "benchmark/benchmark.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

struct Item {
    int64_t id = 123456789;
    double score = 98.76;
    float coef = 1.23F;
    bool offline = false;
    std::string brand = "ExampleBrandName";
    std::vector<int64_t> spuids = {1001, 1002, 1003, 1004, 1005};
    std::unordered_map<std::string, std::string> strs = {{"key1", "val1"}, {"key2", "val2"}};
    std::unordered_map<std::string, double> dbls = {{"metric1", 0.5}, {"metric2", 1.5}};
};

void item_to_json_by_doc(const Item& item, std::string& dst) {
    rapidjson::Document doc;
    doc.SetObject();

    auto& allocator = doc.GetAllocator();
    doc.AddMember("id", item.id, allocator);
    doc.AddMember("score", item.score, allocator);
    doc.AddMember("coef", item.coef, allocator);
    doc.AddMember("offline", item.offline, allocator);
    doc.AddMember("brand", rapidjson::StringRef(item.brand.data()), allocator);

    rapidjson::Value arr;
    arr.SetArray();
    for (const auto& tmp : item.spuids) {
        arr.PushBack(tmp, allocator);
    }
    doc.AddMember("spuids", arr.Move(), allocator);

    rapidjson::Value strs_val;
    strs_val.SetObject();
    for (const auto& [k, v] : item.strs) {
        strs_val.AddMember(
            rapidjson::StringRef(k.data()), rapidjson::StringRef(v.data()), allocator);
    }
    doc.AddMember("strs", strs_val, allocator);

    rapidjson::Value dbl_val;
    dbl_val.SetObject();
    for (const auto& [k, v] : item.dbls) {
        dbl_val.AddMember(rapidjson::StringRef(k.data()), v, allocator);
    }
    doc.AddMember("dbls", dbl_val, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    dst.assign(buffer.GetString(), buffer.GetSize());
}

void item_to_json_by_sax(const Item& item, std::string& dst) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.Key("id");
    writer.Int64(item.id);
    writer.Key("score");
    writer.Double(item.score);
    writer.Key("coef");
    writer.Double(static_cast<double>(item.coef));
    writer.Key("offline");
    writer.Bool(item.offline);
    writer.Key("brand");
    writer.String(item.brand.c_str(), static_cast<rapidjson::SizeType>(item.brand.length()));

    writer.Key("spuids");
    writer.StartArray();
    for (const auto& spuid : item.spuids) {
        writer.Int64(spuid);
    }
    writer.EndArray();

    writer.Key("strs");
    writer.StartObject();
    for (const auto& kv : item.strs) {
        writer.Key(kv.first.c_str(), static_cast<rapidjson::SizeType>(kv.first.length()));
        writer.String(kv.second.c_str(), static_cast<rapidjson::SizeType>(kv.second.length()));
    }
    writer.EndObject();

    writer.Key("dbls");
    writer.StartObject();
    for (const auto& kv : item.dbls) {
        writer.Key(kv.first.c_str(), static_cast<rapidjson::SizeType>(kv.first.length()));
        writer.Double(kv.second);
    }
    writer.EndObject();

    writer.EndObject();

    dst.assign(buffer.GetString(), buffer.GetSize());
}

void item_to_json_by_pointer(const Item& item, std::string& dst) {
    rapidjson::Document d;
    d.SetObject();

    // Basic types using Pointer::Set
    rapidjson::Pointer("/id").Set(d, item.id);
    rapidjson::Pointer("/score").Set(d, item.score);
    rapidjson::Pointer("/coef").Set(d, (double)item.coef);
    rapidjson::Pointer("/offline").Set(d, item.offline);
    rapidjson::Pointer("/brand").Set(d, item.brand.c_str(), d.GetAllocator());

    // Vector
    rapidjson::Value spuids(rapidjson::kArrayType);
    for (auto s : item.spuids) {
        spuids.PushBack(s, d.GetAllocator());
    }
    rapidjson::Pointer("/spuids").Set(d, spuids);

    // Maps
    for (const auto& [k, v] : item.strs) {
        std::string path = "/strs/" + k;
        rapidjson::Pointer(path.c_str()).Set(d, v.c_str(), d.GetAllocator());
    }
    for (const auto& [k, v] : item.dbls) {
        std::string path = "/dbls/" + k;
        rapidjson::Pointer(path.c_str()).Set(d, v);
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    dst.assign(buffer.GetString(), buffer.GetSize());
}

static void BM_JsonByDom(benchmark::State& state) {
    Item item;
    for (auto _ : state) {
        std::string dst;
        item_to_json_by_doc(item, dst);
        benchmark::DoNotOptimize(dst);
    }
}

static void BM_JsonByPointer(benchmark::State& state) {
    Item item;
    for (auto _ : state) {
        std::string dst;
        item_to_json_by_pointer(item, dst);
        benchmark::DoNotOptimize(dst);
    }
}

static void BM_JsonBySax(benchmark::State& state) {
    Item item;
    for (auto _ : state) {
        std::string dst;
        item_to_json_by_sax(item, dst);
        benchmark::DoNotOptimize(dst);
    }
}

BENCHMARK(BM_JsonByDom);
BENCHMARK(BM_JsonByPointer);
BENCHMARK(BM_JsonBySax);

auto main(int argc, char** argv) -> int {
    benchmark::MaybeReenterWithoutASLR(argc, argv);
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}