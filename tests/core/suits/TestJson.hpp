#pragma once
#include "TestSuit.hpp"
#include "core/Json.hpp"
#include "core/Value.hpp"
#include <CUnit/CUnit.h>
#include <cstddef>
#include <fmt/base.h>
class TestJson : public TestSuit<"TestJson", TestJson> {
private:
  void TEST_stringify() {
    core::Map_t object;
    object["null"] = nullptr;
    object["string"] = "Hello world";
    object["number"] = 123;
    object["boolean"] = true;
    std::string result = core::Json::stringify(object);

    auto next = core::Json::parse(result).get<core::Map_t>();

    CU_ASSERT_FATAL(next["null"].get<std::nullptr_t>() == nullptr);
    CU_ASSERT_FATAL(next["string"].get<core::String_t>() == "Hello world");
    CU_ASSERT_FATAL(next["number"].get<core::Float_t>() == 123);
    CU_ASSERT_FATAL(next["boolean"].get<core::Boolean_t>() == true);
  }
  void TEST_parse() {
    core::Value value =
        core::Json::parse("{\"boolean\":true,\"number\":123,\"string\":"
                          "\"Hello world\",\"null\":null}");
    CU_ASSERT_EQUAL_FATAL(value.getType(), core::ValueType::MAP);
    auto &object = value.get<core::Map_t>();
    CU_ASSERT_EQUAL_FATAL(object["null"].getType(), core::ValueType::NIL);
    CU_ASSERT_EQUAL_FATAL(object["string"].get<core::String_t>(),
                          "Hello world");
    CU_ASSERT_EQUAL_FATAL(object["number"].to<core::Integer_t>(), 123);
    CU_ASSERT_EQUAL_FATAL(object["boolean"].get<core::Boolean_t>(), true);
  }

public:
  TestJson() {
    defineTest<"stringify">(&TestJson::TEST_stringify);
    defineTest<"parse">(&TestJson::TEST_parse);
  }
};