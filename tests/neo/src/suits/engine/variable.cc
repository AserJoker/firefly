#include "script/engine/JSContext.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSRuntime.hpp"
#include "script/engine/JSValue.hpp"
#include "script/util/JSSingleton.hpp"
#include <gtest/gtest.h>

class TestVariable : public ::testing::Test {};
class MockVariable : public JSBase {
public:
  static inline size_t count = 0;
  MockVariable(JSAllocator *allocator)
      : JSBase(allocator, JSSingleton::instance<JSObjectType>(allocator)) {
    MockVariable::count++;
  }
  virtual ~MockVariable() { --MockVariable::count; }
};
TEST_F(TestVariable, gc) {
  auto runtime = new JSRuntime(0, nullptr);
  auto ctx = new JSContext(runtime);
  ctx->getScope()->createValue(runtime->getAllocator()->create<MockVariable>());
  delete ctx;
  delete runtime;
  ASSERT_EQ(MockVariable::count, 0);
}
TEST_F(TestVariable, scope) {
  auto runtime = new JSRuntime(0, nullptr);
  auto ctx = new JSContext(runtime);
  ctx->pushScope();
  ctx->getScope()->createValue(runtime->getAllocator()->create<MockVariable>());
  ctx->popScope();
  ASSERT_EQ(MockVariable::count, 0);
  delete ctx;
  delete runtime;
}
TEST_F(TestVariable, closure) {
  auto runtime = new JSRuntime(0, nullptr);
  auto ctx = new JSContext(runtime);
  auto host = ctx->getScope()->createValue(
      runtime->getAllocator()->create<MockVariable>());
  ctx->pushScope();
  auto item = ctx->getScope()->createValue(
      runtime->getAllocator()->create<MockVariable>());
  host->getAtom()->addChild(item->getAtom());
  ctx->popScope();
  ASSERT_EQ(MockVariable::count, 2);
  delete ctx;
  delete runtime;
  ASSERT_EQ(MockVariable::count, 0);
}