#pragma once
#include "Accessor.hpp"
#include "TestSuit.hpp"
#include "core/Object.hpp"

using namespace firefly;
template struct AccessorBinding<"Object._ref", &core::Object::_ref>;
template struct AccessorBinding<"Object.addRef", &core::Object::addRef>;
using ObjectAccessor = Accessor<core::Object>;
class TestObject : public TestSuit<"TestObject", TestObject> {
private:
  void TEST_Object_ref() {
    core::Object object;
    CU_ASSERT_EQUAL(ObjectAccessor::get<"Object._ref">(object), 0);
    ObjectAccessor::call<"Object.addRef">(object);
    CU_ASSERT_EQUAL(ObjectAccessor::get<"Object._ref">(object), 1);
    object.subRef();
    CU_ASSERT_EQUAL(ObjectAccessor::get<"Object._ref">(object), 0);
  }

public:
  TestObject() { defineTest<"Object_ref">(&TestObject::TEST_Object_ref); }
};