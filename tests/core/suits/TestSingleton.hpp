#pragma once
#include "TestSuit.hpp"
#include "core/Object.hpp"
#include "core/Singleton.hpp"
#include <CUnit/CUnit.h>
class TestSingleton : public TestSuit<"TestSingleton", TestSingleton> {
  struct Test : public core::Object {};
  struct Test2 : public core::Object {
    int _data;
    Test2(int data) { _data = data; }
  };

protected:
  int afterAll() override {
    core::Singleton<Test>::instance() = nullptr;
    core::Singleton<Test2>::instance() = nullptr;
    return 0;
  }
  int beforeAll() override {
    core::Singleton<Test>::instance() = nullptr;
    core::Singleton<Test2>::instance() = nullptr;
    return 0;
  }

private:
  void TEST_default_initialize() {
    auto a = core::Singleton<Test>::instance();
    CU_ASSERT_NOT_EQUAL(a, nullptr);
  }

  void TEST_custom_initialize() {
    auto b = core::Singleton<Test2>::instance();
    CU_ASSERT_EQUAL_FATAL(b, nullptr);
    core::Singleton<Test2>::initialize(123);
    b = core::Singleton<Test2>::instance();
    CU_ASSERT_NOT_EQUAL(b, nullptr);
    CU_ASSERT_EQUAL_FATAL(b->_data, 123);
  }

public:
  TestSingleton() {
    defineTest<"default_initialize">(&TestSingleton::TEST_default_initialize);
    defineTest<"custom_initialize">(&TestSingleton::TEST_custom_initialize);
  }
};