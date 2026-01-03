#include "absl/log/log.h"
#include "app/parameter.pb.h"
#include "google/protobuf/arena.h"
#include "gtest/gtest.h"

// TEST(proto, arena) {
//     google::protobuf::ArenaOptions opt;
//     opt.start_block_size = 4096;
//     opt.max_block_size = 65536;

//     google::protobuf::Arena arean(opt);
//     auto* parameter = ::google::protobuf::Arena::Create<idl::Parameter>(&arean);
//     parameter->set_name("hello");
//     EXPECT_EQ(parameter->name(), "hello");
// }

// TEST(proto, reflect) {
//     google::protobuf::ArenaOptions opt;
//     opt.start_block_size = 4096;
//     opt.max_block_size = 65536;

//     google::protobuf::Arena arean(opt);
//     auto* parameter = ::google::protobuf::Arena::Create<idl::Parameter>(&arean);
//     parameter->set_name("hello");
//     parameter->set_alias("clazz_hello");

//     const auto* descriptor = idl::Parameter::GetDescriptor();
//     const auto* reflection = idl::Parameter::GetReflection();

//     LOG(INFO) << "Parameter field_count: " << descriptor->field_count();

//     for (int i = 0; i < descriptor->field_count(); ++i) {
//         const google::protobuf::FieldDescriptor* field = descriptor->field(i);
//         LOG(INFO) << "field info. " << field->name() << " " << field->full_name() << " "
//                   << field->cpp_type_name();

//         if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_STRING) {
//             LOG(INFO) << "string type. " << reflection->GetString(*parameter, field);
//         }
//     }
// }
