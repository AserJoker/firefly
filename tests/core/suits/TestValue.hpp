#pragma once
#include "TestSuit.hpp"
#include "core/Value.hpp"
class TestValue : public TestSuit<"TestValue", TestValue> {
private:
  void TEST_NumberAsString() {
    core::Value v = core::Byte_t(123);
    CU_ASSERT_FATAL(v.to<core::String_t>() == "123");
    v = core::Float_t(1.2);
    CU_ASSERT_EQUAL_FATAL(v.to<core::String_t>(), "1.2");
  }
  void TEST_StringAsNumber() {
    core::Value v = "1.2";
    CU_ASSERT_FATAL(std::fabs(v.to<core::Float_t>() - 1.2) < 0.0000001f);
    v = "1";
    CU_ASSERT_FATAL(v.to<core::Integer_t>() == 1);
  }

public:
  TestValue() {
    defineTest<"NumberAsString">(&TestValue::TEST_NumberAsString);
    defineTest<"StringAsNumber">(&TestValue::TEST_StringAsNumber);
  }
};