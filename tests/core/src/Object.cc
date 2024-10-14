#include "core/Object.hpp"
#include "Accessor.hpp"
#include "TestSuit.hpp"
#include "Testing.hpp"
#include <CUnit/CUnit.h>
#include <SDL2/SDL.h>
using namespace firefly;
template struct AccessorBinding<"Object._ref", &core::Object::_ref>;
using ObjectAccessor = Accessor<core::Object>;
class TestObject : public TestSuit<"Object", TestObject> {
private:
  void TEST_Object_ref() {
    core::Object object;
    CU_ASSERT_EQUAL(ObjectAccessor::get<"Object._ref">(object), 0);
    object.addRef();
    CU_ASSERT_EQUAL(ObjectAccessor::get<"Object._ref">(object), 1);
    object.subRef();
    CU_ASSERT_EQUAL(ObjectAccessor::get<"Object._ref">(object), 0);
  }

public:
  TestObject() { defineTest<"Object_ref">(&TestObject::TEST_Object_ref); }
};

int main(int argc, char *argv[]) {
  Testing<TestObject> testing;
  return testing.run(argc, argv);
}