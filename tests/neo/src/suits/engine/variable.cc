#include "script/neo.hpp"
#include <gtest/gtest.h>
class TestVariable : public ::testing::Test {};
class MockVariable : public neo::JSBase {
public:
  static inline size_t count = 0;
  MockVariable(neo::JSAllocator *allocator)
      : neo::JSBase(allocator, neo::JS_TYPE::OBJECT) {
    MockVariable::count++;
  }
  virtual ~MockVariable() { --MockVariable::count; }

public:
  neo::JSBase *toString() override { return this; }
  neo::JSBase *clone() override { return this; }
};
TEST_F(TestVariable, gc) {
  auto runtime = new neo::JSRuntime(0, nullptr);
  auto ctx = new neo::JSContext(runtime);
  ctx->getScope()->createValue(runtime->getAllocator()->create<MockVariable>());
  delete ctx;
  delete runtime;
  ASSERT_EQ(MockVariable::count, 0);
}
TEST_F(TestVariable, scope) {
  auto runtime = new neo::JSRuntime(0, nullptr);
  auto ctx = new neo::JSContext(runtime);
  ctx->pushScope();
  ctx->getScope()->createValue(runtime->getAllocator()->create<MockVariable>());
  ctx->popScope();
  ASSERT_EQ(MockVariable::count, 0);
  delete ctx;
  delete runtime;
}
TEST_F(TestVariable, closure) {
  auto runtime = new neo::JSRuntime(0, nullptr);
  auto ctx = new neo::JSContext(runtime);
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