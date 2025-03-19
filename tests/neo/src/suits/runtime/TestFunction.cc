#include "script/engine/JSCallableType.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSObjectType.hpp"
#include "script/engine/JSRuntime.hpp"
#include "script/util/JSRef.hpp"
#include <gtest/gtest.h>
class TestFunction : public testing::Test {
protected:
  JSRuntime *runtime;
  JSContext *ctx;
  void SetUp() override {
    runtime = new JSRuntime(0, NULL);
    ctx = new JSContext(runtime);
  }
  void TearDown() override {
    delete ctx;
    delete runtime;
    ASSERT_TRUE(JSRef::registry().empty());
  }
};
TEST_F(TestFunction, constructor) {
  auto Function =
      ctx->getField(ctx->getGlobal(), ctx->createString(L"Function"));
  ASSERT_TRUE(Function->isTypeof<JSCallableType>());
  ASSERT_TRUE(ctx->checkedBoolean(ctx->isEqual(
      Function, ctx->getField(Function, ctx->createString(L"constructor")))));
  ASSERT_TRUE(ctx->checkedBoolean(
      ctx->isEqual(ctx->getField(Function, ctx->createString(L"name")),
                   ctx->createString(L"Function"))));
}
TEST_F(TestFunction, prototype) {
  auto Function =
      ctx->getField(ctx->getGlobal(), ctx->createString(L"Function"));
  ASSERT_TRUE(Function->isTypeof<JSCallableType>());
  auto Object = ctx->getField(ctx->getGlobal(), ctx->createString(L"Object"));
  ASSERT_TRUE(Object->isTypeof<JSCallableType>());
  auto prototype = ctx->getField(Function, ctx->createString(L"prototype"));
  ASSERT_TRUE(prototype->isTypeof<JSObjectType>());
  ASSERT_TRUE(ctx->checkedBoolean(ctx->isEqual(
      Function, ctx->getField(prototype, ctx->createString(L"constructor")))));
  auto proto = ctx->getPrototypeOf(prototype);
  ASSERT_TRUE(proto->isTypeof<JSObjectType>());
  ASSERT_TRUE(ctx->checkedBoolean(ctx->isEqual(
      proto, ctx->getField(Object, ctx->createString(L"prototype")))));
}