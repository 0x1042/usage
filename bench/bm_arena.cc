#include <memory>

#include "benchmark/benchmark.h"
#include "google/protobuf/arena.h"
#include "lib/parameter.pb.h"

void without_arean(std::shared_ptr<idl::Parameter> param) {
    param->set_id(1);
    param->set_name("param1");
    param->set_dt(::idl::DataType::INT64);
    param->set_desc("param with value 1");
    param->set_alias("hello_args");
    auto* attr = param->mutable_attrs();

    for (int i = 0; i < 10; i++) {
        idl::Attr str;
        std::string key = std::to_string(i + 1);
        str.set_string_value(key);
        attr->emplace(key, std::move(str));
    }

    for (int i = 0; i < 10; i++) {
        idl::Attr str;
        std::string key = std::to_string(i + 1);
        str.set_int64_value(i + 1);
        attr->emplace(key, std::move(str));
    }
}

void with_arean(idl::Parameter* param) {
    param->set_id(1);
    param->set_name("param1");
    param->set_dt(::idl::DataType::INT64);
    param->set_desc("param with value 1");
    param->set_alias("hello_args");
    auto* attr = param->mutable_attrs();

    for (int i = 0; i < 10; i++) {
        idl::Attr str;
        std::string key = std::to_string(i + 1);
        str.set_string_value(key);
        attr->emplace(key, std::move(str));
    }

    for (int i = 0; i < 10; i++) {
        idl::Attr str;
        std::string key = std::to_string(i + 1);
        str.set_int64_value(i + 1);
        attr->emplace(key, std::move(str));
    }
}

static void BM_without_arean(benchmark::State& state) {
    for (auto _ : state) {
        auto param = std::make_shared<idl::Parameter>();
        without_arean(param);
        benchmark::DoNotOptimize(param);
    }
}

static void BM_with_arean(benchmark::State& state) {
    google::protobuf::ArenaOptions opt;
    opt.initial_block_size = 4096;
    google::protobuf::Arena arena(opt);
    for (auto _ : state) {
        auto* param = google::protobuf::Arena::Create<idl::Parameter>(&arena);
        with_arean(param);
        benchmark::DoNotOptimize(param);
    }
}

BENCHMARK(BM_without_arean);
BENCHMARK(BM_with_arean);