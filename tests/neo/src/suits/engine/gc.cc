#include "script/neo.hpp"
#include <gtest/gtest.h>
class TestEngine : public ::testing::Test {};
class TestVariable : public neo::JSBase {
public:
  static inline size_t count = 0;
  TestVariable() : neo::JSBase(neo::JS_TYPE::OBJECT) { TestVariable::count++; }
  virtual ~TestVariable() { --TestVariable::count; }

public:
  neo::JSBase *toString() override { return this; }
  neo::JSBase *clone() override { return this; }
};
TEST_F(TestEngine, gc) {
  auto runtime = new neo::JSRuntime(0, nullptr);
  auto ctx = new neo::JSContext(runtime);
  ctx->getScope()->createValue(new TestVariable{});
  delete ctx;
  delete runtime;
  ASSERT_EQ(TestVariable::count, 0);
}
TEST_F(TestEngine, scope) {
  auto runtime = new neo::JSRuntime(0, nullptr);
  auto ctx = new neo::JSContext(runtime);
  ctx->pushScope();
  ctx->getScope()->createValue(new TestVariable{});
  ctx->popScope();
  ASSERT_EQ(TestVariable::count, 0);
  delete ctx;
  delete runtime;
}