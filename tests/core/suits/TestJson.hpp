#pragma once
#include "TestSuit.hpp"
#include "core/Json.hpp"
#include "core/Value.hpp"
#include <CUnit/CUnit.h>
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
    CU_ASSERT_EQUAL_FATAL(result, "{\"boolean\":true,\"number\":123,\"string\":"
                                  "\"Hello world\",\"null\":null}");
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